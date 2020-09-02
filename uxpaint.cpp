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
  PangoColor pangoColor;
  if (pango_color_parse(&pangoColor, s.data())) {
    _r = pangoColor.red / 65535.0;
    _g = pangoColor.green / 65535.0;
    _b = pangoColor.blue / 65535.0;
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
  if (!data_storage->is_valid())
    return false;

  // if a description was provided, determine how it should be interpreted
  if (data_storage->class_type == paint_definition_class_t::descriptive) {
    auto &_image = read_image(data_storage->description, _width, _height);

    if (_image) {

      data_storage=std::make_shared<image_block_pattern_source_definition_t>(data_storage->description,
             cairo_image_surface_get_width(_image), cairo_image_surface_get_height(_image),
             _image, filter_t::fast, extend_t::repeat);

      // determine if the description is another form such as a gradient or
      // color.
    } else if (is_linear_gradient_description()) {
      _gradientType = gradientType::linear;

    } else if (is_radial_gradient_description()) {
      _gradientType = gradientType::radial;

    } else if (is_patch_description()) {

    } else if (pango_color_parse(&_pangoColor, _description.data())) {
      r = _pangoColor.red / 65535.0;
      g = _pangoColor.green / 65535.0;
      b = _pangoColor.blue / 65535.0;
      a = 1;
      is_loaded = true;
    }
  }

  // still more processing to do, -- create gradients
  // the parsing above for gradients populates this data.
  // so this logic is used in duel form. When gradients may be
  // named as a string, or provided in complete API form.
  // the logic below fills in the offset values automatically distributing
  // equally across the noted offset. offsets are provided from 0 - 1
  // and name the point within the line.
  if (!is_loaded && stops.size() > 0) {

    if (_gradientType == gradientType::linear) {
      pattern = cairo_pattern_create_linear(x0, y0, x1, y1);

    } else if (_gradientType == gradientType::radial) {
      pattern =
          cairo_pattern_create_radial(cx0, cy0, radius0, cx1, cy1, radius1);
    }
    // provide auto offsets
    if (pattern) {
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
          cairo_pattern_add_color_stop_rgba(_pattern, n._offset, n._r, n._g,
                                            n._b, n._a);
        else
          cairo_pattern_add_color_stop_rgb(_pattern, n._offset, n._r, n._g,
                                           n._b);
      });

      cairo_pattern_set_extend(_pattern, CAIRO_EXTEND_REPEAT);
      is_loaded = true;
    }
  }

  return _is_loaded;
}

/**
\brief The routine is called by area, text or other rendering attribute
areas when the color style is needed for painting. The cairo context is
passed. The paint is loaded if need be. File processing or simply parsing
the color name string.

*/
void uxdevice::painter_brush_t::emit(cairo_t *cr) {
  if (!is_loaded)
    create();

  if (is_loaded) {
    data_storage->emit(cr);
  }
}

void uxdevice::painter_brush_t::emit(cairo_t *cr, double x, double y, double w,
                                     double h) {
  if (!is_loaded) {
    create();

    // adjust to user space
    if (class_type == paint_definition_class_t::linear_gradient ||
        class_type == paint_definition_class_t::radial_gradient ||
        class_type == paint_definition_class_t::image_block_pattern)
      translate(-x, -y);
  }

  if (is_loaded) {
    data_storage->emit(cr, x, y, w, h);
  }
}
