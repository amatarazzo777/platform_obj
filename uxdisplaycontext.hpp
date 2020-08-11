/**
\author Anthony Matarazzo
\file uxdisplaycontext.hpp
\date 5/12/20
\version 1.0
\version 1.0
 \details CLass holds the display window context, gui drawing, cairo
 context, and provides an interface for threads running to
 invalidate part of the surface, resize the surface. The
 CurrentUnits class is within the public members and holds
 the state of the last used data parameters.

*/
#pragma once

namespace uxdevice {

class image;
class text_font;
class antialias;
class text_shadow;
class text_fill;
class text_outline;
class coordinates;
class source;
class text_alignment;
class TEXT_RENDER;
class FUNCTION;
class OPTION_FUNCTION;
class DRAW_FUNCTION;
class DisplayUnit;
class DrawingOutput;

typedef std::list<std::shared_ptr<DisplayUnit>> DisplayUnitCollection;
typedef std::list<std::shared_ptr<DisplayUnit>>::iterator
    DisplayUnitCollectionIter;
typedef std::list<std::shared_ptr<DrawingOutput>> DrawingOutputCollection;
typedef std::list<std::shared_ptr<DrawingOutput>>::iterator
    DrawingOutputCollectionIter;

class DisplayContext;
typedef std::function<void(DisplayContext &context)> DrawLogic;

typedef std::list<OPTION_FUNCTION *> CairoOptionFn;
typedef struct _DRAWBUFFER {
  cairo_t *cr = nullptr;
  cairo_surface_t *rendered = nullptr;
} DRAWBUFFER;

class CurrentUnits {
public:
  std::shared_ptr<text_font> _text_font = nullptr;
  std::shared_ptr<antialias> _antialias = nullptr;
  std::shared_ptr<text_shadow> _text_shadow = nullptr;
  std::shared_ptr<text_fill> _text_fill = nullptr;
  std::shared_ptr<text_outline> _text_outline = nullptr;
  std::shared_ptr<source> _source = nullptr;
  std::shared_ptr<std::string> _text = nullptr;
  std::shared_ptr<text_alignment> _text_alignment = nullptr;
  std::shared_ptr<coordinates> _coordinates = nullptr;

  CairoOptionFn _options = {};
};

class DisplayContext {
public:
  class CairoRegion {
  public:
    CairoRegion() = delete;
    CairoRegion(bool bOS, int x, int y, int w, int h) {
      rect = {x, y, w, h};
      _rect = {(double)x, (double)y, (double)w, (double)h};
      _ptr = cairo_region_create_rectangle(&rect);
      bOSsurface = bOS;
    }
    CairoRegion(std::size_t _obj, int x, int y, int w, int h) : obj(_obj) {
      rect = {x, y, w, h};
      _rect = {(double)x, (double)y, (double)w, (double)h};
      _ptr = cairo_region_create_rectangle(&rect);
      bOSsurface = false;
    }

    CairoRegion(const CairoRegion &other) { *this = other; }
    CairoRegion &operator=(const CairoRegion &other) {
      _ptr = cairo_region_reference(other._ptr);
      rect = other.rect;
      _rect = other._rect;
      obj = other.obj;
      bOSsurface = other.bOSsurface;
      return *this;
    }
    ~CairoRegion() {
      if (_ptr)
        cairo_region_destroy(_ptr);
    }
    cairo_rectangle_int_t rect = cairo_rectangle_int_t();
    cairo_rectangle_t _rect = cairo_rectangle_t();
    cairo_region_t *_ptr = nullptr;
    std::size_t obj = 0;
    bool bOSsurface = false;
  };

public:
  DisplayContext(void) {}

  DisplayContext(const DisplayContext &other) { *this = other; }
  DisplayContext &operator=(const DisplayContext &other) {
    windowX = other.windowX;
    windowY = other.windowY;
    windowWidth = other.windowWidth;
    windowHeight = other.windowHeight;
    windowOpen = other.windowOpen;
    if (other.cr)
      cr = cairo_reference(other.cr);
    _regions = other._regions;
    _surfaceRequests = other._surfaceRequests;

    xdisplay = other.xdisplay;
    connection = other.connection;
    screen = other.screen;
    window = other.window;
    graphics = other.graphics;
    visualType = other.visualType;
    syms = other.syms;
    xcbSurface = other.xcbSurface;
    preclear = other.preclear;

    return *this;
  }

  DrawingOutputCollection viewportOff = {};
  std::atomic_flag drawables_off_readwrite = ATOMIC_FLAG_INIT;
#define DRAWABLES_OFF_SPIN                                                     \
  while (drawables_off_readwrite.test_and_set(std::memory_order_acquire))
#define DRAWABLES_OFF_CLEAR                                                    \
  drawables_off_readwrite.clear(std::memory_order_release)

  DrawingOutputCollection viewportOn = {};
  std::atomic_flag drawables_on_readwrite = ATOMIC_FLAG_INIT;
#define DRAWABLES_ON_SPIN                                                      \
  while (drawables_on_readwrite.test_and_set(std::memory_order_acquire))
#define DRAWABLES_ON_CLEAR                                                     \
  drawables_on_readwrite.clear(std::memory_order_release)

  bool surface_prime(void);
  void plot(CairoRegion &plotArea);
  void flush(void);
  void device_offset(double x, double y);
  void device_scale(double x, double y);

  void resize_surface(const int w, const int h);

  void offsetPosition(const int x, const int y);
  void surface_brush(Paint &b);

  void render(void);
  void add_drawable(std::shared_ptr<DrawingOutput> _obj);
  void partition_visibility(void);
  void state(std::shared_ptr<DrawingOutput> obj);
  void state(int x, int y, int w, int h);
  bool state(void);
  void state_surface(int x, int y, int w, int h);
  void state_notify_complete(void);

  DRAWBUFFER allocate_buffer(int width, int height);
  static void destroy_buffer(DRAWBUFFER &_buffer);
  void clear(void);

  std::atomic_flag lockErrors = ATOMIC_FLAG_INIT;
#define ERRORS_SPIN while (lockErrors.test_and_set(std::memory_order_acquire))
#define ERRORS_CLEAR lockErrors.clear(std::memory_order_release)
  std::list<std::string> _errors = {};

  void error_state(const std::string_view &sfunc, const std::size_t linenum,
                   const std::string_view &sfile, const cairo_status_t stat);
  void error_state(const std::string_view &sfunc, const std::size_t linenum,
                   const std::string_view &sfile, const std::string_view &desc);
  void error_state(const std::string_view &sfunc, const std::size_t linenum,
                   const std::string_view &sfile, const std::string &desc);
  bool error_state(void);
  std::string error_text(bool bclear = true);

  cairo_status_t error_check(cairo_surface_t *sur) {
    return cairo_surface_status(sur);
  }
  cairo_status_t error_check(cairo_t *cr) { return cairo_status(cr); }

  CurrentUnits currentUnits = CurrentUnits();


  void set_unit(std::shared_ptr<std::string> _text) { currentUnits._text = _text; };
  void set_unit(std::shared_ptr<text_font> _font) { currentUnits._text_font = _font; };
  void set_unit(std::shared_ptr<antialias> _antialias) {
    currentUnits._antialias = _antialias;
  };
  void set_unit(std::shared_ptr<text_shadow> _text_shadow) {
    currentUnits._text_shadow = _text_shadow;
  };
  void set_unit(std::shared_ptr<text_fill> _text_fill) {
    currentUnits._text_fill = _text_fill;
  };
  void set_unit(std::shared_ptr<text_outline> _text_outline) {
    currentUnits._text_outline = _text_outline;
  };
  void set_unit(std::shared_ptr<source> _source) {
    currentUnits._source = _source;
  };

  void set_unit(std::shared_ptr<text_alignment> _align) { currentUnits._text_alignment = _align; };
  void set_unit(std::shared_ptr<coordinates> _pos) { currentUnits._coordinates = _pos; };

public:
  short windowX = 0;
  short windowY = 0;
  unsigned short windowWidth = 0;
  unsigned short windowHeight = 0;
  bool windowOpen = false;

  std::atomic_flag lockBrush = ATOMIC_FLAG_INIT;
#define BRUSH_SPIN while (lockBrush.test_and_set(std::memory_order_acquire))
#define BRUSH_CLEAR lockBrush.clear(std::memory_order_release)
  Paint brush = Paint("white");

  cairo_t *cr = nullptr;

  cairo_rectangle_t viewportRectangle = cairo_rectangle_t();

private:
  std::list<CairoRegion> _regions = {};
  typedef std::list<CairoRegion>::iterator RegionIter;

  std::atomic_flag lockRegions = ATOMIC_FLAG_INIT;
#define REGIONS_SPIN while (lockRegions.test_and_set(std::memory_order_acquire))
#define REGIONS_CLEAR lockRegions.clear(std::memory_order_release)

  typedef struct _WH {
    int w = 0;
    int h = 0;
    _WH(int _w, int _h) : w(_w), h(_h) {}
  } WH;
  std::list<_WH> _surfaceRequests = {};
  typedef std::list<_WH>::iterator SurfaceRequestsIter;
  std::atomic_flag lockSurfaceRequests = ATOMIC_FLAG_INIT;
#define SURFACE_REQUESTS_SPIN                                                  \
  while (lockSurfaceRequests.test_and_set(std::memory_order_acquire))

#define SURFACE_REQUESTS_CLEAR                                                 \
  lockSurfaceRequests.clear(std::memory_order_release)

  int offsetx = 0, offsety = 0;
  void apply_surface_requests(void);
  std::mutex mutexRenderWork = {};
  std::condition_variable cvRenderWork = {};

public:
  // if render request time for objects are less than x ms
  int cacheThreshold = 200;

  std::atomic<bool> bClearFrame = false;
  Display *xdisplay = nullptr;
  xcb_connection_t *connection = nullptr;
  xcb_screen_t *screen = nullptr;
  xcb_drawable_t window = 0;
  xcb_gcontext_t graphics = 0;

  xcb_visualtype_t *visualType = nullptr;

  // xcb -- keyboard
  xcb_key_symbols_t *syms = nullptr;

  cairo_surface_t *xcbSurface = nullptr;
  std::atomic_flag lockXCBSurface = ATOMIC_FLAG_INIT;
#define XCB_SPIN while (lockXCBSurface.test_and_set(std::memory_order_acquire))
#define XCB_CLEAR lockXCBSurface.clear(std::memory_order_release)
  void lock(bool b) {
    if (b) {
      XCB_SPIN;
    } else {
      XCB_CLEAR;
    }
  }
  bool preclear = false;
};
} // namespace uxdevice
