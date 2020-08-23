/**
\author Anthony Matarazzo
\file uxdisplayunits.hpp
\date 5/12/20
\version 1.0
 \details

*/
#pragma once

namespace uxdevice {

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

  std::variant<std::string, std::size_t, bool> key = false;
};

/**
\brief base class for all display units. defaulted
is the is_output function. Drawing object should override
this and return true. This enables the checking of the surface
for errors after invocation.

*/
typedef std::function<void(cairo_t *cr)> CAIRO_FUNCTION;
using CAIRO_FUNCTION = CAIRO_FUNCTION;

typedef std::variant<std::string, std::shared_ptr<std::string>, double,
                     std::vector<double>, std::size_t, Paint, line_cap_t,
                     alias_t, filter_t, extend_t, line_join_t, op_t,
                     alignment_t, ellipsize_t, CAIRO_FUNCTION>
    DisplayUnitStorage;

class DisplayUnit : public index_by {
public:
  DisplayUnit() {}
  DisplayUnit(DisplayUnitStorage _val) : _data(_val) {}
  DisplayUnit(DisplayUnitStorage _val, double _off)
      : _data(_val), _offset(_off) {}

  DisplayUnit(Paint &p, const double &d1, const double &d2) {}
  DisplayUnit(const double &d1, const double &d2, const double &d3,
              const double &d4)
      : _data(std::vector<double>({d1, d2, d3, d4})) {}
  DisplayUnit(const double &d1, const double &d2, const double &d3,
              const double &d4, const double &d5)
      : _data(std::vector<double>({d1, d2, d3, d4, d5})) {}
  DisplayUnit(const double &d1, const double &d2, const double &d3,
              const double &d4, const double &d5, const double &d6)
      : _data(std::vector<double>({d1, d2, d3, d4, d5, d6})) {}
  DisplayUnit(const CAIRO_FUNCTION &_fn) : _data(_fn) {}

  virtual ~DisplayUnit() {}
  DisplayUnit &operator=(const DisplayUnit &other) {
    bprocessed = other.bprocessed;
    viewportInked = other.viewportInked;
    _serror = other._serror;
    return *this;
  }
  DisplayUnit(const DisplayUnit &other) { *this = other; }
  DisplayUnit &operator=(const double &s) {
    _data = s;
    return *this;
  }
  DisplayUnit &operator=(const std::string &s) {
    _data = s;
    return *this;
  }
  DisplayUnit &operator=(const coordinates &s) {
    //    _data = s;
    return *this;
  }
  virtual void invoke(DisplayContext &context) {}
  virtual bool is_output(void) { return false; }
  void error(const char *s) { _serror = s; }
  bool valid(void) { return _serror == nullptr; }

  DisplayUnitStorage _data = {};
  double _offset = {};
  bool bprocessed = false;
  bool viewportInked = false;
  const char *_serror = nullptr;
};

/**
\brief base class for objects that produce image drawing commands
The is_output is overridden to return true. As well the object uses
the render work list to determine if a particular image is on screen.

*/

class DrawingOutput : public DisplayUnit {
public:
  typedef DisplayContext::CairoRegion CairoRegion;
  DrawingOutput(){};
  DrawingOutput(DisplayUnitStorage _val) : DisplayUnit(_val) {}
  DrawingOutput(DisplayUnitStorage _val, double _off)
      : DisplayUnit(_val, _off) {}

  DrawingOutput(Paint &p, const double &d1, const double &d2)
      : DisplayUnit(p, d1, d2) {}
  DrawingOutput(const double &d1, const double &d2, const double &d3,
                const double &d4)
      : DisplayUnit({d1, d2, d3, d4}) {}
  DrawingOutput(const double &d1, const double &d2, const double &d3,
                const double &d4, const double &d5)
      : DisplayUnit({d1, d2, d3, d4, d5}) {}
  DrawingOutput(const double &d1, const double &d2, const double &d3,
                const double &d4, const double &d5, const double &d6)
      : DisplayUnit({d1, d2, d3, d4, d5, d6}) {}
  DrawingOutput(const CAIRO_FUNCTION &_fn) : DisplayUnit(_fn) {}
  ~DrawingOutput() {
    if (oncethread)
      oncethread.reset();

    DisplayContext::destroy_buffer(_buf);
  }
  DrawingOutput &operator=(const DrawingOutput &other) {
    if (other._buf.rendered)
      _buf.rendered = cairo_surface_reference(other._buf.rendered);

    if (other._buf.cr)
      _buf.cr = cairo_reference(other._buf.cr);

    fnDraw = other.fnDraw;
    fnDrawClipped = other.fnDrawClipped;
    fnCacheSurface = other.fnCacheSurface;
    fnBaseSurface = other.fnBaseSurface;

    std::copy(other.options.begin(), other.options.end(),
              std::back_inserter(options));
    _inkRectangle = other._inkRectangle;
    intersection = other.intersection;
    _intersection = other._intersection;

    return *this;
  }
  DrawingOutput(const DrawingOutput &other) { *this = other; }

  bool hasInkExtents = false;
  cairo_rectangle_int_t inkRectangle = cairo_rectangle_int_t();
  cairo_region_overlap_t overlap = CAIRO_REGION_OVERLAP_OUT;
  void intersect(cairo_rectangle_t &r);
  void intersect(CairoRegion &r);

  void invoke(cairo_t *cr);
  void invoke(DisplayContext &context) {}
  bool is_output(void) { return true; }
  std::atomic<bool> bRenderBufferCached = false;
  DRAWBUFFER _buf = {};

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

  DrawLogic fnCacheSurface = DrawLogic();
  DrawLogic fnBaseSurface = DrawLogic();
  DrawLogic fnDraw = DrawLogic();
  DrawLogic fnDrawClipped = DrawLogic();

  // measure processing time
  std::chrono::system_clock::time_point lastRenderTime = {};
  void evaluate_cache(DisplayContext &context);
  bool bFirstTimeRendered = true;
  std::unique_ptr<std::thread> oncethread = nullptr;
  CairoOptionFn options = {};
  cairo_rectangle_t _inkRectangle = cairo_rectangle_t();
  cairo_rectangle_int_t intersection = cairo_rectangle_int_t();
  cairo_rectangle_t _intersection = cairo_rectangle_t();
};

using antialias = class antialias : public DisplayUnit {
public:
  antialias(alias_t _antialias) : DisplayUnit(_antialias) {}
  antialias &operator=(const antialias &other) {
    *this = other;
    return *this;
  }
  antialias(const antialias &other) {
    _data = other._data;
    key = other.key;
  }
  antialias &index(const std::string &_k) {
    key = _k;
    return *this;
  }
  antialias &index(const std::size_t &_k) {
    key = _k;
    return *this;
  }
  void invoke(DisplayContext &context) {
    if (std::holds_alternative<alias_t>(_data)) {
      auto opt = std::get<alias_t>(_data);
      cairo_set_antialias(context.cr, static_cast<cairo_antialias_t>(opt));
    }
    bprocessed = true;
  }
};

#define PAINT_OBJ(CLASS_NAME)                                                  \
  using CLASS_NAME = class CLASS_NAME : public DisplayUnit, public Paint {     \
  public:                                                                      \
    CLASS_NAME(u_int32_t c) : Paint(c) {}                                      \
    CLASS_NAME(double r, double g, double b) : Paint(r, g, b) {}               \
    CLASS_NAME(double r, double g, double b, double a) : Paint(r, g, b, a) {}  \
    CLASS_NAME(const std::string &n) : Paint(n) {}                             \
    CLASS_NAME(const std::string &n, double width, double height)              \
        : Paint(n, width, height) {}                                           \
    CLASS_NAME(double x0, double y0, double x1, double y1,                     \
               const ColorStops &_cs)                                          \
        : Paint(x0, y0, x1, y1, _cs) {}                                        \
    CLASS_NAME(double cx0, double cy0, double radius0, double cx1, double cy1, \
               double radius1, const ColorStops &cs)                           \
        : Paint(cx0, cy0, radius0, cx1, cy1, radius1, cs) {}                   \
    CLASS_NAME &operator=(const CLASS_NAME &other) { *this = other; return *this; }          \
    CLASS_NAME(const CLASS_NAME &other) : Paint(other) {                                      \
      _data = other._data;                                                     \
      key = other.key;                                                         \
    }                                                                          \
    CLASS_NAME &index(const std::string &_k) {                                 \
      key = _k;                                                                \
      return *this;                                                            \
    }                                                                          \
    CLASS_NAME &index(const std::size_t &_k) {                                 \
      key = _k;                                                                \
      return *this;                                                            \
    }                                                                          \
    void emit(cairo_t *cr) {                                                   \
      Paint::emit(cr);                                                         \
      cairo_set_line_width(cr, lineWidth);                                     \
    }                                                                          \
    void emit(cairo_t *cr, double x, double y, double w, double h) {           \
      Paint::emit(cr);                                                         \
      cairo_set_line_width(cr, lineWidth);                                     \
    }                                                                          \
    double lineWidth = 1;                                                      \
    unsigned short radius = 3;                                                 \
    double x = 1, y = 1;                                                       \
  }

PAINT_OBJ(source);
PAINT_OBJ(text_outline);
PAINT_OBJ(text_fill);
PAINT_OBJ(text_shadow);

using text_outline_none = class text_outline_none : public DisplayUnit {
public:
  text_outline_none() {}
  text_outline_none &operator=(const text_outline_none &other) {
    *this = other;
    return *this;
  }
  text_outline_none(const text_outline_none &other) {
    _data = other._data;
    key = other.key;
  }
  text_outline_none &index(const std::string &_k) {
    key = _k;
    return *this;
  }
  text_outline_none &index(const std::size_t &_k) {
    key = _k;
    return *this;
  }
  void invoke(DisplayContext &context) {
    bprocessed = true;
    context.currentUnits._text_outline.reset();
  }
};

using text_fill_none = class text_fill_none : public DisplayUnit {
public:
  text_fill_none() {}
  text_fill_none &operator=(const text_fill_none &other) {
    *this = other;
    return *this;
  }
  text_fill_none(const text_fill_none &other) {
    _data = other._data;
    key = other.key;
  }
  text_fill_none &index(const std::string &_k) {
    key = _k;
    return *this;
  }
  text_fill_none &index(const std::size_t &_k) {
    key = _k;
    return *this;
  }
  void invoke(DisplayContext &context) {
    bprocessed = true;
    context.currentUnits._text_fill.reset();
  }
};

using text_shadow_none = class text_shadow_none : public DisplayUnit {
public:
  text_shadow_none() {}
  text_shadow_none &operator=(const text_shadow_none &other) {
    *this = other;
    return *this;
  }
  text_shadow_none(const text_shadow_none &other) {
    _data = other._data;
    key = other.key;
  }
  text_shadow_none &index(const std::string &_k) {
    key = _k;
    return *this;
  }
  text_shadow_none &index(const std::size_t &_k) {
    key = _k;
    return *this;
  }

  void invoke(DisplayContext &context) {
    bprocessed = true;
    context.currentUnits._text_shadow.reset();
  }
};

using text_alignment = class text_alignment : public DisplayUnit {
public:
  text_alignment(alignment_t opt) : DisplayUnit(opt) {}
  text_alignment &operator=(const text_alignment &other) {
    *this = other;
    return *this;
  }
  text_alignment(const text_alignment &other) {
    _data = other._data;
    key = other.key;
  }
  text_alignment &index(const std::string &_k) {
    key = _k;
    return *this;
  }
  text_alignment &index(const std::size_t &_k) {
    key = _k;
    return *this;
  }

  void emit(PangoLayout *layout) {
    if (std::holds_alternative<alignment_t>(_data)) {
      auto opt = std::get<alignment_t>(_data);

      // only change setting if changed, this saves on unnecessary
      // layout context rendering internal to pango
      if (opt == alignment_t::justified && !pango_layout_get_justify(layout)) {
        pango_layout_set_justify(layout, true);
      } else if (static_cast<alignment_t>(pango_layout_get_alignment(layout)) !=
                 opt) {
        pango_layout_set_justify(layout, false);
        pango_layout_set_alignment(layout, static_cast<PangoAlignment>(opt));
      }
    }
  }
};

using coordinates = class coordinates : public DisplayUnit {
public:
  coordinates() {}
  coordinates(double _x, double _y) : DisplayUnit({_x, _y}) {}
  coordinates(double _x, double _y, double _w, double _h)
      : DisplayUnit({_x, _y, _w, _h}) {}
  coordinates(const coordinates &other) {
    *this = other;
  }
  coordinates &operator=(const coordinates &other) {
    _data = other._data;
    key = other.key;
    return *this;
  }
  coordinates &index(const std::string &_k) {
    key = _k;
    return *this;
  }
  coordinates &index(const std::size_t &_k) {
    key = _k;
    return *this;
  }
  double x(void) {
    double *dret = nullptr;
    if (std::holds_alternative<std::vector<double>>(_data)) {
      auto &val = std::get<std::vector<double>>(_data);
      dret = &val[0];
    }
    return *dret;
  }
  void x(const double &dval) {
    if (std::holds_alternative<std::vector<double>>(_data)) {
      auto &val = std::get<std::vector<double>>(_data);
      val[0] = dval;
    }
  }
  double y(void) {
    double *dret = nullptr;
    if (std::holds_alternative<std::vector<double>>(_data)) {
      auto &val = std::get<std::vector<double>>(_data);
      dret = &val[1];
    }
    return *dret;
  }
  void y(const double &dval) {
    if (std::holds_alternative<std::vector<double>>(_data)) {
      auto &val = std::get<std::vector<double>>(_data);
      val[1] = dval;
    }
  }
  double w(void) {
    double *dret = nullptr;
    if (std::holds_alternative<std::vector<double>>(_data)) {
      auto &val = std::get<std::vector<double>>(_data);
      dret = &val[2];
    }
    return *dret;
  }
  void w(const double &dval) {
    if (std::holds_alternative<std::vector<double>>(_data)) {
      auto &val = std::get<std::vector<double>>(_data);
      val[2] = dval;
    }
  }
  double h(void) {
    double *dret = nullptr;
    if (std::holds_alternative<std::vector<double>>(_data)) {
      auto &val = std::get<std::vector<double>>(_data);
      dret = &val[3];
    }
    return *dret;
  }
  void h(const double &dval) {
    if (std::holds_alternative<std::vector<double>>(_data)) {
      auto &val = std::get<std::vector<double>>(_data);
      val[3] = dval;
    }
  }
};

using line_width = class line_width : public DisplayUnit {
public:
  line_width(double lw) : DisplayUnit(lw) {}
  line_width &operator=(const line_width &other) {
    *this = other;
    return *this;
  }
  line_width(const line_width &other) {
    _data = other._data;
    key = other.key;
  }
  void invoke(DisplayContext &context) {
    bprocessed = true;
    if (std::holds_alternative<double>(_data)) {
      cairo_set_line_width(context.cr, std::get<double>(_data));
    }
  }
  line_width &index(const std::string &_k) {
    key = _k;
    return *this;
  }
  line_width &index(const std::size_t &_k) {
    key = _k;
    return *this;
  }
};

using indent = class indent : public DisplayUnit {
public:
  indent(double space) : DisplayUnit(space) {}
  indent &operator=(const indent &other) {
    *this = other;
    return *this;
  }
  indent(const indent &other) {
    _data = other._data;
    key = other.key;
  }
  void emit(PangoLayout *layout) {
    if (std::holds_alternative<double>(_data)) {
      auto val = std::get<double>(_data);
      int pangoUnits = val * PANGO_SCALE;
      pango_layout_set_indent(layout, pangoUnits);
    }
  }
  indent &index(const std::string &_k) {
    key = _k;
    return *this;
  }
  indent &index(const std::size_t &_k) {
    key = _k;
    return *this;
  }
};

using ellipsize = class ellipsize : public DisplayUnit {
public:
  ellipsize(ellipsize_t e) : DisplayUnit(e){};
  ellipsize &operator=(const ellipsize &other) {
    *this = other;
    return *this;
  }
  ellipsize(const ellipsize &other) {
    _data = other._data;
    key = other.key;
  }
  void emit(PangoLayout *layout) {
    if (std::holds_alternative<ellipsize_t>(_data)) {
      auto val = std::get<ellipsize_t>(_data);
      pango_layout_set_ellipsize(layout, static_cast<PangoEllipsizeMode>(val));
    }
  }
  ellipsize &index(const std::string &_k) {
    key = _k;
    return *this;
  }
  ellipsize &index(const std::size_t &_k) {
    key = _k;
    return *this;
  }
};

using line_space = class line_space : public DisplayUnit {
public:
  line_space(double dSpace) : DisplayUnit(dSpace){};
  line_space &operator=(const line_space &other) {
    *this = other;
    return *this;
  }
  line_space(const line_space &other) {
    _data = other._data;
    key = other.key;
  }
  void emit(PangoLayout *layout) {
    if (std::holds_alternative<double>(_data)) {
      auto val = std::get<double>(_data);
      pango_layout_set_line_spacing(layout, static_cast<float>(val));
    }
  }
  line_space &index(const std::string &_k) {
    key = _k;
    return *this;
  }
  line_space &index(const std::size_t &_k) {
    key = _k;
    return *this;
  }
};

using tab_stops = class tab_stops : public DisplayUnit {
public:
  tab_stops(const std::vector<double> &tabs) : DisplayUnit(tabs){};
  tab_stops &operator=(const tab_stops &other) {
    *this = other;
    return *this;
  }
  tab_stops(const tab_stops &other) {
    _data = other._data;
    key = other.key;
  }
  void emit(PangoLayout *layout) {
    if (std::holds_alternative<std::vector<double>>(_data)) {
      auto &val = std::get<std::vector<double>>(_data);
      PangoTabArray *tabs = pango_tab_array_new(val.size(), true);

      int idx = 0;
      for (auto &tabdef : val) {
        int loc = static_cast<int>(tabdef);
        pango_tab_array_set_tab(tabs, idx, PANGO_TAB_LEFT, loc);
      }
      pango_layout_set_tabs(layout, tabs);
      pango_tab_array_free(tabs);
    }
  }
  tab_stops &index(const std::string &_k) {
    key = _k;
    return *this;
  }
  tab_stops &index(const std::size_t &_k) {
    key = _k;
    return *this;
  }
};

using text_font = class text_font : public DisplayUnit {
public:
  text_font(const std::string &s) : DisplayUnit(s) {}
  ~text_font() {
    if (fontDescription)
      pango_font_description_free(fontDescription);
  }
  text_font &operator=(const text_font &other) {
    *this = other;
    return *this;
  }
  text_font(const text_font &other) {
    _data = other._data;
    key = other.key;
  }

  std::atomic<PangoFontDescription *> fontDescription = nullptr;
  void invoke(DisplayContext &context) {
    bprocessed = true;
    if (!fontDescription) {
      if (std::holds_alternative<std::string>(_data)) {
        auto &val = std::get<std::string>(_data);
        fontDescription = pango_font_description_from_string(val.data());
        if (!fontDescription) {
          std::string s = "Font could not be loaded from description. ( ";
          s += val + ")";
          context.error_state(__func__, __LINE__, __FILE__,
                              std::string_view(s));
        }
      }
    }
  }

  text_font &index(const std::string &_k) {
    key = _k;
    return *this;
  }
  text_font &index(const std::size_t &_k) {
    key = _k;
    return *this;
  }
};

using line_cap = class line_cap : public DisplayUnit {
public:
  line_cap(const line_cap_t _val) : DisplayUnit(_val) {}
  ~line_cap() {}
  line_cap &operator=(const line_cap &other) {
    *this = other;
    return *this;
  }
  line_cap(const line_cap &other) {
    _data = other._data;
    key = other.key;
  }
  line_cap &index(const std::string &_k) {
    key = _k;
    return *this;
  }
  line_cap &index(const std::size_t &_k) {
    key = _k;
    return *this;
  }
  void invoke(DisplayContext &context) {
    bprocessed = true;
    if (std::holds_alternative<line_cap_t>(_data)) {
      auto &val = std::get<line_cap_t>(_data);
      cairo_set_line_cap(context.cr, static_cast<cairo_line_cap_t>(val));
    }
  }
};

using line_join = class line_join : public DisplayUnit {
public:
  line_join(const line_join_t _val) : DisplayUnit(_val) {}
  ~line_join() {}
  line_join &operator=(const line_join &other) {
    *this = other;
    return *this;
  }
  line_join(const line_join &other) {
    _data = other._data;
    key = other.key;
  }
  line_join &index(const std::string &_k) {
    key = _k;
    return *this;
  }
  line_join &index(const std::size_t &_k) {
    key = _k;
    return *this;
  }
  void invoke(DisplayContext &context) {
    bprocessed = true;
    if (std::holds_alternative<line_join_t>(_data)) {
      auto &val = std::get<line_join_t>(_data);
      cairo_set_line_join(context.cr, static_cast<cairo_line_join_t>(val));
    }
  }
};

using miter_limit = class miter_limit : public DisplayUnit {
public:
  miter_limit(const double _val) : DisplayUnit(_val) {}
  ~miter_limit() {}
  miter_limit &operator=(const miter_limit &other) {
    *this = other;
    return *this;
  }
  miter_limit(const miter_limit &other) {
    _data = other._data;
    key = other.key;
  }
  miter_limit &index(const std::string &_k) {
    key = _k;
    return *this;
  }
  miter_limit &index(const std::size_t &_k) {
    key = _k;
    return *this;
  }
  void invoke(DisplayContext &context) {
    bprocessed = true;
    if (std::holds_alternative<double>(_data)) {
      auto &val = std::get<double>(_data);
      cairo_set_miter_limit(context.cr, val);
    }
  }
};

using line_dashes = class line_dashes : public DisplayUnit {
public:
  line_dashes(const std::vector<double> &_val, double _offset = 0)
      : DisplayUnit(_val, _offset) {}
  ~line_dashes() {}
  line_dashes &operator=(const line_dashes &other) {
    *this = other;
    return *this;
  }
  line_dashes(const line_dashes &other) {
    _data = other._data;
    key = other.key;
  }
  line_dashes &index(const std::string &_k) {
    key = _k;
    return *this;
  }
  line_dashes &index(const std::size_t &_k) {
    key = _k;
    return *this;
  }
  void invoke(DisplayContext &context) {
    bprocessed = true;
    if (std::holds_alternative<std::vector<double>>(_data)) {
      auto &_val = std::get<std::vector<double>>(_data);
      cairo_set_dash(context.cr, _val.data(), _val.size(), _offset);
    }
  }
};

class STRING : public DisplayUnit {
public:
  STRING(const std::string &s) : DisplayUnit(s) {}
  STRING(const std::shared_ptr<std::string> &s) : DisplayUnit(s) {}
  ~STRING() {}
  STRING &operator=(const STRING &other) {
    *this = other;
    return *this;
  }
  STRING(const STRING &other) {
    _data = other._data;
    key = other.key;
  }
  STRING &index(const std::string &_k) {
    key = _k;
    return *this;
  }
  STRING &index(const std::size_t &_k) {
    key = _k;
    return *this;
  }
  void invoke(DisplayContext &context) { bprocessed = true; }
};

class TEXT_RENDER : public DrawingOutput {
public:
  TEXT_RENDER(std::shared_ptr<STRING> data) : _text(data) {}
  ~TEXT_RENDER() {}
  TEXT_RENDER &operator=(const TEXT_RENDER &other) {
    *this = other;
    return *this;
  }
  TEXT_RENDER(const TEXT_RENDER &other) {
    _data = other._data;
    key = other.key;
  }
  TEXT_RENDER &index(const std::string &_k) {
    key = _k;
    return *this;
  }
  TEXT_RENDER &index(const std::size_t &_k) {
    key = _k;
    return *this;
  }
  bool is_output(void) { return true; }
  void invoke(DisplayContext &context);

private:
  bool set_layout_options(cairo_t *cr);
  void create_shadow(void);
  void setup_draw(DisplayContext &context);

  std::atomic<cairo_surface_t *> _shadow_image = nullptr;
  std::atomic<cairo_t *> _shadow_cr = nullptr;
  std::atomic<PangoLayout *> _layout = nullptr;
  PangoRectangle _ink_rect = PangoRectangle();
  PangoRectangle _logical_rect = PangoRectangle();
  Matrix _matrix = {};

  // local parameter pointers
  std::shared_ptr<source> _source = nullptr;
  std::shared_ptr<text_outline> _text_outline = nullptr;
  std::shared_ptr<text_fill> _text_fill = nullptr;
  std::shared_ptr<text_shadow> _text_shadow = nullptr;
  std::shared_ptr<text_font> _text_font = nullptr;
  std::shared_ptr<text_alignment> _text_alignment = nullptr;
  std::shared_ptr<coordinates> _coordinates = nullptr;
  std::shared_ptr<STRING> _text = nullptr;
};

class image : public DrawingOutput {
public:
  image(const std::string &data) : _data(data) {}
  image(const image &other) {
    *this = other;
  }
  image &operator=(const image &other) {
    _image = cairo_surface_reference(other._image);
    _data = other._data;
    key = other.key;
    bIsSVG = other.bIsSVG;
    if (_image)
      bLoaded = true;
    _coordinates = other._coordinates;
    return *this;
  }
  ~image() {
    if (_image)
      cairo_surface_destroy(_image);
  }

  image &index(const std::string &_k) {
    key = _k;
    return *this;
  }
  image &index(const std::size_t &_k) {
    key = _k;
    return *this;
  }

  void invoke(DisplayContext &context);
  bool isValid(void);
  std::atomic<cairo_surface_t *> _image = nullptr;
  std::string _data = "";
  bool bIsSVG = false;
  std::atomic<bool> bLoaded = false;
  std::shared_ptr<coordinates> _coordinates = nullptr;
};

/**
\internal
\brief call previously bound function with the cairo context.
*/

class FUNCTION : public DisplayUnit {
public:
  FUNCTION(CAIRO_FUNCTION _func) : DisplayUnit(_func) {}
  ~FUNCTION() {}
  FUNCTION &operator=(const FUNCTION &other) {
    *this = other;
    return *this;
  }
  FUNCTION(const FUNCTION &other) {
    _data = other._data;
    key = other.key;
  }
  FUNCTION &index(const std::string &_k) {
    key = _k;
    return *this;
  }
  FUNCTION &index(const std::size_t &_k) {
    key = _k;
    return *this;
  }
  void invoke(DisplayContext &context) {
    bprocessed = true;
    if (std::holds_alternative<CAIRO_FUNCTION>(_data)) {
      auto &func = std::get<CAIRO_FUNCTION>(_data);
      func(context.cr);
    }
  }
};

class DRAW_FUNCTION : public DrawingOutput {
public:
  DRAW_FUNCTION(CAIRO_FUNCTION _func) : DrawingOutput(_func) {}
  ~DRAW_FUNCTION() {}
  DRAW_FUNCTION &operator=(const DRAW_FUNCTION &other) {
    *this = other;
    return *this;
  }
  DRAW_FUNCTION(const DRAW_FUNCTION &other) {
    _data = other._data;
    key = other.key;
  }
  DRAW_FUNCTION &index(const std::string &_k) {
    key = _k;
    return *this;
  }
  DRAW_FUNCTION &index(const std::size_t &_k) {
    key = _k;
    return *this;
  }
  void invoke(DisplayContext &context);
};

class OPTION_FUNCTION : public DisplayUnit {
public:
  OPTION_FUNCTION(CAIRO_FUNCTION _func) : DisplayUnit(_func) {}
  ~OPTION_FUNCTION() {}
  OPTION_FUNCTION &operator=(const OPTION_FUNCTION &other) {
    *this = other;
    return *this;
  }
  OPTION_FUNCTION(const OPTION_FUNCTION &other) {
    _data = other._data;
    key = other.key;
  }
  OPTION_FUNCTION &index(const std::string &_k) {
    key = _k;
    return *this;
  }
  OPTION_FUNCTION &index(const std::size_t &_k) {
    key = _k;
    return *this;
  }

  void invoke(DisplayContext &context);
  void invoke(cairo_t *cr) {
    if (std::holds_alternative<CAIRO_FUNCTION>(_data)) {
      auto &func = std::get<CAIRO_FUNCTION>(_data);
      func(cr);
    }
  }
};

using tollerance = class tollerance : public DisplayUnit {
public:
  tollerance(const double _t) : DisplayUnit(_t) {}
  tollerance &operator=(const tollerance &other) {
    *this = other;
    return *this;
  }
  tollerance(const tollerance &other) {
    _data = other._data;
    key = other.key;
  }
  tollerance &index(const std::string &_k) {
    key = _k;
    return *this;
  }
  tollerance &index(const std::size_t &_k) {
    key = _k;
    return *this;
  }
  void invoke(DisplayContext &context) {
    bprocessed = true;
    if (std::holds_alternative<double>(_data)) {
      auto &_val = std::get<double>(_data);
      cairo_set_tolerance(context.cr, _val);
    }
  }
};

// draw and paint objects

using op = class op : public DisplayUnit {
public:
  op(op_t _op) : DisplayUnit(_op) {}
  op &operator=(const op &other) {
    *this = other;
    return *this;
  }
  op(const op &other) {
    _data = other._data;
    key = other.key;
  }
  op &index(const std::string &_k) {
    key = _k;
    return *this;
  }
  op &index(const std::size_t &_k) {
    key = _k;
    return *this;
  }
  void invoke(DisplayContext &context) {
    bprocessed = true;
    if (std::holds_alternative<op_t>(_data)) {
      auto &_val = std::get<op_t>(_data);
      cairo_set_operator(context.cr, static_cast<cairo_operator_t>(_val));
    }
  }
};

using close_path = class close_path : public DrawingOutput {
public:
  close_path() {}
  ~close_path() {}
  close_path &operator=(const close_path &other) {
    *this = other;
    return *this;
  }
  close_path(const close_path &other) {
    _data = other._data;
    key = other.key;
  }
  close_path &index(const std::string &_k) {
    key = _k;
    return *this;
  }
  close_path &index(const std::size_t &_k) {
    key = _k;
    return *this;
  }
  void invoke(DisplayContext &context) {
    bprocessed = true;
    cairo_close_path(context.cr);
  }
};

using arc = class arc : public DrawingOutput {
public:
  arc(double xc, double yc, double radius, double angle1, double angle2)
      : DrawingOutput(xc, yc, radius, angle1, angle2) {}
  arc &operator=(const arc &other) {
    *this = other;
    return *this;
  }
  arc(const arc &other) {
    _data = other._data;
    key = other.key;
  }
  arc &index(const std::string &_k) {
    key = _k;
    return *this;
  }
  arc &index(const std::size_t &_k) {
    key = _k;
    return *this;
  }
  void invoke(DisplayContext &context) {
    bprocessed = true;
    if (std::holds_alternative<std::vector<double>>(_data)) {
      auto &_val = std::get<std::vector<double>>(_data);
      cairo_arc(context.cr, _val[0], _val[1], _val[2], _val[3], _val[4]);
    }
  }
};

using negative_arc = class negative_arc : public DrawingOutput {
public:
  negative_arc(double xc, double yc, double radius, double angle1,
               double angle2)
      : DrawingOutput(xc, yc, radius, angle1, angle2) {}
  negative_arc &operator=(const negative_arc &other) {
    *this = other;
    return *this;
  }
  negative_arc(const negative_arc &other) {
    _data = other._data;
    key = other.key;
  }
  negative_arc &index(const std::string &_k) {
    key = _k;
    return *this;
  }
  negative_arc &index(const std::size_t &_k) {
    key = _k;
    return *this;
  }
  void invoke(DisplayContext &context) {
    bprocessed = true;
    if (std::holds_alternative<std::vector<double>>(_data)) {
      auto &_val = std::get<std::vector<double>>(_data);
      cairo_arc_negative(context.cr, _val[0], _val[1], _val[2], _val[3],
                         _val[4]);
    }
  }
};

using curve = class curve : public DrawingOutput {
public:
  curve(double x1, double y1, double x2, double y2, double x3, double y3)
      : DrawingOutput(x1, y1, x2, y2, x3, y3) {}
  curve &operator=(const curve &other) {
    *this = other;
    return *this;
  }
  curve(const curve &other) {
    _data = other._data;
    key = other.key;
  }
  curve &index(const std::string &_k) {
    key = _k;
    return *this;
  }
  curve &index(const std::size_t &_k) {
    key = _k;
    return *this;
  }
  void invoke(DisplayContext &context) {
    bprocessed = true;
    if (std::holds_alternative<std::vector<double>>(_data)) {
      auto &_val = std::get<std::vector<double>>(_data);

      if (context.bRelative)
        cairo_rel_curve_to(context.cr, _val[0], _val[1], _val[2], _val[3],
                           _val[4], _val[5]);
      else
        cairo_curve_to(context.cr, _val[0], _val[1], _val[2], _val[3], _val[4],
                       _val[5]);
    }
  }
};

using line = class line : public DrawingOutput {
public:
  line(double x, double y) : DrawingOutput(x, y) {}
  line &operator=(const line &other) {
    *this = other;
    return *this;
  }
  line(const line &other) {
    _data = other._data;
    key = other.key;
  }
  line &index(const std::string &_k) {
    key = _k;
    return *this;
  }
  line &index(const std::size_t &_k) {
    key = _k;
    return *this;
  }
  void invoke(DisplayContext &context) {
    bprocessed = true;
    if (std::holds_alternative<std::vector<double>>(_data)) {
      auto &_val = std::get<std::vector<double>>(_data);
      if (context.bRelative)
        cairo_rel_line_to(context.cr, _val[0], _val[1]);
      else
        cairo_line_to(context.cr, _val[0], _val[1]);
    }
  }
};
using hline = class hline : public DrawingOutput {
public:
  hline(double x) : DrawingOutput(x) {}
  hline &operator=(const hline &other) {
    *this = other;
    return *this;
  }
  hline(const hline &other) {
    _data = other._data;
    key = other.key;
  }
  hline &index(const std::string &_k) {
    key = _k;
    return *this;
  }
  hline &index(const std::size_t &_k) {
    key = _k;
    return *this;
  }
  void invoke(DisplayContext &context) {
    bprocessed = true;
    if (std::holds_alternative<double>(_data)) {
      auto &_val = std::get<double>(_data);

      if (cairo_has_current_point(context.cr)) {
        double curx = 0.0, cury = 0.0;
        cairo_get_current_point(context.cr, &curx, &cury);

        if (context.bRelative)
          cairo_rel_line_to(context.cr, _val, cury);
        else
          cairo_line_to(context.cr, _val, cury);
      }
    }
  }
};
using vline = class vline : public DrawingOutput {
public:
  vline(double y) : DrawingOutput(y) {}
  vline &operator=(const vline &other) {
    *this = other;
    return *this;}
    vline(const vline &other) {
      _data = other._data;
      key = other.key;
    }
    vline &index(const std::string &_k) {
      key = _k;
      return *this;
    }
    vline &index(const std::size_t &_k) {
      key = _k;
      return *this;
    }
    void invoke(DisplayContext & context) {
      bprocessed = true;
      if (std::holds_alternative<double>(_data)) {
        auto &_val = std::get<double>(_data);

        if (cairo_has_current_point(context.cr)) {
          double curx = 0.0, cury = 0.0;
          cairo_get_current_point(context.cr, &curx, &cury);

          if (context.bRelative)
            cairo_rel_line_to(context.cr, curx, _val);
          else
            cairo_line_to(context.cr, curx, _val);
        }
      }
    }
  };
  using move_to = class move_to : public DisplayUnit {
  public:
    move_to(double x, double y) : DisplayUnit(x, y) {}
    move_to &operator=(const move_to &other) {
      *this = other;
      return *this;
    }
    move_to(const move_to &other) {
      _data = other._data;
      key = other.key;
    }
    move_to &index(const std::string &_k) {
      key = _k;
      return *this;
    }
    move_to &index(const std::size_t &_k) {
      key = _k;
      return *this;
    }
    void invoke(DisplayContext &context) {
      bprocessed = true;
      if (std::holds_alternative<std::vector<double>>(_data)) {
        auto &_val = std::get<std::vector<double>>(_data);
        if (context.bRelative)
          cairo_rel_move_to(context.cr, _val[0], _val[1]);
        else
          cairo_move_to(context.cr, _val[0], _val[1]);
      }
    }
  };
  using rectangle = class rectangle : public DisplayUnit {
  public:
    rectangle(double x, double y, double width, double height)
        : DisplayUnit(x, y, width, height) {}
    rectangle &operator=(const rectangle &other) {
      *this = other;
      return *this;
    }
    rectangle(const rectangle &other) {
      _data = other._data;
      key = other.key;
    }
    rectangle &index(const std::string &_k) {
      key = _k;
      return *this;
    }
    rectangle &index(const std::size_t &_k) {
      key = _k;
      return *this;
    }
    void invoke(DisplayContext &context) {
      bprocessed = true;
      if (std::holds_alternative<std::vector<double>>(_data)) {
        auto &_val = std::get<std::vector<double>>(_data);
        cairo_rectangle(context.cr, _val[0], _val[1], _val[2], _val[3]);
      }
    }
  };

  using stroke_path_preserve = class stroke_path_preserve : public DisplayUnit {
  public:
    stroke_path_preserve(Paint &p) : DisplayUnit(p) {}
    stroke_path_preserve &operator=(const stroke_path_preserve &other) {
      *this = other;
      return *this;
    }
    stroke_path_preserve(const stroke_path_preserve &other) {
      _data = other._data;
      key = other.key;
    }
    stroke_path_preserve &index(const std::string &_k) {
      key = _k;
      return *this;
    }
    stroke_path_preserve &index(const std::size_t &_k) {
      key = _k;
      return *this;
    }
    void invoke(DisplayContext &context) {
      bprocessed = true;
      if (std::holds_alternative<Paint>(_data)) {
        auto &_val = std::get<Paint>(_data);
        _val.emit(context.cr);
        cairo_stroke_preserve(context.cr);
      }
    }
  };
  using stroke_path = class stroke_path : public DisplayUnit {
  public:
    stroke_path(Paint &p) : DisplayUnit(p) {}
    stroke_path &operator=(const stroke_path &other) {
      *this = other;
      return *this;
    }
    stroke_path(const stroke_path &other) {
      _data = other._data;
      key = other.key;
    }
    stroke_path &index(const std::string &_k) {
      key = _k;
      return *this;
    }
    stroke_path &index(const std::size_t &_k) {
      key = _k;
      return *this;
    }
    void invoke(DisplayContext &context) {
      bprocessed = true;
      if (std::holds_alternative<Paint>(_data)) {
        auto &_val = std::get<Paint>(_data);
        _val.emit(context.cr);
        cairo_stroke(context.cr);
      }
    }
  };
  using fill_path_preserve = class fill_path_preserve : public DisplayUnit {
  public:
    fill_path_preserve(Paint &p) : DisplayUnit(p) {}
    fill_path_preserve &operator=(const fill_path_preserve &other) {
      *this = other;
      return *this;
    }
    fill_path_preserve(const fill_path_preserve &other) {
      _data = other._data;
      key = other.key;
    }
    fill_path_preserve &index(const std::string &_k) {
      key = _k;
      return *this;
    }
    fill_path_preserve &index(const std::size_t &_k) {
      key = _k;
      return *this;
    }
    void invoke(DisplayContext &context) {
      bprocessed = true;
      if (std::holds_alternative<Paint>(_data)) {
        auto &_val = std::get<Paint>(_data);
        _val.emit(context.cr);
        cairo_fill_preserve(context.cr);
      }
    }
  };
  using fill_path = class fill_path : public DisplayUnit {
  public:
    fill_path(Paint &p) : DisplayUnit(p) {}
    fill_path &operator=(const fill_path &other) {
      *this = other;
      return *this;
    }
    fill_path(const fill_path &other) {
      _data = other._data;
      key = other.key;
    }
    fill_path &index(const std::string &_k) {
      key = _k;
      return *this;
    }
    fill_path &index(const std::size_t &_k) {
      key = _k;
      return *this;
    }
    void invoke(DisplayContext &context) {
      bprocessed = true;
      if (std::holds_alternative<Paint>(_data)) {
        auto &_val = std::get<Paint>(_data);
        _val.emit(context.cr);
        cairo_fill(context.cr);
      }
    }
  };
  using mask = class mask : public DisplayUnit {
  public:
    mask(Paint &p) : DisplayUnit(p) {}
    mask(Paint &p, double x, double y) : DisplayUnit(p, x, y) {}
    mask &operator=(const mask &other) {
      *this = other;
      return *this;
    }
    mask(const mask &other) {
      _data = other._data;
      key = other.key;
    }
    mask &index(const std::string &_k) {
      key = _k;
      return *this;
    }
    mask &index(const std::size_t &_k) {
      key = _k;
      return *this;
    }
  };

  using paint = class paint : public DisplayUnit {
  public:
    paint(double alpha = 1.0) : DisplayUnit(alpha) {}
    paint &operator=(const paint &other) {
      *this = other;
      return *this;
    }
    paint(const paint &other) {
      _data = other._data;
      key = other.key;
    }
    paint &index(const std::string &_k) {
      key = _k;
      return *this;
    }
    paint &index(const std::size_t &_k) {
      key = _k;
      return *this;
    }
    void invoke(DisplayContext &context) {
      bprocessed = true;
      if (std::holds_alternative<double>(_data)) {
        auto &_val = std::get<double>(_data);
        if (_val == 1.0) {
          cairo_paint(context.cr);
        } else {
          cairo_paint_with_alpha(context.cr, _val);
        }
      }
    }
  };

  using relative = class relative : public DisplayUnit {
    relative &operator=(const relative &other) {
      *this = other;
      return *this;
    }
    relative(const relative &other) {
      _data = other._data;
      key = other.key;
    }
    relative &index(const std::string &_k) {
      key = _k;
      return *this;
    }
    relative &index(const std::size_t &_k) {
      key = _k;
      return *this;
    }
  };
  using absolute = class absolute : public DisplayUnit {
    absolute &operator=(const absolute &other) {
      *this = other;
      return *this;
    }
    absolute(const absolute &other) {
      _data = other._data;
      key = other.key;
    }
    absolute &index(const std::string &_k) {
      key = _k;
      return *this;
    }
    absolute &index(const std::size_t &_k) {
      key = _k;
      return *this;
    }
  };

  // listeners are named from this base class.
  using listener = class listener : public DisplayUnit {
  public:
    listener(const eventType &_etype, const eventHandler &_evtDispatcher)
        : evtDispatcher(_evtDispatcher) {}
    listener &operator=(const listener &other) {
      *this = other;
      return *this;
    }
    listener(const listener &other) {
      _data = other._data;
      key = other.key;
    }
    listener &index(const std::string &_k) {
      key = _k;
      return *this;
    }
    listener &index(const std::size_t &_k) {
      key = _k;
      return *this;
    }
    eventType etype = {};
    eventHandler evtDispatcher = {};
  };

  // Named event listeners shorten the code a little and make the code more
  // readable. pasting macros would be nice, however, its not the c++ standard
  using listen_paint = class listen_paint : public listener {
  public:
    listen_paint(const eventHandler &_evtDispatcher)
        : listener(eventType::paint, _evtDispatcher) {}
    listen_paint &operator=(const listen_paint &other) {
      *this = other;
      return *this;
    }
    listen_paint(const listen_paint &other) : listener(other) {
      _data = other._data;
      key = other.key;
    }
    listen_paint &index(const std::string &_k) {
      key = _k;
      return *this;
    }
    listen_paint &index(const std::size_t &_k) {
      key = _k;
      return *this;
    }
  };
  using listen_focus = class listen_focus : public listener {
  public:
    listen_focus(const eventHandler &_evtDispatcher)
        : listener(eventType::focus, _evtDispatcher) {}
    listen_focus &operator=(const listen_focus &other) {
      *this = other;
      return *this;
    }
    listen_focus(const listen_focus &other) : listener(other) {
      _data = other._data;
      key = other.key;
    }
    listen_focus &index(const std::string &_k) {
      key = _k;
      return *this;
    }
    listen_focus &index(const std::size_t &_k) {
      key = _k;
      return *this;
    }
  };
  using listen_blur = class listen_blur : public listener {
  public:
    listen_blur(const eventHandler &_evtDispatcher)
        : listener(eventType::blur, _evtDispatcher) {}
    listen_blur &operator=(const listen_blur &other) {
      *this = other;
      return *this;
    }
    listen_blur(const listen_blur &other) : listener(other) {
      _data = other._data;
      key = other.key;
    }
    listen_blur &index(const std::string &_k) {
      key = _k;
      return *this;
    }
    listen_blur &index(const std::size_t &_k) {
      key = _k;
      return *this;
    }
  };
  using listen_resize = class listen_resize : public listener {
  public:
    listen_resize(const eventHandler &_evtDispatcher)
        : listener(eventType::resize, _evtDispatcher) {}
    listen_resize &operator=(const listen_resize &other) {
      *this = other;
      return *this;
    }
    listen_resize(const listen_resize &other) : listener(other){
      _data = other._data;
      key = other.key;
    }
    listen_resize &index(const std::string &_k) {
      key = _k;
      return *this;
    }
    listen_resize &index(const std::size_t &_k) {
      key = _k;
      return *this;
    }
  };
  using listen_keydown = class listen_keydown : public listener {
  public:
    listen_keydown(const eventHandler &_evtDispatcher)
        : listener(eventType::keydown, _evtDispatcher) {}
    listen_keydown &operator=(const listen_keydown &other) {
      *this = other;
      return *this;
    }
    listen_keydown(const listen_keydown &other) : listener(other) {
      _data = other._data;
      key = other.key;
    }
    listen_keydown &index(const std::string &_k) {
      key = _k;
      return *this;
    }
    listen_keydown &index(const std::size_t &_k) {
      key = _k;
      return *this;
    }
  };
  using listen_keyup = class listen_keyup : public listener {
  public:
    listen_keyup(const eventHandler &_evtDispatcher)
        : listener(eventType::keyup, _evtDispatcher) {}
    listen_keyup &operator=(const listen_keyup &other) {
      *this = other;
      return *this;
    }
    listen_keyup(const listen_keyup &other): listener(other) {
      _data = other._data;
      key = other.key;
    }
    listen_keyup &index(const std::string &_k) {
      key = _k;
      return *this;
    }
    listen_keyup &index(const std::size_t &_k) {
      key = _k;
      return *this;
    }
  };
  using listen_keypress = class listen_keypress : public listener {
  public:
    listen_keypress(const eventHandler &_evtDispatcher)
        : listener(eventType::keypress, _evtDispatcher) {}
    listen_keypress &operator=(const listen_keypress &other) {
      *this = other;
      return *this;
    }
    listen_keypress(const listen_keypress &other): listener(other) {
      _data = other._data;
      key = other.key;
    }
    listen_keypress &index(const std::string &_k) {
      key = _k;
      return *this;
    }
    listen_keypress &index(const std::size_t &_k) {
      key = _k;
      return *this;
    }
  };
  using listen_mouseenter = class listen_mouseenter : public listener {
  public:
    listen_mouseenter(const eventHandler &_evtDispatcher)
        : listener(eventType::mouseenter, _evtDispatcher) {}
    listen_mouseenter &operator=(const listen_mouseenter &other) {
      *this = other;
      return *this;
    }
    listen_mouseenter(const listen_mouseenter &other): listener(other) {
      _data = other._data;
      key = other.key;
    }
    listen_mouseenter &index(const std::string &_k) {
      key = _k;
      return *this;
    }
    listen_mouseenter &index(const std::size_t &_k) {
      key = _k;
      return *this;
    }
  };
  using listen_mousemove = class listen_mousemove : public listener {
  public:
    listen_mousemove(const eventHandler &_evtDispatcher)
        : listener(eventType::mousemove, _evtDispatcher) {}
    listen_mousemove &operator=(const listen_mousemove &other) {
      *this = other;
      return *this;
    }
    listen_mousemove(const listen_mousemove &other) : listener(other){
      _data = other._data;
      key = other.key;
    }
    listen_mousemove &index(const std::string &_k) {
      key = _k;
      return *this;
    }
    listen_mousemove &index(const std::size_t &_k) {
      key = _k;
      return *this;
    }
  };
  using listen_mousedown = class listen_mousedown : public listener {
  public:
    listen_mousedown(const eventHandler &_evtDispatcher)
        : listener(eventType::mousedown, _evtDispatcher) {}
    listen_mousedown &operator=(const listen_mousedown &other) {
      *this = other;
      return *this;
    }
    listen_mousedown(const listen_mousedown &other) : listener(other){
      _data = other._data;
      key = other.key;
    }
    listen_mousedown &index(const std::string &_k) {
      key = _k;
      return *this;
    }
    listen_mousedown &index(const std::size_t &_k) {
      key = _k;
      return *this;
    }
  };
  using listen_mouseup = class listen_mouseup : public listener {
  public:
    listen_mouseup(const eventHandler &_evtDispatcher)
        : listener(eventType::mouseup, _evtDispatcher) {}
    listen_mouseup &operator=(const listen_mouseup &other) {
      *this = other;
      return *this;
    }
    listen_mouseup(const listen_mouseup &other): listener(other) {
      _data = other._data;
      key = other.key;
    }
    listen_mouseup &index(const std::string &_k) {
      key = _k;
      return *this;
    }
    listen_mouseup &index(const std::size_t &_k) {
      key = _k;
      return *this;
    }
  };
  using listen_click = class listen_click : public listener {
  public:
    listen_click(const eventHandler &_evtDispatcher)
        : listener(eventType::click, _evtDispatcher) {}
    listen_click &operator=(const listen_click &other) {
      *this = other;
      return *this;
    }
    listen_click(const listen_click &other): listener(other) {
      _data = other._data;
      key = other.key;
    }
    listen_click &index(const std::string &_k) {
      key = _k;
      return *this;
    }
    listen_click &index(const std::size_t &_k) {
      key = _k;
      return *this;
    }
  };
  using listen_dblclick = class listen_dblclick : public listener {
  public:
    listen_dblclick(const eventHandler &_evtDispatcher)
        : listener(eventType::dblclick, _evtDispatcher) {}
    listen_dblclick &operator=(const listen_dblclick &other) {
      *this = other;
      return *this;
    }
    listen_dblclick(const listen_dblclick &other): listener(other) {
      _data = other._data;
      key = other.key;
    }
    listen_dblclick &index(const std::string &_k) {
      key = _k;
      return *this;
    }
    listen_dblclick &index(const std::size_t &_k) {
      key = _k;
      return *this;
    }
  };
  using listen_contextmenu = class listen_contextmenu : public listener {
  public:
    listen_contextmenu(const eventHandler &_evtDispatcher)
        : listener(eventType::contextmenu, _evtDispatcher) {}
    listen_contextmenu &operator=(const listen_contextmenu &other) {
      *this = other;
      return *this;
    }
    listen_contextmenu(const listen_contextmenu &other) : listener(other){
      _data = other._data;
      key = other.key;
    }
    listen_contextmenu &index(const std::string &_k) {
      key = _k;
      return *this;
    }
    listen_contextmenu &index(const std::size_t &_k) {
      key = _k;
      return *this;
    }
  };
  using listen_wheel = class listen_wheel : public listener {
  public:
    listen_wheel(const eventHandler &_evtDispatcher)
        : listener(eventType::wheel, _evtDispatcher) {}
    listen_wheel &operator=(const listen_wheel &other) {
      *this = other;
      return *this;
    }
    listen_wheel(const listen_wheel &other) : listener(other){
      _data = other._data;
      key = other.key;
    }
    listen_wheel &index(const std::string &_k) {
      key = _k;
      return *this;
    }
    listen_wheel &index(const std::size_t &_k) {
      key = _k;
      return *this;
    }
  };
  using listen_mouseleave = class listen_mouseleave : public listener {
  public:
    listen_mouseleave(const eventHandler &_evtDispatcher)
        : listener(eventType::mouseleave, _evtDispatcher) {}
    listen_mouseleave &operator=(const listen_mouseleave &other) {
      *this = other;
      return *this;
    }
    listen_mouseleave(const listen_mouseleave &other) : listener(other){
      _data = other._data;
      key = other.key;
    }
    listen_mouseleave &index(const std::string &_k) {
      key = _k;
      return *this;
    }
    listen_mouseleave &index(const std::size_t &_k) {
      key = _k;
      return *this;
    }
  };
} // namespace uxdevice
