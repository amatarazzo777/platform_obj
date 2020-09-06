/**
\author Anthony Matarazzo
\file uxdisplaycontext.hpp
\date 5/12/20
\version 1.0
\version 1.0
 \details CLass holds the display window context, gui drawing, cairo
 context, and provides an interface for threads running to
 invalidate part of the surface, resize the surface. The
 current_units_t class is within the public members and holds
 the state of the last used data parameters.

*/
#pragma once

namespace uxdevice {

class image_block;
class text_font;
class antialias;
class text_shadow;
class text_fill;
class text_outline;
class coordinates;
class text_color;
class text_alignment;
class index_by;
class line_width;
class indent;
class ellipsize;
class line_space;
class tab_stops;
class textual_render;
class function_object_t;
class option_function_object_t;
class textual_data;
class draw_function_object_t;
class display_unit_t;
class drawing_output_t;

typedef std::list<std::shared_ptr<display_unit_t>> display_unit_collection_t;
typedef std::list<std::shared_ptr<display_unit_t>>::iterator
    display_unit_collection_iter_t;
typedef std::list<std::shared_ptr<drawing_output_t>>
    drawing_output_collection_t;
typedef std::list<std::shared_ptr<drawing_output_t>>::iterator
    drawing_output_collection_iter_t;

class display_context_t;
typedef std::function<void(display_context_t &context)> draw_logic_t;

typedef std::list<option_function_object_t *> cairo_option_function_t;
typedef struct _draw_buffer_t {
  cairo_t *cr = nullptr;
  cairo_surface_t *rendered = nullptr;
} draw_buffer_t;

class current_units_t {
public:
  std::shared_ptr<text_font> _text_font = nullptr;
  std::shared_ptr<antialias> _antialias = nullptr;
  std::shared_ptr<text_shadow> _text_shadow = nullptr;
  std::shared_ptr<text_fill> _text_fill = nullptr;
  std::shared_ptr<text_outline> _text_outline = nullptr;
  std::shared_ptr<text_color> _text_color = nullptr;
  std::shared_ptr<textual_data> _text = nullptr;
  std::shared_ptr<text_alignment> _text_alignment = nullptr;
  std::shared_ptr<coordinates> _coordinates = nullptr;
  std::shared_ptr<index_by> _index_by = nullptr;
  std::shared_ptr<line_width> _line_width = nullptr;
  std::shared_ptr<indent> _indent = nullptr;
  std::shared_ptr<ellipsize> _ellipsize = nullptr;
  std::shared_ptr<line_space> _line_space = nullptr;
  std::shared_ptr<tab_stops> _tab_stops = nullptr;
  cairo_option_function_t _options = {};
};

class display_context_t {
public:
  class context_cairo_region_t {
  public:
    context_cairo_region_t() = delete;
    context_cairo_region_t(bool bOS, int x, int y, int w, int h) {
      rect = {x, y, w, h};
      _rect = {(double)x, (double)y, (double)w, (double)h};
      _ptr = cairo_region_create_rectangle(&rect);
      bOSsurface = bOS;
    }
    context_cairo_region_t(std::size_t _obj, int x, int y, int w, int h)
        : obj(_obj) {
      rect = {x, y, w, h};
      _rect = {(double)x, (double)y, (double)w, (double)h};
      _ptr = cairo_region_create_rectangle(&rect);
      bOSsurface = false;
    }

    context_cairo_region_t(const context_cairo_region_t &other) {
      *this = other;
    }
    context_cairo_region_t &operator=(const context_cairo_region_t &other) {
      _ptr = cairo_region_reference(other._ptr);
      rect = other.rect;
      _rect = other._rect;
      obj = other.obj;
      bOSsurface = other.bOSsurface;
      return *this;
    }
    ~context_cairo_region_t() {
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
  display_context_t(void) {}

  display_context_t(const display_context_t &other) { *this = other; }
  display_context_t &operator=(const display_context_t &other) {
    window_x = other.window_x;
    window_y = other.window_y;
    window_width = other.window_width;
    window_height = other.window_height;
    window_open = other.window_open;
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

  drawing_output_collection_t viewport_off = {};
  std::atomic_flag drawables_off_readwrite = ATOMIC_FLAG_INIT;
#define DRAWABLES_OFF_SPIN                                                     \
  while (drawables_off_readwrite.test_and_set(std::memory_order_acquire))
#define DRAWABLES_OFF_CLEAR                                                    \
  drawables_off_readwrite.clear(std::memory_order_release)

  drawing_output_collection_t viewport_on = {};
  std::atomic_flag drawables_on_readwrite = ATOMIC_FLAG_INIT;
#define DRAWABLES_ON_SPIN                                                      \
  while (drawables_on_readwrite.test_and_set(std::memory_order_acquire))
#define DRAWABLES_ON_CLEAR                                                     \
  drawables_on_readwrite.clear(std::memory_order_release)

  bool surface_prime(void);
  void plot(context_cairo_region_t &plotArea);
  void flush(void);
  void device_offset(double x, double y);
  void device_scale(double x, double y);

  void resize_surface(const int w, const int h);

  void offsetPosition(const int x, const int y);
  void surface_brush(painter_brush_t &b);

  void render(void);
  void add_drawable(std::shared_ptr<drawing_output_t> _obj);
  void partition_visibility(void);
  void state(std::shared_ptr<drawing_output_t> obj);
  void state(int x, int y, int w, int h);
  bool state(void);
  void state_surface(int x, int y, int w, int h);
  void state_notify_complete(void);

  draw_buffer_t allocate_buffer(int width, int height);
  static void destroy_buffer(draw_buffer_t &_buffer);
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

  current_units_t current_units = current_units_t();

  void set_unit(std::shared_ptr<textual_data> _text) {
    current_units._text = _text;
  };
  void set_unit(std::shared_ptr<text_font> _font) {
    current_units._text_font = _font;
  };
  void set_unit(std::shared_ptr<antialias> _antialias) {
    current_units._antialias = _antialias;
  };
  void set_unit(std::shared_ptr<text_shadow> _text_shadow) {
    current_units._text_shadow = _text_shadow;
  };
  void set_unit(std::shared_ptr<text_fill> _text_fill) {
    current_units._text_fill = _text_fill;
  };
  void set_unit(std::shared_ptr<text_outline> _text_outline) {
    current_units._text_outline = _text_outline;
  };
  void set_unit(std::shared_ptr<text_color> _text_color) {
    current_units._text_color = _text_color;
  };

  void set_unit(std::shared_ptr<text_alignment> _align) {
    current_units._text_alignment = _align;
  };
  void set_unit(std::shared_ptr<coordinates> _pos) {
    current_units._coordinates = _pos;
  };
  void set_unit(std::shared_ptr<index_by> _val) {
    current_units._index_by = _val;
  };
  void set_unit(std::shared_ptr<line_width> _val) {
    current_units._line_width = _val;
  };
  void set_unit(std::shared_ptr<indent> _val) { current_units._indent = _val; };
  void set_unit(std::shared_ptr<ellipsize> _val) {
    current_units._ellipsize = _val;
  };
  void set_unit(std::shared_ptr<line_space> _val) {
    current_units._line_space = _val;
  };
  void set_unit(std::shared_ptr<tab_stops> _val) {
    current_units._tab_stops = _val;
  };

public:
  short window_x = 0;
  short window_y = 0;
  unsigned short window_width = 0;
  unsigned short window_height = 0;
  bool window_open = false;
  std::atomic<bool> relative = false;

  std::atomic_flag lockBrush = ATOMIC_FLAG_INIT;
#define BRUSH_SPIN while (lockBrush.test_and_set(std::memory_order_acquire))
#define BRUSH_CLEAR lockBrush.clear(std::memory_order_release)
  painter_brush_t brush = painter_brush_t("white");

  cairo_t *cr = nullptr;

  cairo_rectangle_t viewport_rectangle = cairo_rectangle_t();

private:
  std::list<context_cairo_region_t> _regions = {};
  typedef std::list<context_cairo_region_t>::iterator region_iter_t;

  std::atomic_flag lockRegions = ATOMIC_FLAG_INIT;
#define REGIONS_SPIN while (lockRegions.test_and_set(std::memory_order_acquire))
#define REGIONS_CLEAR lockRegions.clear(std::memory_order_release)

  typedef struct _WH {
    int w = 0;
    int h = 0;
    _WH(int _w, int _h) : w(_w), h(_h) {}
  } __WH;
  std::list<_WH> _surfaceRequests = {};
  typedef std::list<_WH>::iterator surface_requests_iter_t;
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
  int cache_threshold = 200;

  std::atomic<bool> clearing_frame = false;
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
