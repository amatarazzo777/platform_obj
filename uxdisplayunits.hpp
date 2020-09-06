/**
\author Anthony Matarazzo
\file uxdisplay_unit_ts.hpp
\date 5/12/20
\version 1.0
 \details

*/
#pragma once

namespace uxdevice {
typedef std::variant<std::string, std::size_t> indirect_index_t;

class index_by {
public:
  index_by() {}
  virtual ~index_by() {}

  index_by(std::string _k) : key(_k) {}
  index_by(std::size_t _k) : key(_k) {}

  index_by &operator=(const index_by &other) {
    key = other.key;
    return *this;
  }
  index_by(const index_by &other) { *this = other; }

  indirect_index_t key = {};
};
} // namespace uxdevice

namespace uxdevice {
/**
\brief base class for all display units. defaulted
is the is_output function. Drawing object should override
this and return true. This enables the checking of the surface
for errors after invocation.

*/
typedef std::function<void(cairo_t *cr)> cairo_function;
using cairo_function = cairo_function;

typedef std::variant<std::string, std::shared_ptr<std::string>, double,
                     std::vector<double>, std::size_t, painter_brush_t,
                     line_cap_t, alias_t, filter_t, extend_t, line_join_t, op_t,
                     alignment_t, ellipsize_t, cairo_function>
    display_unit_storage_t;

} // namespace uxdevice
template <> struct std::hash<uxdevice::display_unit_storage_t> {
  std::size_t
  operator()(uxdevice::display_unit_storage_t const &o) const noexcept {
    size_t value = o.index();
    if (auto pval = std::get_if<std::string>(&o))
      uxdevice::hash_combine(value, *pval);
    else if (auto pval = std::get_if<double>(&o))
      uxdevice::hash_combine(value, *pval);
    else if (auto pval = std::get_if<std::vector<double>>(&o)) {
      for(auto n:*pval)
        uxdevice::hash_combine(value, n);
    } else if (auto pval = std::get_if<std::size_t>(&o))
      uxdevice::hash_combine(value, *pval);
    else if (auto pval = std::get_if<uxdevice::painter_brush_t>(&o))
      uxdevice::hash_combine(value, pval->hash_code());
    else if (auto pval = std::get_if<uxdevice::line_cap_t>(&o))
      uxdevice::hash_combine(value, *pval);
    else if (auto pval = std::get_if<uxdevice::alias_t>(&o))
      uxdevice::hash_combine(value, *pval);
    else if (auto pval = std::get_if<uxdevice::filter_t>(&o))
      uxdevice::hash_combine(value, *pval);
    else if (auto pval = std::get_if<uxdevice::extend_t>(&o))
      uxdevice::hash_combine(value, *pval);
    else if (auto pval = std::get_if<uxdevice::line_join_t>(&o))
      uxdevice::hash_combine(value, *pval);
    else if (auto pval = std::get_if<uxdevice::op_t>(&o))
      uxdevice::hash_combine(value, *pval);
    else if (auto pval = std::get_if<uxdevice::alignment_t>(&o))
        uxdevice::hash_combine(value, *pval);
    else if (auto pval = std::get_if<uxdevice::ellipsize_t>(&o))
        uxdevice::hash_combine(value, *pval);

    return value;
  }
};

namespace uxdevice {
class display_unit_t : public index_by {
public:
  display_unit_t() : _data(0.0) {}
  display_unit_t(const display_unit_storage_t &_val) : _data(_val) {}
  display_unit_t(const std::vector<double> &_val, const double _off)
      : _data(_val), _offset(_off) {}
  display_unit_t(const double &d1, const double &d2)
      : _data(std::vector<double>({d1, d2})) {}
  display_unit_t(painter_brush_t &p, const double &d1, const double &d2)
      : _data(std::vector<double>({d1, d2})) {}
  display_unit_t(const double &d1, const double &d2, const double &d3,
                 const double &d4)
      : _data(std::vector<double>({d1, d2, d3, d4})) {}
  display_unit_t(const double &d1, const double &d2, const double &d3,
                 const double &d4, const double &d5)
      : _data(std::vector<double>({d1, d2, d3, d4, d5})) {}
  display_unit_t(const double &d1, const double &d2, const double &d3,
                 const double &d4, const double &d5, const double &d6)
      : _data(std::vector<double>({d1, d2, d3, d4, d5, d6})) {}
  display_unit_t(const cairo_function &_fn) : _data(_fn) {}

  virtual ~display_unit_t() {}
  display_unit_t &operator=(const display_unit_t &other) {
    is_processed = other.is_processed;
    viewport_inked = other.viewport_inked;
    _serror = other._serror;
    return *this;
  }
  display_unit_t(const display_unit_t &other) {
    _data = other._data;
    key = other.key;
  }
  display_unit_t &operator=(const double &s) {
    _data = s;
    return *this;
  }
  display_unit_t &operator=(const std::string &s) {
    _data = s;
    return *this;
  }
  display_unit_t(display_unit_t &&other) { _data = std::move(other._data); }

  virtual void invoke(display_context_t &context) {}
  virtual bool is_output(void) { return false; }
  void error(const char *s) { _serror = s; }
  bool valid(void) { return _serror == nullptr; }

  display_unit_storage_t _data = {};

  HASH_OBJECT_MEMBERS(std::type_index(typeid(this)).hash_code(), _data, _offset,
                      _serror)

  double _offset = {};
  bool is_processed = false;
  bool viewport_inked = false;
  const char *_serror = nullptr;
};
} // namespace uxdevice
STD_HASHABLE(uxdevice::display_unit_t);

/**
\brief base class for objects that produce image_block drawing commands
The is_output is overridden to return true. As well the object uses
the render work list to determine if a particular image_block is on screen.

*/
namespace uxdevice {
class drawing_output_t : public display_unit_t {
public:
  typedef display_context_t::context_cairo_region_t context_cairo_region_t;
  drawing_output_t() : display_unit_t() {}
  drawing_output_t(const display_unit_storage_t &_val) : display_unit_t(_val) {}
  drawing_output_t(const std::vector<double> &_val, const double &_off)
      : display_unit_t(_val, _off) {}
  drawing_output_t(const double &d1, const double &d2)
      : display_unit_t(d1, d2) {}

  drawing_output_t(painter_brush_t &p, const double &d1, const double &d2)
      : display_unit_t(p, d1, d2) {}
  drawing_output_t(const double &d1, const double &d2, const double &d3,
                   const double &d4)
      : display_unit_t({d1, d2, d3, d4}) {}
  drawing_output_t(const double &d1, const double &d2, const double &d3,
                   const double &d4, const double &d5)
      : display_unit_t({d1, d2, d3, d4, d5}) {}
  drawing_output_t(const double &d1, const double &d2, const double &d3,
                   const double &d4, const double &d5, const double &d6)
      : display_unit_t({d1, d2, d3, d4, d5, d6}) {}
  drawing_output_t(const cairo_function &_fn) : display_unit_t(_fn) {}
  ~drawing_output_t() {
    if (oncethread)
      oncethread.reset();

    display_context_t::destroy_buffer(_buf);
  }
  drawing_output_t &operator=(const drawing_output_t &other) {
    _data = other._data;
    if (other._buf.rendered)
      _buf.rendered = cairo_surface_reference(other._buf.rendered);

    if (other._buf.cr)
      _buf.cr = cairo_reference(other._buf.cr);

    fn_draw = other.fn_draw;
    fn_draw_clipped = other.fn_draw_clipped;
    fn_cache_surface = other.fn_cache_surface;
    fn_base_surface = other.fn_base_surface;

    std::copy(other.options.begin(), other.options.end(),
              std::back_inserter(options));
    _ink_rectangle = other._ink_rectangle;
    intersection = other.intersection;
    _intersection = other._intersection;

    return *this;
  }
  drawing_output_t(const drawing_output_t &other) : display_unit_t(other) {
    *this = other;
  }

  bool has_ink_extents = false;
  cairo_rectangle_int_t ink_rectangle = cairo_rectangle_int_t();
  cairo_region_overlap_t overlap = CAIRO_REGION_OVERLAP_OUT;
  void intersect(cairo_rectangle_t &r);
  void intersect(context_cairo_region_t &r);

  void invoke(cairo_t *cr);
  void invoke(display_context_t &context) {}
  bool is_output(void) { return true; }
  std::atomic<bool> bRenderBufferCached = false;
  draw_buffer_t _buf = {};

  // These functions switch the rendering apparatus from off
  // screen threaded to on screen. all rendering is serialize to the main
  // surface
  //
  std::atomic_flag lockFunctors = ATOMIC_FLAG_INIT;
#define LOCK_FUNCTORS_SPIN                                                     \
  while (lockFunctors.test_and_set(std::memory_order_acquire))

#define LOCK_FUNCTORS_CLEAR lockFunctors.clear(std::memory_order_release)

  void functors_lock(bool b) {
    if (b)
      LOCK_FUNCTORS_SPIN;
    else
      LOCK_FUNCTORS_CLEAR;
  }

  HASH_OBJECT_MEMBERS(std::type_index(typeid(this)).hash_code(),
                      last_render_time.time_since_epoch().count(),
                      fn_cache_surface.target_type().name(),
                      fn_base_surface.target_type().name(),
                      fn_draw.target_type().name(),
                      fn_draw_clipped.target_type().name())

  draw_logic_t fn_cache_surface = draw_logic_t();
  draw_logic_t fn_base_surface = draw_logic_t();
  draw_logic_t fn_draw = draw_logic_t();
  draw_logic_t fn_draw_clipped = draw_logic_t();

  // measure processing time
  std::chrono::system_clock::time_point last_render_time = {};
  void evaluate_cache(display_context_t &context);
  bool first_time_rendered = true;
  std::unique_ptr<std::thread> oncethread = nullptr;
  cairo_option_function_t options = {};
  cairo_rectangle_t _ink_rectangle = cairo_rectangle_t();
  cairo_rectangle_int_t intersection = cairo_rectangle_int_t();
  cairo_rectangle_t _intersection = cairo_rectangle_t();
};
} // namespace uxdevice
STD_HASHABLE(uxdevice::drawing_output_t);

#define TYPED_INDEX_INTERFACE(CLASS_NAME)                                      \
  CLASS_NAME &index(const std::string &_k) {                                   \
    key = _k;                                                                  \
    return *this;                                                              \
  }                                                                            \
  CLASS_NAME &index(const std::size_t &_k) {                                   \
    key = _k;                                                                  \
    return *this;                                                              \
  }
namespace uxdevice {
using antialias = class antialias : public display_unit_t {
public:
  antialias() = delete;
  antialias(alias_t _antialias)
      : display_unit_t(_antialias), value(std::get<alias_t>(_data)) {}
  antialias &operator=(const antialias &other) {
    display_unit_t::operator=(other);
    return *this;
  }
  antialias(const antialias &other)
      : display_unit_t(other), value(std::get<alias_t>(_data)) {}
  antialias(antialias &&other) : value(other.value) {}

  void invoke(display_context_t &context) {
    cairo_set_antialias(context.cr, static_cast<cairo_antialias_t>(value));
    is_processed = true;
  }

  HASH_OBJECT_MEMBERS(display_unit_t::hash_code(),
                      std::type_index(typeid(this)).hash_code(), value)

  alias_t &value;

  TYPED_INDEX_INTERFACE(antialias)
};
} // namespace uxdevice
STD_HASHABLE(uxdevice::antialias);

#define PAINT_OBJ(CLASS_NAME)                                                  \
  namespace uxdevice {                                                         \
  using CLASS_NAME = class CLASS_NAME : public display_unit_t,                 \
        public painter_brush_t {                                               \
  public:                                                                      \
    CLASS_NAME(u_int32_t c) : painter_brush_t(c) {}                            \
    CLASS_NAME(double r, double g, double b) : painter_brush_t(r, g, b) {}     \
    CLASS_NAME(double r, double g, double b, double a)                         \
        : painter_brush_t(r, g, b, a) {}                                       \
    CLASS_NAME(const std::string &n) : painter_brush_t(n) {}                   \
    CLASS_NAME(const std::string &n, double width, double height)              \
        : painter_brush_t(n, width, height) {}                                 \
    CLASS_NAME(double x0, double y0, double x1, double y1,                     \
               const color_stops_t &_cs)                                       \
        : painter_brush_t(x0, y0, x1, y1, _cs) {}                              \
    CLASS_NAME(double cx0, double cy0, double radius0, double cx1, double cy1, \
               double radius1, const color_stops_t &cs)                        \
        : painter_brush_t(cx0, cy0, radius0, cx1, cy1, radius1, cs) {}         \
    CLASS_NAME &operator=(const CLASS_NAME &other) {                           \
      display_unit_t::operator=(other);                                        \
      return *this;                                                            \
    }                                                                          \
    CLASS_NAME(const CLASS_NAME &other) : painter_brush_t(other) {}            \
    CLASS_NAME(CLASS_NAME &&other) : painter_brush_t(other) {}                 \
    void emit(cairo_t *cr) {                                                   \
      painter_brush_t::emit(cr);                                               \
      cairo_set_line_width(cr, lineWidth);                                     \
    }                                                                          \
    void emit(cairo_t *cr, double x, double y, double w, double h) {           \
      painter_brush_t::emit(cr);                                               \
      cairo_set_line_width(cr, lineWidth);                                     \
    }                                                                          \
    HASH_OBJECT_MEMBERS(display_unit_t::hash_code(),                           \
                        std::type_index(typeid(this)).hash_code(),             \
                        painter_brush_t::hash_code(), lineWidth, radius, x, y) \
    double lineWidth = 1;                                                      \
    unsigned short radius = 3;                                                 \
    double x = 1, y = 1;                                                       \
    TYPED_INDEX_INTERFACE(CLASS_NAME)                                          \
  };                                                                           \
  }                                                                            \
  STD_HASHABLE(uxdevice::CLASS_NAME)

PAINT_OBJ(text_color);
PAINT_OBJ(text_outline);
PAINT_OBJ(text_fill);
PAINT_OBJ(text_shadow);

namespace uxdevice {
using text_outline_off = class text_outline_off : public display_unit_t {
public:
  text_outline_off() {}
  text_outline_off &operator=(const text_outline_off &other) { return *this; }
  text_outline_off(const text_outline_off &other) {}
  void invoke(display_context_t &context) {
    is_processed = true;
    context.current_units._text_outline.reset();
  }

  HASH_OBJECT_MEMBERS(display_unit_t::hash_code(),
                      std::type_index(typeid(this)).hash_code())

  TYPED_INDEX_INTERFACE(text_outline_off)
};
} // namespace uxdevice
STD_HASHABLE(uxdevice::text_outline_off);

namespace uxdevice {
using text_fill_off = class text_fill_off : public display_unit_t {
public:
  text_fill_off() {}
  text_fill_off &operator=(const text_fill_off &other) { return *this; }
  text_fill_off(const text_fill_off &other) {}
  void invoke(display_context_t &context) {
    is_processed = true;
    context.current_units._text_fill.reset();
  }
  HASH_OBJECT_MEMBERS(display_unit_t::hash_code(),
                      std::type_index(typeid(this)).hash_code())
  TYPED_INDEX_INTERFACE(text_fill_off)
};
} // namespace uxdevice
STD_HASHABLE(uxdevice::text_fill_off);

namespace uxdevice {
using text_shadow_off = class text_shadow_off : public display_unit_t {
public:
  text_shadow_off() {}
  text_shadow_off &operator=(const text_shadow_off &other) { return *this; }
  text_shadow_off(const text_shadow_off &other) {}

  void invoke(display_context_t &context) {
    is_processed = true;
    context.current_units._text_shadow.reset();
  }
  HASH_OBJECT_MEMBERS(display_unit_t::hash_code(),
                      std::type_index(typeid(this)).hash_code())
  TYPED_INDEX_INTERFACE(text_shadow_off)
};
} // namespace uxdevice
STD_HASHABLE(uxdevice::text_shadow_off);

namespace uxdevice {
using text_alignment = class text_alignment : public display_unit_t {
public:
  text_alignment() = delete;
  text_alignment(alignment_t opt)
      : display_unit_t(opt), value(std::get<alignment_t>(_data)) {}
  text_alignment &operator=(const text_alignment &other) { return *this; }
  text_alignment(const text_alignment &other)
      : display_unit_t(other), value(std::get<alignment_t>(_data)) {}
  void emit(PangoLayout *layout) {
    // only change setting if changed, this saves on unnecessary
    // layout context rendering internal to pango
    if (value == alignment_t::justified && !pango_layout_get_justify(layout)) {
      pango_layout_set_justify(layout, true);
    } else if (static_cast<alignment_t>(pango_layout_get_alignment(layout)) !=
               value) {
      pango_layout_set_justify(layout, false);
      pango_layout_set_alignment(layout, static_cast<PangoAlignment>(value));
    }
  }
  HASH_OBJECT_MEMBERS(display_unit_t::hash_code(),
                      std::type_index(typeid(this)).hash_code(), value)

  alignment_t &value;
  TYPED_INDEX_INTERFACE(text_alignment)
};
} // namespace uxdevice
STD_HASHABLE(uxdevice::text_alignment);

namespace uxdevice {
using coordinates = class coordinates : public display_unit_t {
public:
  coordinates() = delete;
  coordinates(double _x, double _y, double _w, double _h)
      : display_unit_t({_x, _y, _w, _h}),
        x(std::get<std::vector<double>>(_data)[0]),
        y(std::get<std::vector<double>>(_data)[1]),
        w(std::get<std::vector<double>>(_data)[2]),
        h(std::get<std::vector<double>>(_data)[3]) {}
  coordinates(const coordinates &other)
      : display_unit_t(other), x(std::get<std::vector<double>>(_data)[0]),
        y(std::get<std::vector<double>>(_data)[1]),
        w(std::get<std::vector<double>>(_data)[2]),
        h(std::get<std::vector<double>>(_data)[3]) {}
  coordinates &operator=(const coordinates &other) { return *this; }

  HASH_OBJECT_MEMBERS(display_unit_t::hash_code(),
                      std::type_index(typeid(this)).hash_code(), x, y, w, h)

  double &x, &y, &w, &h;

  TYPED_INDEX_INTERFACE(coordinates)
};
} // namespace uxdevice
STD_HASHABLE(uxdevice::coordinates);

namespace uxdevice {
using line_width = class line_width : public display_unit_t {
public:
  line_width(double lw) : display_unit_t(lw), value(std::get<double>(_data)) {}
  line_width &operator=(const line_width &other) { return *this; }
  line_width(const line_width &other)
      : display_unit_t(other), value(std::get<double>(_data)) {}
  void invoke(display_context_t &context) {
    is_processed = true;
    cairo_set_line_width(context.cr, value);
  }
  HASH_OBJECT_MEMBERS(display_unit_t::hash_code(),
                      std::type_index(typeid(this)).hash_code(), value)
  double &value;
  TYPED_INDEX_INTERFACE(line_width)
};
} // namespace uxdevice
STD_HASHABLE(uxdevice::line_width);

namespace uxdevice {
using indent = class indent : public display_unit_t {
public:
  indent(double _space)
      : display_unit_t(_space), value(std::get<double>(_data)) {}
  indent &operator=(const indent &other) { return *this; }
  indent(const indent &other)
      : display_unit_t(other), value(std::get<double>(_data)) {}
  void emit(PangoLayout *layout) {
    int pangoUnits = value * PANGO_SCALE;
    pango_layout_set_indent(layout, pangoUnits);
  }
  HASH_OBJECT_MEMBERS(display_unit_t::hash_code(),
                      std::type_index(typeid(this)).hash_code(), value)
  double &value;
  TYPED_INDEX_INTERFACE(indent)
};
} // namespace uxdevice
STD_HASHABLE(uxdevice::indent);

namespace uxdevice {
using ellipsize = class ellipsize : public display_unit_t {
public:
  ellipsize(ellipsize_t eType)
      : display_unit_t(eType), value(std::get<ellipsize_t>(_data)) {}
  ellipsize &operator=(const ellipsize &other) { return *this; }
  ellipsize(const ellipsize &other)
      : display_unit_t(other), value(std::get<ellipsize_t>(_data)) {}
  void emit(PangoLayout *layout) {
    pango_layout_set_ellipsize(layout, static_cast<PangoEllipsizeMode>(value));
  }
  HASH_OBJECT_MEMBERS(display_unit_t::hash_code(),
                      std::type_index(typeid(this)).hash_code(), value)
  ellipsize_t &value;

  TYPED_INDEX_INTERFACE(ellipsize)
};
} // namespace uxdevice
STD_HASHABLE(uxdevice::ellipsize);

namespace uxdevice {
using line_space = class line_space : public display_unit_t {
public:
  line_space(double dSpace)
      : display_unit_t(dSpace), value(std::get<double>(_data)) {}
  line_space &operator=(const line_space &other) { return *this; }
  line_space(const line_space &other)
      : display_unit_t(other), value(std::get<double>(_data)) {}
  void emit(PangoLayout *layout) {
    pango_layout_set_line_spacing(layout, static_cast<float>(value));
  }
  HASH_OBJECT_MEMBERS(display_unit_t::hash_code(),
                      std::type_index(typeid(this)).hash_code(), value)
  double &value;
  TYPED_INDEX_INTERFACE(line_space)
};
} // namespace uxdevice
STD_HASHABLE(uxdevice::line_space);

namespace uxdevice {
using tab_stops = class tab_stops : public display_unit_t {
public:
  tab_stops(const std::vector<double> &_tabs)
      : display_unit_t(_tabs), value(std::get<std::vector<double>>(_data)) {}
  tab_stops &operator=(const tab_stops &other) { return *this; }
  tab_stops(const tab_stops &other)
      : display_unit_t(other), value(std::get<std::vector<double>>(_data)) {}
  void emit(PangoLayout *layout) {
    PangoTabArray *tabs = pango_tab_array_new(value.size(), true);

    int idx = 0;
    for (auto &tabdef : value) {
      int loc = static_cast<int>(tabdef);
      pango_tab_array_set_tab(tabs, idx, PANGO_TAB_LEFT, loc);
    }
    pango_layout_set_tabs(layout, tabs);
    pango_tab_array_free(tabs);
  }
  HASH_OBJECT_MEMBERS(display_unit_t::hash_code(),
                      std::type_index(typeid(this)).hash_code(), value)
  std::vector<double> &value;
  TYPED_INDEX_INTERFACE(tab_stops)
};
} // namespace uxdevice
STD_HASHABLE(uxdevice::tab_stops);

namespace uxdevice {
using text_font = class text_font : public display_unit_t {
public:
  text_font(const std::string &s)
      : display_unit_t(s), value(std::get<std::string>(_data)) {}
  ~text_font() {
    if (fontDescription)
      pango_font_description_free(fontDescription);
  }
  text_font &operator=(const text_font &other) { return *this; }
  text_font(const text_font &other)
      : display_unit_t(other), value(std::get<std::string>(_data)) {}

  std::atomic<PangoFontDescription *> fontDescription = nullptr;
  void invoke(display_context_t &context) {
    is_processed = true;
    if (!fontDescription) {
      fontDescription = pango_font_description_from_string(value.data());
      if (!fontDescription) {
        std::string s = "Font could not be loaded from description. ( ";
        s += value + ")";
        context.error_state(__func__, __LINE__, __FILE__, std::string_view(s));
      }
    }
  }
  HASH_OBJECT_MEMBERS(display_unit_t::hash_code(),
                      std::type_index(typeid(this)).hash_code(), value)

  std::string &value;
  TYPED_INDEX_INTERFACE(text_font)
};
} // namespace uxdevice
STD_HASHABLE(uxdevice::text_font);

namespace uxdevice {
using line_cap = class line_cap : public display_unit_t {
public:
  line_cap(const line_cap_t _val)
      : display_unit_t(_val), value(std::get<line_cap_t>(_data)) {}
  ~line_cap() {}
  line_cap &operator=(const line_cap &other) { return *this; }
  line_cap(const line_cap &other)
      : display_unit_t(other), value(std::get<line_cap_t>(_data)) {}
  void invoke(display_context_t &context) {
    is_processed = true;
    cairo_set_line_cap(context.cr, static_cast<cairo_line_cap_t>(value));
  }
  HASH_OBJECT_MEMBERS(display_unit_t::hash_code(),
                      std::type_index(typeid(this)).hash_code(), value)
  line_cap_t &value;

  TYPED_INDEX_INTERFACE(line_cap)
};
} // namespace uxdevice
STD_HASHABLE(uxdevice::line_cap);

namespace uxdevice {
using line_join = class line_join : public display_unit_t {
public:
  line_join(const line_join_t _val)
      : display_unit_t(_val), value(std::get<line_join_t>(_data)) {}
  ~line_join() {}
  line_join &operator=(const line_join &other) { return *this; }
  line_join(const line_join &other)
      : display_unit_t(other), value(std::get<line_join_t>(_data)) {}
  void invoke(display_context_t &context) {
    is_processed = true;
    cairo_set_line_join(context.cr, static_cast<cairo_line_join_t>(value));
  }
  HASH_OBJECT_MEMBERS(display_unit_t::hash_code(),
                      std::type_index(typeid(this)).hash_code(), value)
  line_join_t &value;
  TYPED_INDEX_INTERFACE(line_join)
};
} // namespace uxdevice
STD_HASHABLE(uxdevice::line_join);

namespace uxdevice {
using miter_limit = class miter_limit : public display_unit_t {
public:
  miter_limit(const double _val)
      : display_unit_t(_val), value(std::get<double>(_data)) {}
  ~miter_limit() {}
  miter_limit &operator=(const miter_limit &other) { return *this; }
  miter_limit(const miter_limit &other)
      : display_unit_t(other), value(std::get<double>(_data)) {}
  void invoke(display_context_t &context) {
    is_processed = true;
    cairo_set_miter_limit(context.cr, value);
  }
  HASH_OBJECT_MEMBERS(display_unit_t::hash_code(),
                      std::type_index(typeid(this)).hash_code(), value)
  double &value;
  TYPED_INDEX_INTERFACE(miter_limit)
};
} // namespace uxdevice
STD_HASHABLE(uxdevice::miter_limit);

namespace uxdevice {
using line_dashes = class line_dashes : public display_unit_t {
public:
  line_dashes(const std::vector<double> &_val, double _offset = 0)
      : display_unit_t(_val, _offset),
        value(std::get<std::vector<double>>(_data)) {}
  ~line_dashes() {}
  line_dashes &operator=(const line_dashes &other) { return *this; }
  line_dashes(const line_dashes &other)
      : display_unit_t(other), value(std::get<std::vector<double>>(_data)) {}
  void invoke(display_context_t &context) {
    is_processed = true;
    cairo_set_dash(context.cr, value.data(), value.size(), _offset);
  }
  TYPED_INDEX_INTERFACE(line_dashes)
  HASH_OBJECT_MEMBERS(display_unit_t::hash_code(),
                      std::type_index(typeid(this)).hash_code(), value)
  std::vector<double> &value;
};
} // namespace uxdevice
STD_HASHABLE(uxdevice::line_dashes);

namespace uxdevice {
class textual_data : public display_unit_t {
public:
  textual_data(const std::string &s)
      : display_unit_t(s), value(std::get<std::string>(_data)) {}
  textual_data(const std::shared_ptr<std::string> &s)
      : display_unit_t(s),
        value(*(std::get<std::shared_ptr<std::string>>(_data)).get()) {}

  ~textual_data() {}
  textual_data &operator=(const textual_data &other) { return *this; }
  textual_data(const textual_data &other)
      : display_unit_t(other), value(std::get<std::string>(_data)) {}
  void invoke(display_context_t &context) { is_processed = true; }
  TYPED_INDEX_INTERFACE(textual_data)

  HASH_OBJECT_MEMBERS(display_unit_t::hash_code(),
                      std::type_index(typeid(this)).hash_code(), value)

  std::string &value;
};
} // namespace uxdevice
STD_HASHABLE(uxdevice::textual_data);

namespace uxdevice {
class textual_render : public drawing_output_t {
public:
  textual_render(std::shared_ptr<textual_data> data) : _text(data) {}
  ~textual_render() {}
  textual_render &operator=(const textual_render &other) { return *this; }
  textual_render(const textual_render &other) : drawing_output_t(other) {}

  TYPED_INDEX_INTERFACE(textual_render)

  bool is_output(void) { return true; }
  void invoke(display_context_t &context);

private:
  bool set_layout_options(cairo_t *cr);
  void create_shadow(void);
  void setup_draw(display_context_t &context);

public:
  HASH_OBJECT_MEMBERS(drawing_output_t::hash_code(),
                      std::type_index(typeid(this)).hash_code(),
                      HASH_OBJECT_MEMBER_SHARED_PTR(_text_color),
                      HASH_OBJECT_MEMBER_SHARED_PTR(_text_outline),
                      HASH_OBJECT_MEMBER_SHARED_PTR(_text_fill),
                      HASH_OBJECT_MEMBER_SHARED_PTR(_text_shadow),
                      HASH_OBJECT_MEMBER_SHARED_PTR(_text_font),
                      HASH_OBJECT_MEMBER_SHARED_PTR(_text_alignment),
                      HASH_OBJECT_MEMBER_SHARED_PTR(_coordinates),
                      HASH_OBJECT_MEMBER_SHARED_PTR(_text),
                      pango_layout_get_serial(_layout), _ink_rect.x,
                      _ink_rect.y, _ink_rect.width, _ink_rect.height,
                      _matrix.hash_code())

private:
  std::atomic<cairo_surface_t *> _shadow_image = nullptr;
  std::atomic<cairo_t *> _shadow_cr = nullptr;
  std::atomic<PangoLayout *> _layout = nullptr;
  PangoRectangle _ink_rect = PangoRectangle();
  PangoRectangle _logical_rect = PangoRectangle();
  Matrix _matrix = {};

  // local parameter pointers
  std::shared_ptr<text_color> _text_color = nullptr;
  std::shared_ptr<text_outline> _text_outline = nullptr;
  std::shared_ptr<text_fill> _text_fill = nullptr;
  std::shared_ptr<text_shadow> _text_shadow = nullptr;
  std::shared_ptr<text_font> _text_font = nullptr;
  std::shared_ptr<text_alignment> _text_alignment = nullptr;
  std::shared_ptr<coordinates> _coordinates = nullptr;
  std::shared_ptr<textual_data> _text = nullptr;
};
} // namespace uxdevice
STD_HASHABLE(uxdevice::textual_render);

namespace uxdevice {
class image_block : public drawing_output_t {
public:
  image_block(const std::string &data)
      : drawing_output_t(data), value(std::get<std::string>(_data)) {}
  image_block(const image_block &other)
      : drawing_output_t(other), value(std::get<std::string>(_data)) {
    *this = other;
  }
  image_block &operator=(const image_block &other) {
    _image_block = cairo_surface_reference(other._image_block);

    is_SVG = other.is_SVG;
    if (_image_block)
      is_loaded = true;
    _coordinates = other._coordinates;
    return *this;
  }
  ~image_block() {
    if (_image_block)
      cairo_surface_destroy(_image_block);
  }

  void invoke(display_context_t &context);
  bool is_valid(void);

  HASH_OBJECT_MEMBERS(drawing_output_t::hash_code(),
                      std::type_index(typeid(this)).hash_code(), value,
                       is_SVG,
                      _coordinates ? _coordinates->hash_code() : 0)

  std::string &value;
  std::atomic<cairo_surface_t *> _image_block = {};
  bool is_SVG = {};
  std::atomic<bool> is_loaded = {};
  std::shared_ptr<coordinates> _coordinates = {};

  TYPED_INDEX_INTERFACE(image_block)
};
} // namespace uxdevice
STD_HASHABLE(uxdevice::image_block);

/**
\internal
\brief call previously bound function with the cairo context.
*/
namespace uxdevice {
class function_object_t : public display_unit_t {
public:
  function_object_t(cairo_function _func)
      : display_unit_t(_func), value(std::get<cairo_function>(_data)) {}
  ~function_object_t() {}
  function_object_t &operator=(const function_object_t &other) { return *this; }
  function_object_t(const function_object_t &other)
      : display_unit_t(other), value(std::get<cairo_function>(_data)) {}

  void invoke(display_context_t &context) {
    is_processed = true;
    value(context.cr);
  }
  HASH_OBJECT_MEMBERS(display_unit_t::hash_code(),
                      std::type_index(typeid(this)).hash_code(), value)
  cairo_function &value;
  TYPED_INDEX_INTERFACE(function_object_t)
};
} // namespace uxdevice
STD_HASHABLE(uxdevice::function_object_t);

namespace uxdevice {
class draw_function_object_t : public drawing_output_t {
public:
  draw_function_object_t(cairo_function _func)
      : drawing_output_t(_func), value(std::get<cairo_function>(_data)) {}
  ~draw_function_object_t() {}
  draw_function_object_t &operator=(const draw_function_object_t &other) {

    return *this;
  }
  draw_function_object_t(const draw_function_object_t &other)
      : drawing_output_t(other), value(std::get<cairo_function>(_data)) {}

  HASH_OBJECT_MEMBERS(drawing_output_t::hash_code(),
                      std::type_index(typeid(this)).hash_code(), value)

  void invoke(display_context_t &context);
  cairo_function &value;

  TYPED_INDEX_INTERFACE(draw_function_object_t)
};
} // namespace uxdevice
STD_HASHABLE(uxdevice::draw_function_object_t);

namespace uxdevice {
class option_function_object_t : public display_unit_t {
public:
  option_function_object_t(cairo_function _func)
      : display_unit_t(_func), value(std::get<cairo_function>(_data)) {}
  ~option_function_object_t() {}
  option_function_object_t &operator=(const option_function_object_t &other) {
    _data = other._data;
    key = other.key;
    return *this;
  }
  option_function_object_t(const option_function_object_t &other)
      : display_unit_t(other), value(std::get<cairo_function>(_data)) {}

  void invoke(display_context_t &context);
  void invoke(cairo_t *cr) { value(cr); }

  HASH_OBJECT_MEMBERS(display_unit_t::hash_code(),
                      std::type_index(typeid(this)).hash_code(), value)
  cairo_function &value;
  TYPED_INDEX_INTERFACE(option_function_object_t)
};
} // namespace uxdevice
STD_HASHABLE(uxdevice::option_function_object_t);

namespace uxdevice {
using tollerance = class tollerance : public display_unit_t {
public:
  tollerance(const double _t)
      : display_unit_t(_t), value(std::get<double>(_data)) {}
  tollerance &operator=(const tollerance &other) { return *this; }
  tollerance(const tollerance &other)
      : display_unit_t(other), value(std::get<double>(_data)) {}

  void invoke(display_context_t &context) {
    is_processed = true;
    cairo_set_tolerance(context.cr, value);
  }
  HASH_OBJECT_MEMBERS(display_unit_t::hash_code(),
                      std::type_index(typeid(this)).hash_code(), value)
  double &value;

  TYPED_INDEX_INTERFACE(tollerance)
};
} // namespace uxdevice
STD_HASHABLE(uxdevice::tollerance);

namespace uxdevice {
using op = class op : public display_unit_t {
public:
  op(op_t _op) : display_unit_t(_op), value(std::get<op_t>(_data)) {}
  op &operator=(const op &other) { return *this; }
  op(const op &other) : display_unit_t(other), value(std::get<op_t>(_data)) {}

  void invoke(display_context_t &context) {
    is_processed = true;
    cairo_set_operator(context.cr, static_cast<cairo_operator_t>(value));
  }
  HASH_OBJECT_MEMBERS(display_unit_t::hash_code(),
                      std::type_index(typeid(this)).hash_code(), value)
  op_t &value;
  TYPED_INDEX_INTERFACE(op)
};
} // namespace uxdevice
STD_HASHABLE(uxdevice::op);

namespace uxdevice {
using close_path = class close_path : public drawing_output_t {
public:
  close_path() {}
  ~close_path() {}
  close_path &operator=(const close_path &other) { return *this; }
  close_path(const close_path &other) : drawing_output_t(other) {}

  void invoke(display_context_t &context) {
    is_processed = true;
    cairo_close_path(context.cr);
  }
  TYPED_INDEX_INTERFACE(close_path)
  HASH_OBJECT_MEMBERS(drawing_output_t::hash_code(),
                      std::type_index(typeid(this)).hash_code(), value)
};
} // namespace uxdevice
STD_HASHABLE(uxdevice::close_path);

namespace uxdevice {
using arc = class arc : public drawing_output_t {
public:
  arc(double _xc, double _yc, double _radius, double _angle1, double _angle2)
      : drawing_output_t(_xc, _yc, _radius, _angle1, _angle2),
        xc(std::get<std::vector<double>>(_data)[0]),
        yc(std::get<std::vector<double>>(_data)[1]),
        radius(std::get<std::vector<double>>(_data)[2]),
        angle1(std::get<std::vector<double>>(_data)[3]),
        angle2(std::get<std::vector<double>>(_data)[4]) {}
  arc &operator=(const arc &other) {
    _data = other._data;
    key = other.key;
    return *this;
  }
  arc(const arc &other)
      : drawing_output_t(other), xc(std::get<std::vector<double>>(_data)[0]),
        yc(std::get<std::vector<double>>(_data)[1]),
        radius(std::get<std::vector<double>>(_data)[2]),
        angle1(std::get<std::vector<double>>(_data)[3]),
        angle2(std::get<std::vector<double>>(_data)[4]) {}

  void invoke(display_context_t &context) {
    is_processed = true;
    cairo_arc(context.cr, xc, yc, radius, angle1, angle2);
  }
  HASH_OBJECT_MEMBERS(drawing_output_t::hash_code(),
                      std::type_index(typeid(this)).hash_code(), xc, yc, radius,
                      angle1, angle2)

  double &xc;
  double &yc;
  double &radius;
  double &angle1;
  double &angle2;
  TYPED_INDEX_INTERFACE(arc)
};
} // namespace uxdevice
STD_HASHABLE(uxdevice::arc);

namespace uxdevice {
using negative_arc = class negative_arc : public drawing_output_t {
public:
  negative_arc(double _xc, double _yc, double _radius, double _angle1,
               double _angle2)
      : drawing_output_t(_xc, _yc, _radius, _angle1, _angle2),
        xc(std::get<std::vector<double>>(_data)[0]),
        yc(std::get<std::vector<double>>(_data)[1]),
        radius(std::get<std::vector<double>>(_data)[2]),
        angle1(std::get<std::vector<double>>(_data)[3]),
        angle2(std::get<std::vector<double>>(_data)[4]) {}
  negative_arc &operator=(const negative_arc &other) { return *this; }
  negative_arc(const negative_arc &other)
      : drawing_output_t(other), xc(std::get<std::vector<double>>(_data)[0]),
        yc(std::get<std::vector<double>>(_data)[1]),
        radius(std::get<std::vector<double>>(_data)[2]),
        angle1(std::get<std::vector<double>>(_data)[3]),
        angle2(std::get<std::vector<double>>(_data)[4]) {}

  void invoke(display_context_t &context) {
    is_processed = true;
    cairo_arc_negative(context.cr, xc, yc, radius, angle1, angle2);
  }
  HASH_OBJECT_MEMBERS(drawing_output_t::hash_code(),
                      std::type_index(typeid(this)).hash_code(), xc, yc, radius,
                      angle1, angle2)
  double &xc;
  double &yc;
  double &radius;
  double &angle1;
  double &angle2;
  TYPED_INDEX_INTERFACE(negative_arc)
};
} // namespace uxdevice
STD_HASHABLE(uxdevice::negative_arc);

namespace uxdevice {
using curve = class curve : public drawing_output_t {
public:
  curve(double _x1, double _y1, double _x2, double _y2, double _x3, double _y3)
      : drawing_output_t(_x1, _y1, _x2, _y2, _x3, _y3),
        x1(std::get<std::vector<double>>(_data)[0]),
        y1(std::get<std::vector<double>>(_data)[1]),
        x2(std::get<std::vector<double>>(_data)[2]),
        y2(std::get<std::vector<double>>(_data)[3]),
        x3(std::get<std::vector<double>>(_data)[4]),
        y3(std::get<std::vector<double>>(_data)[5]) {}
  curve &operator=(const curve &other) { return *this; }
  curve(const curve &other)
      : drawing_output_t(other), x1(std::get<std::vector<double>>(_data)[0]),
        y1(std::get<std::vector<double>>(_data)[1]),
        x2(std::get<std::vector<double>>(_data)[2]),
        y2(std::get<std::vector<double>>(_data)[3]),
        x3(std::get<std::vector<double>>(_data)[4]),
        y3(std::get<std::vector<double>>(_data)[5]) {}
  void invoke(display_context_t &context) {
    is_processed = true;
    if (context.relative)
      cairo_rel_curve_to(context.cr, x1, y1, x2, y3, x3, y3);
    else
      cairo_curve_to(context.cr, x1, y1, x2, y2, x3, y3);
  }

  HASH_OBJECT_MEMBERS(drawing_output_t::hash_code(),
                      std::type_index(typeid(this)).hash_code(), x1, y1, x2, y2,
                      x3, y3)

  double &x1, &y1, &x2, &y2, &x3, &y3;
  TYPED_INDEX_INTERFACE(curve)
};
} // namespace uxdevice
STD_HASHABLE(uxdevice::curve);

namespace uxdevice {
using line = class line : public drawing_output_t {
public:
  line(double _x, double _y)
      : drawing_output_t(_x, _y), x(std::get<std::vector<double>>(_data)[0]),
        y(std::get<std::vector<double>>(_data)[1]) {}
  line &operator=(const line &other) { return *this; }
  line(const line &other)
      : drawing_output_t(other), x(std::get<std::vector<double>>(_data)[0]),
        y(std::get<std::vector<double>>(_data)[1]) {}

  void invoke(display_context_t &context) {
    is_processed = true;
    if (context.relative)
      cairo_rel_line_to(context.cr, x, y);
    else
      cairo_line_to(context.cr, x, y);
  }
  HASH_OBJECT_MEMBERS(drawing_output_t::hash_code(),
                      std::type_index(typeid(this)).hash_code(), x, y)
  double &x, &y;
  TYPED_INDEX_INTERFACE(line)
};
} // namespace uxdevice
STD_HASHABLE(uxdevice::line);

namespace uxdevice {
using hline = class hline : public drawing_output_t {
public:
  hline(double _x) : drawing_output_t(_x), x(std::get<double>(_data)) {}
  hline &operator=(const hline &other) { return *this; }
  hline(const hline &other)
      : drawing_output_t(other), x(std::get<double>(_data)) {}

  void invoke(display_context_t &context) {
    is_processed = true;

    if (cairo_has_current_point(context.cr)) {
      double curx = 0.0, cury = 0.0;
      cairo_get_current_point(context.cr, &curx, &cury);

      if (context.relative)
        cairo_rel_line_to(context.cr, x, 0);
      else
        cairo_line_to(context.cr, x, cury);
    }
  }
  HASH_OBJECT_MEMBERS(drawing_output_t::hash_code(),
                      std::type_index(typeid(this)).hash_code(), x)
  double &x;
  TYPED_INDEX_INTERFACE(hline)
};
} // namespace uxdevice
STD_HASHABLE(uxdevice::hline);

namespace uxdevice {
using vline = class vline : public drawing_output_t {
public:
  vline(double _y) : drawing_output_t(_y), y(std::get<double>(_data)) {}
  vline &operator=(const vline &other) {
    _data = other._data;
    key = other.key;
    return *this;
  }
  vline(const vline &other)
      : drawing_output_t(other), y(std::get<double>(_data)) {}
  void invoke(display_context_t &context) {
    is_processed = true;

    if (cairo_has_current_point(context.cr)) {
      double curx = 0.0, cury = 0.0;
      cairo_get_current_point(context.cr, &curx, &cury);

      if (context.relative)
        cairo_rel_line_to(context.cr, 0, y);
      else
        cairo_line_to(context.cr, curx, y);
    }
  }
  HASH_OBJECT_MEMBERS(drawing_output_t::hash_code(),
                      std::type_index(typeid(this)).hash_code(), y)
  double &y;
  TYPED_INDEX_INTERFACE(vline)
};
} // namespace uxdevice
STD_HASHABLE(uxdevice::vline);

namespace uxdevice {
using move_to = class move_to : public drawing_output_t {
public:
  move_to(double _x, double _y)
      : drawing_output_t(_x, _y), x(std::get<std::vector<double>>(_data)[0]),
        y(std::get<std::vector<double>>(_data)[1]) {}
  move_to &operator=(const move_to &other) {
    _data = other._data;
    key = other.key;
    return *this;
  }
  move_to(const move_to &other)
      : drawing_output_t(other), x(std::get<std::vector<double>>(_data)[0]),
        y(std::get<std::vector<double>>(_data)[1]) {}
  void invoke(display_context_t &context) {
    is_processed = true;
    if (context.relative)
      cairo_rel_move_to(context.cr, x, y);
    else
      cairo_move_to(context.cr, x, y);
  }
  HASH_OBJECT_MEMBERS(drawing_output_t::hash_code(),
                      std::type_index(typeid(this)).hash_code(), x, y)
  double &x, &y;
  TYPED_INDEX_INTERFACE(move_to)
};
} // namespace uxdevice
STD_HASHABLE(uxdevice::move_to);

namespace uxdevice {
using rectangle = class rectangle : public drawing_output_t {
public:
  rectangle(double _x, double _y, double _width, double _height)
      : drawing_output_t(_x, _y, _width, _height),
        x(std::get<std::vector<double>>(_data)[0]),
        y(std::get<std::vector<double>>(_data)[1]),
        width(std::get<std::vector<double>>(_data)[2]),
        height(std::get<std::vector<double>>(_data)[3]) {}
  rectangle &operator=(const rectangle &other) {
    _data = other._data;
    key = other.key;
    return *this;
  }
  rectangle(const rectangle &other)
      : drawing_output_t(other), x(std::get<std::vector<double>>(_data)[0]),
        y(std::get<std::vector<double>>(_data)[1]),
        width(std::get<std::vector<double>>(_data)[2]),
        height(std::get<std::vector<double>>(_data)[3]) {}

  void invoke(display_context_t &context) {
    is_processed = true;
    cairo_rectangle(context.cr, x, y, width, height);
  }
  HASH_OBJECT_MEMBERS(drawing_output_t::hash_code(),
                      std::type_index(typeid(this)).hash_code(), x, y, width,
                      height)
  double &x, &y, &width, &height;
  TYPED_INDEX_INTERFACE(rectangle)
};
} // namespace uxdevice
STD_HASHABLE(uxdevice::rectangle);

namespace uxdevice {
using stroke_path_preserve =
    class stroke_path_preserve : public drawing_output_t {
public:
  stroke_path_preserve(painter_brush_t &p)
      : drawing_output_t(p), value(std::get<painter_brush_t>(_data)) {}
  stroke_path_preserve &operator=(const stroke_path_preserve &other) {

    return *this;
  }
  stroke_path_preserve(const stroke_path_preserve &other)
      : drawing_output_t(other), value(std::get<painter_brush_t>(_data)) {}

  void invoke(display_context_t &context) {
    is_processed = true;
    value.emit(context.cr);
    cairo_stroke_preserve(context.cr);
  }
  HASH_OBJECT_MEMBERS(drawing_output_t::hash_code(),
                      std::type_index(typeid(this)).hash_code(), value)
  painter_brush_t &value;
  TYPED_INDEX_INTERFACE(stroke_path_preserve)
};
} // namespace uxdevice
STD_HASHABLE(uxdevice::stroke_path_preserve);

namespace uxdevice {
using stroke_path = class stroke_path : public drawing_output_t {
public:
  stroke_path(painter_brush_t &p)
      : drawing_output_t(p), value(std::get<painter_brush_t>(_data)) {}
  stroke_path &operator=(const stroke_path &other) { return *this; }
  stroke_path(const stroke_path &other)
      : drawing_output_t(other), value(std::get<painter_brush_t>(_data)) {}

  void invoke(display_context_t &context) {
    is_processed = true;
    value.emit(context.cr);
    cairo_stroke(context.cr);
  }
  HASH_OBJECT_MEMBERS(drawing_output_t::hash_code(),
                      std::type_index(typeid(this)).hash_code(), value)
  painter_brush_t &value;
  TYPED_INDEX_INTERFACE(stroke_path)
};
} // namespace uxdevice
STD_HASHABLE(uxdevice::stroke_path);

namespace uxdevice {
using fill_path_preserve = class fill_path_preserve : public drawing_output_t {
public:
  fill_path_preserve(painter_brush_t &p)
      : drawing_output_t(p), value(std::get<painter_brush_t>(_data)) {}
  fill_path_preserve &operator=(const fill_path_preserve &other) {

    return *this;
  }
  fill_path_preserve(const fill_path_preserve &other)
      : drawing_output_t(other), value(std::get<painter_brush_t>(_data)) {}

  void invoke(display_context_t &context) {
    is_processed = true;
    value.emit(context.cr);
    cairo_fill_preserve(context.cr);
  }
  HASH_OBJECT_MEMBERS(drawing_output_t::hash_code(),
                      std::type_index(typeid(this)).hash_code(), value)
  painter_brush_t &value;
  TYPED_INDEX_INTERFACE(fill_path_preserve)
};
} // namespace uxdevice
STD_HASHABLE(uxdevice::fill_path_preserve);

namespace uxdevice {
using fill_path = class fill_path : public drawing_output_t {
public:
  fill_path(painter_brush_t &p)
      : drawing_output_t(p), value(std::get<painter_brush_t>(_data)) {}
  fill_path &operator=(const fill_path &other) { return *this; }
  fill_path(const fill_path &other)
      : drawing_output_t(other), value(std::get<painter_brush_t>(_data)) {}

  void invoke(display_context_t &context) {
    is_processed = true;
    value.emit(context.cr);
    cairo_fill(context.cr);
  }
  HASH_OBJECT_MEMBERS(drawing_output_t::hash_code(),
                      std::type_index(typeid(this)).hash_code(), value)
  painter_brush_t &value;
  TYPED_INDEX_INTERFACE(fill_path)
};
} // namespace uxdevice
STD_HASHABLE(uxdevice::fill_path);

namespace uxdevice {
using mask = class mask : public drawing_output_t {
public:
  mask(painter_brush_t &p)
      : drawing_output_t(p), value(std::get<painter_brush_t>(_data)) {}
  mask(painter_brush_t &p, double x, double y)
      : drawing_output_t(p, x, y), value(std::get<painter_brush_t>(_data)) {}
  mask &operator=(const mask &other) { return *this; }
  mask(const mask &other)
      : drawing_output_t(other), value(std::get<painter_brush_t>(_data)) {}

  HASH_OBJECT_MEMBERS(drawing_output_t::hash_code(),
                      std::type_index(typeid(this)).hash_code(), value)
  painter_brush_t &value;
  TYPED_INDEX_INTERFACE(mask)
};
} // namespace uxdevice
STD_HASHABLE(uxdevice::mask);

namespace uxdevice {
using paint = class paint : public drawing_output_t {
public:
  paint(double alpha = 1.0)
      : drawing_output_t(alpha), value(std::get<double>(_data)) {}
  paint &operator=(const paint &other) { return *this; }
  paint(const paint &other)
      : drawing_output_t(other), value(std::get<double>(_data)) {}

  void invoke(display_context_t &context) {
    is_processed = true;
    if (value == 1.0) {
      cairo_paint(context.cr);
    } else {
      cairo_paint_with_alpha(context.cr, value);
    }
  }
  HASH_OBJECT_MEMBERS(drawing_output_t::hash_code(),
                      std::type_index(typeid(this)).hash_code(), value)
  double &value;
  TYPED_INDEX_INTERFACE(paint)
};
} // namespace uxdevice
STD_HASHABLE(uxdevice::paint);

namespace uxdevice {
using relative = class relative : public display_unit_t {
public:
  relative &operator=(const relative &other) { return *this; }
  relative(const relative &other) : display_unit_t(other) {}
  HASH_OBJECT_MEMBERS(display_unit_t::hash_code(),
                      std::type_index(typeid(this)).hash_code())
  TYPED_INDEX_INTERFACE(relative)
};
} // namespace uxdevice
STD_HASHABLE(uxdevice::relative);

namespace uxdevice {
using absolute = class absolute : public display_unit_t {
public:
  absolute &operator=(const absolute &other) { return *this; }
  absolute(const absolute &other) : display_unit_t(other) {}
  HASH_OBJECT_MEMBERS(display_unit_t::hash_code(),
                      std::type_index(typeid(this)).hash_code())
  TYPED_INDEX_INTERFACE(absolute)
};
} // namespace uxdevice
STD_HASHABLE(uxdevice::absolute);

namespace uxdevice {
// listeners are named from this base class.
using listener = class listener : public display_unit_t {
public:
  listener(const eventType &_eType, const event_handler_t &_evtDispatcher)
      : eType(_eType), evtDispatcher(_evtDispatcher) {}
  listener &operator=(const listener &other) {
    _data = other._data;
    key = other.key;
    return *this;
  }
  listener(const listener &other)
      : display_unit_t(other), eType(other.eType),
        evtDispatcher(other.evtDispatcher) {}

  eventType eType = {};
  event_handler_t evtDispatcher = {};
  TYPED_INDEX_INTERFACE(listener)
};
} // namespace uxdevice
STD_HASHABLE(uxdevice::listener);

namespace uxdevice {
// Named event listeners shorten the code a little and make the code more
// readable. pasting macros would be nice, however, its not the c++ standard
using listen_paint = class listen_paint : public listener {
public:
  listen_paint(const event_handler_t &_evtDispatcher)
      : listener(eventType::paint, _evtDispatcher) {}
  listen_paint &operator=(const listen_paint &other) {
    _data = other._data;
    key = other.key;
    return *this;
  }
  listen_paint(const listen_paint &other) : listener(other) {}
  TYPED_INDEX_INTERFACE(listen_paint)
};
} // namespace uxdevice
STD_HASHABLE(uxdevice::listen_paint);

namespace uxdevice {
using listen_focus = class listen_focus : public listener {
public:
  listen_focus(const event_handler_t &_evtDispatcher)
      : listener(eventType::focus, _evtDispatcher) {}
  listen_focus &operator=(const listen_focus &other) {
    _data = other._data;
    key = other.key;
    return *this;
  }
  listen_focus(const listen_focus &other) : listener(other) {}
  TYPED_INDEX_INTERFACE(listen_focus)
};
} // namespace uxdevice
STD_HASHABLE(uxdevice::listen_focus);

namespace uxdevice {
using listen_blur = class listen_blur : public listener {
public:
  listen_blur(const event_handler_t &_evtDispatcher)
      : listener(eventType::blur, _evtDispatcher) {}
  listen_blur &operator=(const listen_blur &other) {
    _data = other._data;
    key = other.key;
    return *this;
  }
  listen_blur(const listen_blur &other) : listener(other) {}
  TYPED_INDEX_INTERFACE(listen_blur)
};
} // namespace uxdevice
STD_HASHABLE(uxdevice::listen_blur);

namespace uxdevice {
using listen_resize = class listen_resize : public listener {
public:
  listen_resize(const event_handler_t &_evtDispatcher)
      : listener(eventType::resize, _evtDispatcher) {}
  listen_resize &operator=(const listen_resize &other) {
    _data = other._data;
    key = other.key;
    return *this;
  }
  listen_resize(const listen_resize &other) : listener(other) {}
  TYPED_INDEX_INTERFACE(listen_resize)
};
} // namespace uxdevice
STD_HASHABLE(uxdevice::listen_resize);

namespace uxdevice {
using listen_keydown = class listen_keydown : public listener {
public:
  listen_keydown(const event_handler_t &_evtDispatcher)
      : listener(eventType::keydown, _evtDispatcher) {}
  listen_keydown &operator=(const listen_keydown &other) {
    _data = other._data;
    key = other.key;
    return *this;
  }
  listen_keydown(const listen_keydown &other) : listener(other) {}
  TYPED_INDEX_INTERFACE(listen_keydown)
};
} // namespace uxdevice
STD_HASHABLE(uxdevice::listen_keydown);

namespace uxdevice {
using listen_keyup = class listen_keyup : public listener {
public:
  listen_keyup(const event_handler_t &_evtDispatcher)
      : listener(eventType::keyup, _evtDispatcher) {}
  listen_keyup &operator=(const listen_keyup &other) {
    _data = other._data;
    key = other.key;
    return *this;
  }
  listen_keyup(const listen_keyup &other) : listener(other) {}
  TYPED_INDEX_INTERFACE(listen_keyup)
};
} // namespace uxdevice
STD_HASHABLE(uxdevice::listen_keyup);

namespace uxdevice {
using listen_keypress = class listen_keypress : public listener {
public:
  listen_keypress(const event_handler_t &_evtDispatcher)
      : listener(eventType::keypress, _evtDispatcher) {}
  listen_keypress &operator=(const listen_keypress &other) {
    _data = other._data;
    key = other.key;
    return *this;
  }
  listen_keypress(const listen_keypress &other) : listener(other) {}
  TYPED_INDEX_INTERFACE(listen_keypress)
};
} // namespace uxdevice
STD_HASHABLE(uxdevice::listen_keypress);

namespace uxdevice {
using listen_mouseenter = class listen_mouseenter : public listener {
public:
  listen_mouseenter(const event_handler_t &_evtDispatcher)
      : listener(eventType::mouseenter, _evtDispatcher) {}
  listen_mouseenter &operator=(const listen_mouseenter &other) {
    _data = other._data;
    key = other.key;
    return *this;
  }
  listen_mouseenter(const listen_mouseenter &other) : listener(other) {}
  TYPED_INDEX_INTERFACE(listen_mouseenter)
};
} // namespace uxdevice
STD_HASHABLE(uxdevice::listen_mouseenter);

namespace uxdevice {
using listen_mousemove = class listen_mousemove : public listener {
public:
  listen_mousemove(const event_handler_t &_evtDispatcher)
      : listener(eventType::mousemove, _evtDispatcher) {}
  listen_mousemove &operator=(const listen_mousemove &other) {
    _data = other._data;
    key = other.key;
    return *this;
  }
  listen_mousemove(const listen_mousemove &other) : listener(other) {}
  TYPED_INDEX_INTERFACE(listen_mousemove)
};
} // namespace uxdevice
STD_HASHABLE(uxdevice::listen_mousemove);

namespace uxdevice {
using listen_mousedown = class listen_mousedown : public listener {
public:
  listen_mousedown(const event_handler_t &_evtDispatcher)
      : listener(eventType::mousedown, _evtDispatcher) {}
  listen_mousedown &operator=(const listen_mousedown &other) {
    _data = other._data;
    key = other.key;
    return *this;
  }
  listen_mousedown(const listen_mousedown &other) : listener(other) {}
  TYPED_INDEX_INTERFACE(listen_mousedown)
};
} // namespace uxdevice
STD_HASHABLE(uxdevice::listen_mousedown);

namespace uxdevice {
using listen_mouseup = class listen_mouseup : public listener {
public:
  listen_mouseup(const event_handler_t &_evtDispatcher)
      : listener(eventType::mouseup, _evtDispatcher) {}
  listen_mouseup &operator=(const listen_mouseup &other) {
    _data = other._data;
    key = other.key;
    return *this;
  }
  listen_mouseup(const listen_mouseup &other) : listener(other) {}
  TYPED_INDEX_INTERFACE(listen_mouseup)
};
} // namespace uxdevice
STD_HASHABLE(uxdevice::listen_mouseup);

namespace uxdevice {
using listen_click = class listen_click : public listener {
public:
  listen_click(const event_handler_t &_evtDispatcher)
      : listener(eventType::click, _evtDispatcher) {}
  listen_click &operator=(const listen_click &other) {
    _data = other._data;
    key = other.key;
    return *this;
  }
  listen_click(const listen_click &other) : listener(other) {}
  TYPED_INDEX_INTERFACE(listen_click)
};
} // namespace uxdevice
STD_HASHABLE(uxdevice::listen_click);

namespace uxdevice {
using listen_dblclick = class listen_dblclick : public listener {
public:
  listen_dblclick(const event_handler_t &_evtDispatcher)
      : listener(eventType::dblclick, _evtDispatcher) {}
  listen_dblclick &operator=(const listen_dblclick &other) {
    _data = other._data;
    key = other.key;
    return *this;
  }
  listen_dblclick(const listen_dblclick &other) : listener(other) {}
  TYPED_INDEX_INTERFACE(listen_dblclick)
};
} // namespace uxdevice
STD_HASHABLE(uxdevice::listen_dblclick);

namespace uxdevice {
using listen_contextmenu = class listen_contextmenu : public listener {
public:
  listen_contextmenu(const event_handler_t &_evtDispatcher)
      : listener(eventType::contextmenu, _evtDispatcher) {}
  listen_contextmenu &operator=(const listen_contextmenu &other) {
    _data = other._data;
    key = other.key;
    return *this;
  }
  listen_contextmenu(const listen_contextmenu &other) : listener(other) {}
  TYPED_INDEX_INTERFACE(listen_contextmenu)
};
} // namespace uxdevice
STD_HASHABLE(uxdevice::listen_contextmenu);

namespace uxdevice {
using listen_wheel = class listen_wheel : public listener {
public:
  listen_wheel(const event_handler_t &_evtDispatcher)
      : listener(eventType::wheel, _evtDispatcher) {}
  listen_wheel &operator=(const listen_wheel &other) { return *this; }
  listen_wheel(const listen_wheel &other) : listener(other) {}
  TYPED_INDEX_INTERFACE(listen_wheel)
};
} // namespace uxdevice
STD_HASHABLE(uxdevice::listen_wheel);

namespace uxdevice {
using listen_mouseleave = class listen_mouseleave : public listener {
public:
  listen_mouseleave(const event_handler_t &_evtDispatcher)
      : listener(eventType::mouseleave, _evtDispatcher) {}
  listen_mouseleave &operator=(const listen_mouseleave &other) { return *this; }
  listen_mouseleave(const listen_mouseleave &other) : listener(other) {}
  TYPED_INDEX_INTERFACE(listen_mouseleave)
};
} // namespace uxdevice
STD_HASHABLE(uxdevice::listen_mouseleave);
