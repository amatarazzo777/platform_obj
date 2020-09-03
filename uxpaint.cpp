/**
\author Anthony Matarazzo
\file uxdisplaycontext.hpp
\date 5/12/20
\version 1.0
 \details CLass provides the painting object interface which invokes the
 appropriate cairo api. The notable virtual method is the emit function
 which applies the cairo text_color setting.

*/
#include "uxdevice.hpp"

/**
\brief color stops interface
*/
uxdevice::color_stop_t::color_stop_t(u_int32_t c) : color_stop_t(-1, c) {
  _bAutoOffset = true;
}

uxdevice::color_stop_t::color_stop_t(double o, u_int32_t c) {
  _bRGBA = false;
  _bAutoOffset = false;
  _offset = o;
  _r = static_cast<u_int8_t>(c >> 16) / 255.0;
  _g = static_cast<u_int8_t>(c >> 8) / 255.0;
  _b = static_cast<u_int8_t>(c) / 255.0;
  _a = 1.0;
}

uxdevice::color_stop_t::color_stop_t(double r, double g, double b)
    : _bAutoOffset(true), _bRGBA(false), _offset(-1), _r(r), _g(g), _b(b),
      _a(1) {}

uxdevice::color_stop_t::color_stop_t(double o, double r, double g, double b)
    : _bAutoOffset(false), _bRGBA(false), _offset(o), _r(r), _g(g), _b(b),
      _a(1) {}

uxdevice::color_stop_t::color_stop_t(double o, double r, double g, double b,
                                     double a)
    : _bAutoOffset(false), _bRGBA(true), _offset(o), _r(r), _g(g), _b(b),
      _a(a) {}

uxdevice::color_stop_t::color_stop_t(const std::string &s)
    : color_stop_t(-1, s) {
  _bAutoOffset = true;
}
uxdevice::color_stop_t::color_stop_t(const std::string &s, double a)
    : color_stop_t(-1, s, a) {
  _bAutoOffset = true;
}
uxdevice::color_stop_t::color_stop_t(double o, const std::string &s) {
  _bAutoOffset = false;
  _bRGBA = false;
  _offset = o;
  parse_color(s);
}

uxdevice::color_stop_t::color_stop_t(double o, const std::string &s, double a) {
  _bAutoOffset = false;
  _bRGBA = true;
  _offset = o;
  _a = a;
  parse_color(s);
}

void uxdevice::color_stop_t::parse_color(const std::string &s) {
  PangoColor pango_color;
  if (pango_color_parse(&pango_color, s.data())) {
    _r = pango_color.red / 65535.0;
    _g = pango_color.green / 65535.0;
    _b = pango_color.blue / 65535.0;
  }
}

/**
\brief The routine handles the creation of the pattern or surface.
Patterns can be an image_block file, a description of a linear, actual
parameters of linear, a description of a radial, the actual radial parameters
stored. SVG inline or a base64 data set.

*/
bool uxdevice::painter_brush_t::create(void) {

  // already created,
  if (data_storage->is_loaded)
    return true;
  if (!is_valid())
    return false;

  // if a description was provided, determine how it should be interpreted
  if (data_storage->class_type == paint_definition_class_t::descriptive) {
    double _width = {};
    double _height = {};

    auto _image = read_image(data_storage->description, _width, _height);

    if (_image) {

      data_storage = std::make_shared<image_block_pattern_source_definition_t>(
          data_storage->description, cairo_image_surface_get_width(_image),
          cairo_image_surface_get_height(_image), _image, filter_t::fast,
          extend_t::repeat);

      // determine if the description is another form such as a gradient or
      // color.
    } else if (data_storage->is_linear_gradient_description()) {
      data_storage = std::make_shared<linear_gradient_definition_t>(
          data_storage->description);

    } else if (data_storage->is_radial_gradient_description()) {
      data_storage = std::make_shared<radial_gradient_definition_t>(
          data_storage->description);

    } else if (data_storage->is_patch_description()) {

    } else if (data_storage->is_text_color_description()) {
      data_storage =
          std::make_shared<color_definition_t>(data_storage->description);
    }
  }

  // still more processing to do, -- create gradients
  // the parsing above for gradients populates this data.
  // so this logic is used in duel form. When gradients may be
  // named as a string, or provided in complete API form.
  // the logic below fills in the offset values automatically distributing
  // equally across the noted offset. offsets are provided from 0 - 1
  // and name the point within the line.
  if (!data_storage->is_loaded) {
    color_stops_t *ptr_cs = {};
    cairo_pattern_t *ptr_cp = {};

    switch (data_storage->class_type) {
    case paint_definition_class_t::linear_gradient: {
      auto p =
          std::dynamic_pointer_cast<linear_gradient_definition_t>(data_storage);
      p->pattern = cairo_pattern_create_linear(p->x0, p->y0, p->x1, p->y1);
      ptr_cp = p->pattern;
      ptr_cs = &p->cs;
    } break;
    case paint_definition_class_t::radial_gradient: {
      auto p =
          std::dynamic_pointer_cast<radial_gradient_definition_t>(data_storage);
      p->pattern = cairo_pattern_create_radial(p->cx0, p->cy0, p->radius0,
                                               p->cx1, p->cy1, p->radius1);
      ptr_cp = p->pattern;
      ptr_cs = &p->cs;
    } break;
    case paint_definition_class_t::none: {} break;
    case paint_definition_class_t::descriptive: {} break;
    case paint_definition_class_t::color: {} break;
    case paint_definition_class_t::image_block_pattern: {} break;
    }

    color_stops_t &_stops = *ptr_cs;
    if (_stops.size() > 0 && ptr_cp) {
      bool bDone = false;
      bool bEdgeEnd = false;

      // first one, if auto offset set to
      //   0 - the beginning of the color stops
      color_stops_iterator_t it = _stops.begin();
      if (it->_bAutoOffset) {
        it->_bAutoOffset = false;
        it->_offset = 0;
      }
      double dOffset = it->_offset;

      while (!bDone) {

        // find first color stop with a defined offset.
        color_stops_iterator_t it2 =
            find_if(it + 1, _stops.end(),
                    [](auto const &o) { return !o._bAutoOffset; });

        // not found, the last item in color stops did not have a value,
        // assign it 1.0
        if (it2 == _stops.end()) {
          bEdgeEnd = true;
          bDone = true;
          // very last one has a setting
        } else if (it2 == _stops.end() - 1) {
          bDone = true;
        }

        // distribute offsets equally across range
        int ncolor_stops_t = std::distance(it, it2);
        if (bEdgeEnd)
          ncolor_stops_t--;

        if (ncolor_stops_t > 0) {
          double incr = 0;
          if (bEdgeEnd) {
            incr = (1 - it->_offset) / ncolor_stops_t;
          } else {
            incr = (it2->_offset - it->_offset) / ncolor_stops_t;
            ncolor_stops_t--;
          }

          dOffset = it->_offset;
          while (ncolor_stops_t) {
            it++;
            dOffset += incr;
            it->_offset = dOffset;
            it->_bAutoOffset = false;
            ncolor_stops_t--;
          }
        }
        // forward to next range
        it = it2;
      }
      // add the color stops
      std::for_each(_stops.begin(), _stops.end(), [=](auto &n) {
        if (n._bRGBA)
          cairo_pattern_add_color_stop_rgba(ptr_cp, n._offset, n._r, n._g, n._b,
                                            n._a);
        else
          cairo_pattern_add_color_stop_rgb(ptr_cp, n._offset, n._r, n._g, n._b);
      });

      cairo_pattern_set_extend(ptr_cp, CAIRO_EXTEND_REPEAT);
      data_storage->is_loaded = true;
    }
  }

  return data_storage->is_loaded;
}

/**
\brief The routine is called by area, text or other rendering attribute
areas when the color style is needed for painting. The cairo context is
passed. The paint is loaded if need be. File processing or simply parsing
the color name string.

*/
void uxdevice::painter_brush_t::emit(cairo_t *cr) {
  if (!data_storage->is_loaded)
    create();

  if (data_storage->is_loaded) {
    data_storage->emit(cr);
  }
}

void uxdevice::painter_brush_t::emit(cairo_t *cr, double x, double y, double w,
                                     double h) {
  if (!data_storage->is_loaded) {
    create();

    // adjust to user space
    if (data_storage->class_type == paint_definition_class_t::linear_gradient ||
        data_storage->class_type == paint_definition_class_t::radial_gradient ||
        data_storage->class_type ==
            paint_definition_class_t::image_block_pattern)
      translate(-x, -y);
  }

  if (data_storage->is_loaded) {
    data_storage->emit(cr, x, y, w, h);
  }
}
