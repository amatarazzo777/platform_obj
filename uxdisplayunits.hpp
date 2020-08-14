/**
\author Anthony Matarazzo
\file uxworkstate.hpp
\date 5/12/20
\version 1.0
 \details The class holds the context of parameters for
 any drawing operations that occur. Such as AREA, or STRING.
 These objects have named slot positions within the by
 contextUnit.


*/
#pragma once

namespace uxdevice {

/**
\brief base class for all display units. defaulted
is the is_output function. Drawing object should override
this and return true. This enables the checking of the surface
for errors after invocation.

*/
class DisplayUnit {
public:
  DisplayUnit() {}
  virtual ~DisplayUnit() {}
  DisplayUnit &operator=(const DisplayUnit &other) {
    bprocessed = other.bprocessed;
    viewportInked = other.viewportInked;
    _serror = other._serror;
    return *this;
  }
  DisplayUnit(const DisplayUnit &other) { *this = other; }
  virtual void invoke(DisplayContext &context) {}
  virtual bool is_output(void) { return false; }
  void error(const char *s) { _serror = s; }
  bool valid(void) { return _serror == nullptr; }

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

typedef std::function<void(void)> CLEAR_FUNCTION;

class CLEARUNIT : public DisplayUnit {
public:
  CLEARUNIT(CLEAR_FUNCTION _fn) : fn(_fn) {}

  CLEARUNIT &operator=(const CLEARUNIT &other) {
    fn = other.fn;
    return *this;
  }
  CLEARUNIT(const CLEARUNIT &other) { *this = other; }
  void invoke(DisplayContext &context) {
    fn();
    bprocessed = true;
  }
  CLEAR_FUNCTION fn = {};
};


using antialias = class antialias : public DisplayUnit {
public:
  antialias(alias_t _antialias)
      : setting(static_cast<cairo_antialias_t>(_antialias)) {}

  void invoke(DisplayContext &context) {
    cairo_set_antialias(context.cr, setting);
    bprocessed = true;
  }

  cairo_antialias_t setting;
};

#define PAINT_OBJ(X_NAME)                                                      \
  using X_NAME = class X_NAME : public DisplayUnit, public Paint {             \
  public:                                                                      \
    X_NAME(u_int32_t c) : Paint(c) {}                                          \
    X_NAME(double r, double g, double b) : Paint(r, g, b) {}                   \
    X_NAME(double r, double g, double b, double a) : Paint(r, g, b, a) {}      \
    X_NAME(const std::string &n) : Paint(n) {}                                 \
    X_NAME(const std::string &n, double width, double height)                  \
        : Paint(n, width, height) {}                                           \
    X_NAME(double x0, double y0, double x1, double y1, const ColorStops &_cs)  \
        : Paint(x0, y0, x1, y1, _cs) {}                                        \
    X_NAME(double cx0, double cy0, double radius0, double cx1, double cy1,     \
           double radius1, const ColorStops &cs)                               \
        : Paint(cx0, cy0, radius0, cx1, cy1, radius1, cs) {}                   \
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
PAINT_OBJ(fill_path);
PAINT_OBJ(stroke_path);

using source_default = class source_default : public DisplayUnit {
public:
  source_default() {}
};

using text_outline_none =  class text_outline_none : public DisplayUnit {
public:
  text_outline_none() {}
};

using text_fill_none  = class text_fill_none : public DisplayUnit {
public:
  text_fill_none() {}
};

using text_shadow_none  = class text_shadow_none : public DisplayUnit {
public:
  text_shadow_none() {}
};

using text_alignment = class text_alignment : public DisplayUnit  {
public:
  enum setting {
    left = PangoAlignment::PANGO_ALIGN_LEFT,
    center = PangoAlignment::PANGO_ALIGN_CENTER,
    right = PangoAlignment::PANGO_ALIGN_RIGHT,
    justified = 4
  };
  setting _setting = setting::left;
  text_alignment(setting opt) : _setting(opt) {}
  void emit(PangoLayout *layout) {
    // only change setting if changed, this saves on unnecessary
    // layout context rendering internal to pango
    if (_setting == setting::justified && !pango_layout_get_justify(layout)) {
      pango_layout_set_justify(layout, true);
    } else if (static_cast<setting>(pango_layout_get_alignment(layout)) !=
               _setting) {
      pango_layout_set_justify(layout, false);
      pango_layout_set_alignment(layout, static_cast<PangoAlignment>(_setting));
    }
  }
};
using coordinates = class coordinates : public DisplayUnit {
public:
  coordinates(double _x, double _y) : x(_x), y(_y) {}
  coordinates(double _x, double _y, double _w, double _h)
      : x(_x), y(_y), w(_w), h(_h) {}
  double x = 0, y = 0, w = 0, h = 0;
};

using index_by = class index_by : public DisplayUnit {
public:
  index_by(const std::string &s) : skey(s) {}
  index_by(const std::size_t n) : nkey(n) {}
  std::string skey = "";
  std::size_t nkey = 0;
};

#if 0
std::unordered_map<std::string, std::list<std::reference_wrapper<DisplayUnit>>>
    mappedString = {};
std::unordered_map<std::size_t, std::list<std::reference_wrapper<DisplayUnit>>>
    mappedInteger = {};
#endif

using line_width = class line_width : public DisplayUnit {
public:
  line_width(double lw) {}
};

using indent = class indent : public DisplayUnit {
public:
  indent(double space);
};

using ellipse = class ellipse : public DisplayUnit {
public:
  ellipse(ellipsize_t e);
};

using line_space = class line_space : public DisplayUnit {
public:
  line_space(double dSpace);
};

using tab_stops = class tab_stops : public DisplayUnit {
public:
  tab_stops(const std::vector<double> &tabs);
};

using text_font = class text_font : public DisplayUnit {
public:
  text_font(const std::string &s) : description(s) {}
  ~text_font() {
    if (fontDescription)
      pango_font_description_free(fontDescription);
  }
  text_font &operator=(const text_font &other) = delete;
  text_font(const text_font &other) { description = other.description; }
  std::string description ={};
  std::atomic<PangoFontDescription *> fontDescription = nullptr;
  void invoke(DisplayContext &context) {
    if (!fontDescription) {
      fontDescription = pango_font_description_from_string(description.data());
      if (!fontDescription) {
        std::string s = "Font could not be loaded from description. ( ";
        s += description + ")";
        context.error_state(__func__, __LINE__, __FILE__, std::string_view(s));
      }
    }
  }
};


using line_cap = class line_cap : public DisplayUnit {
public:
  line_cap(const line_cap_t _val) : data(_val) {}
  ~line_cap() { }
  line_cap_t data =line_cap_t::butt;
};

using line_join = class line_join : public DisplayUnit {
public:
  line_join(const line_join_t _val) : data(_val) {}
  ~line_join() {
  }
  line_join_t data =line_join_t::miter;
};

using miter_limit = class miter_limit : public DisplayUnit {
public:
  miter_limit(const double _val) : data(_val) {}
  ~miter_limit() {
  }
  double data=1.0;
};

using line_dashes = class line_dashes : public DisplayUnit {
public:
  line_dashes(const std::vector<double> &_val, double _offset=0) : data(_val), offset(_offset)  {}
  ~line_dashes() {
  }
  std::vector<double> data={};
  double offset={};
};

class STRING : public DisplayUnit {
public:
  STRING(const std::string &s) : data(s) {}
  ~STRING() {}
  std::string data;
  void invoke(DisplayContext &context) { bprocessed = true; }
};

class TEXT_RENDER : public DrawingOutput {
public:
  TEXT_RENDER(std::shared_ptr<STRING> data) : _text(data) {}
  TEXT_RENDER(const TEXT_RENDER &other) = delete;
  TEXT_RENDER &operator=(const TEXT_RENDER &other) = delete;
  bool is_output(void) { return true; }
  ~TEXT_RENDER() {}
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
  image(const image &other) { *this = other; }
  image &operator=(const image &other) {
    _image = cairo_surface_reference(other._image);
    _data = other._data;
    bIsSVG = other.bIsSVG;
    if(_image)
      bLoaded = true;
    _coordinates = other._coordinates;
    return *this;
  }
  ~image() {
    if (_image)
      cairo_surface_destroy(_image);
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
typedef std::function<void(cairo_t *cr)> CAIRO_FUNCTION;
class FUNCTION : public DisplayUnit {
public:
  FUNCTION(CAIRO_FUNCTION _func) : func(_func) {}
  ~FUNCTION() {}
  void invoke(DisplayContext &context) {
    bprocessed = true;
    func(context.cr);
  }

private:
  CAIRO_FUNCTION func;
};

class DRAW_FUNCTION : public DrawingOutput {
public:
  DRAW_FUNCTION(CAIRO_FUNCTION _func) : func(_func) {}
  ~DRAW_FUNCTION() {}
  void invoke(DisplayContext &context);

private:
  CAIRO_FUNCTION func;
};

typedef std::function<void(cairo_t *cr)> CAIRO_OPTION;
class OPTION_FUNCTION : public DisplayUnit {
public:
  OPTION_FUNCTION(CAIRO_OPTION _func) : fnOption(_func) {}
  ~OPTION_FUNCTION() {}
  void invoke(DisplayContext &context);

  CAIRO_OPTION fnOption;
};
} // namespace uxdevice
