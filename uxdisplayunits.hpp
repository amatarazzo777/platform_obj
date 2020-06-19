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

class ANTIALIAS : public DisplayUnit {
public:
  ANTIALIAS(alias_t _antialias)
      : setting(static_cast<cairo_antialias_t>(_antialias)) {}

  void invoke(DisplayContext &context) {
    cairo_set_antialias(context.cr, setting);
    bprocessed = true;
  }

  cairo_antialias_t setting;
};

class FONT : public DisplayUnit {
public:
  FONT(const std::string &s)
      : description(s), pointSize(DEFAULT_TEXTSIZE), bProvidedName(false),
        bProvidedSize(false), bProvidedDescription(true) {}
  FONT(const std::string &s, const double &pt)
      : description(s), pointSize(pt) {}

  FONT &operator=(const FONT &other) = delete;
  FONT(const FONT &);

  ~FONT() {
    if (fontDescription)
      pango_font_description_free(fontDescription);
  }
  std::string description = DEFAULT_TEXTFACE;
  double pointSize = DEFAULT_TEXTSIZE;
  bool bProvidedName = false;
  bool bProvidedSize = false;
  bool bProvidedDescription = false;
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
    bprocessed = true;
  }
};

class STROKE : public DisplayUnit, public Paint {
public:
  STROKE(const Paint &c) : Paint(c) {}
  STROKE(u_int32_t c) : Paint(c) {}
  STROKE(double _r, double _g, double _b) : Paint(_r, _g, _b) {}
  STROKE(double _r, double _g, double _b, double _a) : Paint(_r, _g, _b, _a) {}
  STROKE(const std::string &n) : Paint(n) {}
  STROKE(const std::string &n, double w, double h) : Paint(n, w, h) {}
  STROKE(double x0, double y0, double x1, double y1, const ColorStops &cs)
      : Paint(x0, y0, x1, y1, cs) {}
  STROKE(double cx0, double cy0, double radius0, double cx1, double cy1,
         double radius1, const ColorStops &cs)
      : Paint(cx0, cy0, radius0, cx1, cy1, radius1, cs) {}
  ~STROKE() {}
  void invoke(DisplayContext &context) { bprocessed = true; }
};

class FILL : public DisplayUnit, public Paint {
public:
  FILL(const Paint &c) : Paint(c) {}
  FILL(u_int32_t c) : Paint(c) {}
  FILL(double _r, double _g, double _b) : Paint(_r, _g, _b) {}
  FILL(double _r, double _g, double _b, double _a) : Paint(_r, _g, _b, _a) {}
  FILL(const std::string &n) : Paint(n) {}
  FILL(const std::string &n, double w, double h) : Paint(n, w, h) {}
  FILL(double x0, double y0, double x1, double y1, const ColorStops &cs)
      : Paint(x0, y0, x1, y1, cs) {}
  FILL(double cx0, double cy0, double radius0, double cx1, double cy1,
       double radius1, const ColorStops &cs)
      : Paint(cx0, cy0, radius0, cx1, cy1, radius1, cs) {}
  ~FILL() {}
  void invoke(DisplayContext &context) { bprocessed = true; }
};

class ALIGN : public DisplayUnit {
public:
  ALIGN(alignment_t _aln) : setting(_aln) {}
  ~ALIGN() {}
  void emit(PangoLayout *layout);
  alignment_t setting = alignment_t::left;
  void invoke(DisplayContext &context) { bprocessed = true; }
};

class TEXTSHADOW : public DisplayUnit, public Paint {
public:
  TEXTSHADOW(const Paint &c, int r, double xOffset, double yOffset)
      : Paint(c), radius(r), x(xOffset), y(yOffset) {}
  TEXTSHADOW(u_int32_t c, int r, double xOffset, double yOffset)
      : Paint(c), radius(r), x(xOffset), y(yOffset) {}
  TEXTSHADOW(double _r, double _g, double _b, int r, double xOffset,
             double yOffset)
      : Paint(_r, _g, _b), radius(r), x(xOffset), y(yOffset) {}
  TEXTSHADOW(double _r, double _g, double _b, double _a, int r, double xOffset,
             double yOffset)
      : Paint(_r, _g, _b, _a), radius(r), x(xOffset), y(yOffset) {}
  TEXTSHADOW(const std::string &n, int r, double xOffset, double yOffset)
      : Paint(n), radius(r), x(xOffset), y(yOffset) {}
  TEXTSHADOW(const std::string &n, double w, double h, int r, double xOffset,
             double yOffset)
      : Paint(n, w, h), radius(r), x(xOffset), y(yOffset) {}
  TEXTSHADOW(double x0, double y0, double x1, double y1, const ColorStops &cs,
             int r, double xOffset, double yOffset)
      : Paint(x0, y0, x1, y1, cs), radius(r), x(xOffset), y(yOffset) {}
  TEXTSHADOW(double cx0, double cy0, double radius0, double cx1, double cy1,
             double radius1, const ColorStops &cs, int r, double xOffset,
             double yOffset)
      : Paint(cx0, cy0, radius0, cx1, cy1, radius1, cs), radius(r), x(xOffset),
        y(yOffset) {}

  ~TEXTSHADOW() {}
  void invoke(DisplayContext &context) { bprocessed = true; }

public:
  unsigned short radius = 3;
  double x = 1, y = 1;
};

class TEXTOUTLINE : public Paint, public DisplayUnit {
public:
  TEXTOUTLINE(const Paint &c, double _lineWidth)
      : Paint(c), lineWidth(_lineWidth) {}
  TEXTOUTLINE(u_int32_t c, double _lineWidth)
      : Paint(c), lineWidth(_lineWidth) {}
  TEXTOUTLINE(double _r, double _g, double _b, double _lineWidth)
      : Paint(_r, _g, _b), lineWidth(_lineWidth) {}
  TEXTOUTLINE(double _r, double _g, double _b, double _a, double _lineWidth)
      : Paint(_r, _g, _b, _a), lineWidth(_lineWidth) {}
  TEXTOUTLINE(const std::string &n, double _lineWidth)
      : Paint(n), lineWidth(_lineWidth) {}
  TEXTOUTLINE(const std::string &n, double w, double h, double _lineWidth)
      : Paint(n, w, h), lineWidth(_lineWidth) {}
  TEXTOUTLINE(double x0, double y0, double x1, double y1, const ColorStops &cs,
              double _lineWidth)
      : Paint(x0, y0, x1, y1, cs), lineWidth(_lineWidth) {}
  TEXTOUTLINE(double cx0, double cy0, double radius0, double cx1, double cy1,
              double radius1, const ColorStops &cs, double _lineWidth)
      : Paint(cx0, cy0, radius0, cx1, cy1, radius1, cs), lineWidth(_lineWidth) {
  }
  void emit(cairo_t *cr) {
    Paint::emit(cr);
    cairo_set_line_width(cr, lineWidth);
  }
  void emit(cairo_t *cr, double x, double y, double w, double h) {
    Paint::emit(cr);
    cairo_set_line_width(cr, lineWidth);
  }

  void invoke(DisplayContext &context) { bprocessed = true; }

public:
  double lineWidth = .5;
};

class TEXTFILL : public Paint, public DisplayUnit {
public:
  TEXTFILL(const Paint &c) : Paint(c) {}
  TEXTFILL(u_int32_t c) : Paint(c) {}
  TEXTFILL(double _r, double _g, double _b) : Paint(_r, _g, _b) {}
  TEXTFILL(double _r, double _g, double _b, double _a)
      : Paint(_r, _g, _b, _a) {}
  TEXTFILL(const std::string &n) : Paint(n) {}
  TEXTFILL(const std::string &n, double w, double h) : Paint(n, w, h) {}
  TEXTFILL(double x0, double y0, double x1, double y1, const ColorStops &cs)
      : Paint(x0, y0, x1, y1, cs) {}
  TEXTFILL(double cx0, double cy0, double radius0, double cx1, double cy1,
           double radius1, const ColorStops &cs)
      : Paint(cx0, cy0, radius0, cx1, cy1, radius1, cs) {}

  ~TEXTFILL() {}
  void invoke(DisplayContext &context) { bprocessed = true; }
};

class TEXT_RENDER : public DrawingOutput {
public:
  TEXT_RENDER(std::string _data) : data(_data) {}
  TEXT_RENDER(const TEXT_RENDER &other) = delete;
  TEXT_RENDER &operator=(const TEXT_RENDER &other) = delete;
  bool is_output(void) { return true; }
  ~TEXT_RENDER() {}
  void invoke(DisplayContext &context);

private:
  bool set_layout_options(cairo_t *cr);
  void create_shadow(void);

  std::atomic<cairo_surface_t *> shadowImage = nullptr;
  std::atomic<cairo_t *> shadowCr = nullptr;
  std::atomic<PangoLayout *> layout = nullptr;
  PangoRectangle ink_rect = PangoRectangle();
  PangoRectangle logical_rect = PangoRectangle();
  Matrix mat = {};

  // local parameter pointers
  std::shared_ptr<SOURCE> source = nullptr;
  std::shared_ptr<TEXTOUTLINE> textoutline = nullptr;
  std::shared_ptr<TEXTFILL> textfill = nullptr;
  std::shared_ptr<TEXTSHADOW> textshadow = nullptr;
  std::shared_ptr<STRING> text = nullptr;
  std::shared_ptr<FONT> font = nullptr;
  std::shared_ptr<ALIGN> align = nullptr;
  std::string data;
};

class IMAGE : public DisplayUnit {
public:
  IMAGE(const std::string &data) : _data(data) {}
  IMAGE(const IMAGE &other) { *this = other; }
  IMAGE &operator=(const IMAGE &other) {
    _image = cairo_surface_reference(other._image);
    return *this;
  }
  ~IMAGE() {
    if (_image)
      cairo_surface_destroy(_image);
  }

  void invoke(DisplayContext &context);
  std::atomic<cairo_surface_t *> _image = nullptr;
  std::string _data = "";
  bool bIsSVG = false;
  std::atomic<bool> bLoaded = false;
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
