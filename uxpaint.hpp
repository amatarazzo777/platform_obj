/**
\author Anthony Matarazzo
\file uxevent.hpp
\date 5/12/20
\version 1.0
 \details  paint class

*/
#pragma once

namespace uxdevice {
/**
\class painter_brush_t

\brief interface for the paint class.

*/
enum class paintType { none, color, pattern, image_block };
enum class gradientType { none, linear, radial };

template <typename T, typename... Rest>
void hash_combine(std::size_t &seed, const T &v, const Rest &... rest) {
  seed ^= std::hash<T>{}(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
  (hash_combine(seed, rest), ...);
}

class color_stop_t {
public:
  color_stop_t(u_int32_t c);
  color_stop_t(double r, double g, double b);
  color_stop_t(const std::string &s);
  color_stop_t(const std::string &s, double a);

  color_stop_t(double o, u_int32_t c);
  color_stop_t(double o, double r, double g, double b);
  color_stop_t(double o, double r, double g, double b, double a);
  color_stop_t(double o, const std::string &s);
  color_stop_t(double o, const std::string &s, double a);
  void parse_color(const std::string &s);

  bool _bAutoOffset = false;
  bool _bRGBA = false;
  double _offset = 0;
  double _r = 0;
  double _g = 0;
  double _b = 0;
  double _a = 1;
};
typedef std::vector<color_stop_t> color_stops_t;
typedef std::vector<color_stop_t>::iterator color_stops_iterator_t;

/**
 parse web formats

linear-gradient(to bottom, #1e5799 0%,#2989d8 50%,#207cca 51%,#2989d8
51%,#7db9e8 100%);

linear-gradient(to right, #1e5799 0%,#2989d8 50%,#207cca
51%,#2989d8 51%,#7db9e8 100%);

linear-gradient(135deg, #1e5799 0%,#2989d8
50%,#207cca 51%,#2989d8 51%,#7db9e8 100%);

linear-gradient(45deg, #1e5799
0%,#2989d8 50%,#207cca 51%,#2989d8 51%,#7db9e8 100%);

radial-gradient(ellipsize at center, #1e5799 0%,#2989d8 50%,#207cca
51%,#2989d8 51%,#7db9e8 100%);

*/

class painter_brush_t : public Matrix {
public:
  const std::string_view sLinearPattern = "linear-gradient";
  const std::string_view sRadialPattern = "radial-gradient";
  enum class paint_definition_class_t {
    none,
    descriptive,
    color,
    linear_gradient,
    radial_gradient,
    image_block_pattern
  };

  class paint_definition_base_t {
  public:
    paint_definition_base_t(paint_definition_class_t _ct,
                            const std::string &_description)
        : class_type(ct), description(_description) {}

    virtual void emit(cairo_t *cr);
    virtual void emit(cairo_t *cr, double x, double y, double w, double h);

    bool is_linear_gradient_description(const std::string &s) {
      if (s.compare(0, sLinearPattern.size(), sLinearPattern) != 0)
        return false;

      return true;
    }
    bool is_radial_gradient_description(const std::string &s) {
      if (s.compare(0, sRadialPattern.size(), sRadialPattern) != 0)
        return false;

      return true;
    }

    bool uxdevice::painter_brush_t::is_patch_description(const std::string &s) {
      return false;
    }

    std::size_t hash_value(void) {
      std::size_t value = {};
      hash_combine(value, class_type, description, pango_color, is_loaded);
      return value;
    }
    paint_definition_class_t class_type = paint_definition_class_t::none;
    std::string description = {};
    PangoColor pango_color = {0, 0, 0};
    bool is_loaded = false;
    bool is_valid(void) {
      return data_storage!=nullptr;
    }
  };

  class descriptive_definition_t : public paint_definition_base_t {
  public:
    descriptive_definition_t(const std::string &_description)
        : paint_definition_base_t(paint_definition_class_t::descriptive,
                                  _description) {}
    std::size_t hash_value(void) {
      std::size_t value = {};
      hash_combine(value, paint_definition_base_t::hash_value());
      return value;
    }
  };

  class color_definition_t : public paint_definition_base_t {
  public:
    color_definition_t(const std::string &_description, double _r, double _g,
                       double _b, double _a)
        : paint_definition_base_t(paint_definition_class_t::color,
                                  _description),
          r(_r), g(_g), b(_b), a(_a) {}

    virtual void emit(cairo_t *cr) { cairo_set_source_rgba(cr, r, g, b, a); }
    virtual void emit(cairo_t *cr, double x, double y, double w, double h) {
      cairo_set_source_rgba(cr, r, g, b, a);
    }

    std::size_t hash_value(void) {
      std::size_t value = {};
      hash_combine(value, paint_definition_base_t::hash_value(), r, g, b, a);
      return value;
    }
    double r = {};
    double g = {};
    double b = {};
    double a = {};
  };

  class linear_gradient_definition_t : public paint_definition_base_t {
  public:
    linear_gradient_definition_t(const std::string &_description, double _x0,
                                 double _y0, double _x1, double _y1,
                                 color_stops_t &__cs, filter_t _filter,
                                 extend_t _extend)
        : paint_definition_base_t((paint_definition_class_t::linear_gradient,_description), x0(_x0), y0(_x0), x1(_x0),
          y1(_x0), cs(__cs), filter(_filter), extend(_extend) {}

    ~linear_gradient_definition_t() {
      if (_pattern)
        cairo_pattern_destroy(_pattern);
    }
    virtual void emit(cairo_t *cr) {
      cairo_pattern_set_matrix(pattern, &matrix);
      cairo_set_source(cr, pattern);
    }
    virtual void emit(cairo_t *cr, double x, double y, double w, double h) {
      cairo_pattern_set_matrix(pattern, &matrix);
      cairo_set_source(cr, pattern);
    }

    std::size_t hash_value(void) {
      std::size_t value = {};
      hash_combine(value, paint_definition_base_t::hash_value(), x0, y0, x1, y1,
                   cs, filter, extend, pattern);
      return value;
    }
    double x0 = {};
    double y0 = {};
    double x1 = {};
    double y1 = {};
    color_stops_t &cs = {};
    filter_t filter = {};
    extend_t extend = {};
    cairo_pattern_t *pattern = {};
  };

  class radial_gradient_definition_t : public paint_definition_base_t {
  public:
    radial_gradient_definition_t(const std::string &_description, double _cx0,
                                 double _cy0, double _radius0, double _cx1,
                                 double _cy1, double _radius1,
                                 const color_stops_t &_cs)
        : paint_definition_base_t(paint_definition_class_t::radial_gradient,
                                  _description),
          cx0(_cx0), cy0(_cy0), radius0(_radius0), cx1(cx1), cy1(_cy),
          radius1(_radius1), cs(_cs) {}
    ~radial_gradient_definition_t() {
      if (pattern)
        cairo_pattern_destroy(pattern);
    }
    virtual void emit(cairo_t *cr) {
      cairo_pattern_set_matrix(pattern, &matrix);
      cairo_set_source(cr, pattern);
    }
    virtual void emit(cairo_t *cr, double x, double y, double w, double h) {
      cairo_pattern_set_matrix(pattern, &matrix);
      cairo_set_source(cr, pattern);
    }

    std::size_t hash_value(void) {
      hash_combine(value, paint_definition_base_t::hash_value(),  cx0,
                 cy0, radius0, cx1, cy1, radius1, cs, filter,
                 extend, pattern));
      return value;
    }
    double cx0 = {};
    double cy0 = {};
    double radius0 = {};
    double cx1 = {};
    double cy1 = {};
    double radius1 = {};
    color_stops_t &cs = {};
    filter_t filter = {};
    extend_t extend = {};
    cairo_pattern_t *pattern = {};
  };

  class image_block_pattern_source_definition_t
      : public paint_definition_base_t {
  public:
    image_block_pattern_source_definition_t(const std::string &_description,
                                            double _width, double _height)
        : paint_definition_base_t(
              paint_definition_class_t::image_block_pattern),
          width(_width), height(_height) {}
    image_block_pattern_source_definition_t(const std::string &_description,
                                            double _width, double _height,
                                            cairo_surface_t *_image,
                                            filter_t _filter, extend_t _extend)
        : paint_definition_base_t(paint_definition_class_t::image_block_pattern,
                                  _description),
          width(_width), height(_height), image_block(_image), filter(_filter),
          extend(_extend) {
      pattern = cairo_pattern_create_for_surface(image_block);
      cairo_pattern_set_extend(_pattern, CAIRO_EXTEND_REPEAT);
      cairo_pattern_set_filter(_pattern, CAIRO_FILTER_FAST);
    }
    ~image_block_pattern_source_definition_t() {
      if (image_block)
        cairo_surface_destroy(image_block);
      if (pattern)
        cairo_pattern_destroy(pattern);
    }
    virtual void emit(cairo_t *cr) {
      cairo_pattern_set_matrix(pattern, &matrix);
      cairo_set_source(cr, pattern);
    }
    virtual void emit(cairo_t *cr, double x, double y, double w, double h) {
      cairo_pattern_set_matrix(pattern, &matrix);
      cairo_set_source(cr, pattern);
    }

    std::size_t hash_value(void) {
      hash_combine(value, paint_definition_base_t::hash_value(), width, height,
                   image_block, pattern, filter, extend);
      return value;
    }
    double width = {};
    double height = {};
    cairo_surface_t *image_block = {};
    cairo_pattern_t *pattern = {};
    filter_t filter = {};
    extend_t extend = {};
  };

  std::shared_ptr<paint_definition_base_t> data_storage_t = {};
  /**
  \brief color given as a uint32 value
  */
  painter_brush_t(u_int32_t c) {

    _r = static_cast<u_int8_t>(c >> 16) / 255.0;
    _g = static_cast<u_int8_t>(c >> 8) / 255.0;
    _b = static_cast<u_int8_t>(c) / 255.0;
    _a = 1.0;

    data_storage= std::make_shared<color_definition_t>(r,g,b,a,"u_int32_t RGB", PangoColor{},true));
  }

  painter_brush_t(double r, double g, double b)
      : data_storage(
            std::make_shared<color_definition_t>("RGB", r, g, b, 1.0)) {}

  painter_brush_t(double r, double g, double b, double a)
      : data_storage(std::make_shared<color_definition_t>("RGBA", r, g, b, a)) {
  }

  // color given as a description
  painter_brush_t(const std::string &n)
      : data_storage(std::make_shared<descriptive_definition_t>(n)) {}

  painter_brush_t(const std::string &n, double width, double height)
      : data_storage(std::make_shared<image_block_pattern_source_definition_t>(
            n, width, height)) {}

  painter_brush_t(double x0, double y0, double x1, double y1,
                  const color_stops_t &cs)
      : data_storage(std::make_shared<linear_gradient_definition_t>(
            "linear_gradient", x0, y0, x1, y1, cs, filter_t::none,
            extend_t::none)) {}

  // specify a radial gradient
  painter_brush_t(double cx0, double cy0, double radius0, double cx1,
                  double cy1, double radius1, const color_stops_t &cs)
      : data_storage(std::make_shared<radial_gradient_definition_t>(
            "radial_gradient", cx0, cy0, radius0, cx1, cy1, radius1, cs,
            filter_t::none, extend_t::none, _radius0(radius0), _cx1(cx1),
            _cy1(cy1), _radius1(radius1), _stops(cs))) {}

  painter_brush_t(const painter_brush_t &other) { *this = other; }
  painter_brush_t &operator=(const painter_brush_t &other) {
    Matrix::operator=(other);
    data_storage = other.data_storage;
    return *this;
  }
  painter_brush_t(painter_brush_t &&other) {
    data_storage = std::move(other.data_storage);
  }

  virtual ~painter_brush_t() {
    if (data_storage)
      data_storage.reset();
  }

  virtual void emit(cairo_t *cr);
  virtual void emit(cairo_t *cr, double x, double y, double w, double h);

private:
  bool create(void);
  bool is_loaded(void) const { return _is_loaded; }
  bool is_linear_gradient(const std::string &s);
  bool is_radial_gradient(const std::string &s);
  bool patch(const std::string &s);
  std::size_t current_hash = {};

public:
  data_storage_t data_storage = {};
}; // class painter_brush_t

// from -
// https://stackoverflow.com/questions/2590677/how-do-i-combine-hash-values-in-c0x

} // namespace uxdevice

#define STD_HASHABLE(CLASS_NAME)                                               \
  template <> struct std::hash<CLASS_NAME> {                                   \
    std::size_t operator()(CLASS_NAME const &o) const noexcept {               \
      return o.hash_value();                                                   \
    }                                                                          \
  }

STD_HASHABLE(uxdevice::painter_brush_t::paint_definition_base_t);
STD_HASHABLE(uxdevice::painter_brush_t::descriptive_definition_t);
STD_HASHABLE(uxdevice::painter_brush_t::color_definition_t);
STD_HASHABLE(uxdevice::painter_brush_t::linear_gradient_definition_t);
STD_HASHABLE(uxdevice::painter_brush_t::radial_gradient_definition_t);
STD_HASHABLE(
    uxdevice::painter_brush_t::image_block_pattern_source_definition_t);
