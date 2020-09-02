/**
\author Anthony Matarazzo
\file uxdevice.hpp
\date 3/26/20
\version 1.0
\brief interface for the platform.

*/
#pragma once

#define PI (3.14159265358979323846264338327f)

/**
\addtogroup Library Build Options
\brief Library Options
\details These options provide the selection to configure selection
options when compiling the text_color.
@{
*/

#define DEFAULT_TEXTFACE "arial"
#define DEFAULT_TEXTSIZE 12
#define DEFAULT_TEXTCOLOR 0
#define USE_STACKBLUR

/**
\def USE_DEBUG_CONSOLE
*/
#define USE_DEBUG_CONSOLE
#define CONSOLE
/** @} */

#include "uxbase.hpp"

#include "uxcairoimage.hpp"
#include "uxevent.hpp"
#include "uxmatrix.hpp"
#include "uxpaint.hpp"

#include "uxdisplaycontext.hpp"
#include "uxdisplayunits.hpp"

std::string _errorReport(std::string text_colorFile, int ln, std::string sfunc,
                         std::string cond, std::string ecode);
typedef std::function<void(const std::string &err)> errorHandler;

namespace uxdevice {

class event;

/**
 \details
*/

using bounds = class bounds {
public:
  double x = 0, y = 0, w = 0, h = 0;
};

using point = class point {
public:
  double x = 0, y = 0;
};

/**
\internal
\class platform
\brief The platform contains logic to connect the document object model to the
local operating system.
*/
typedef std::list<short int> CoordinateList;

class surface_area {
public:
  surface_area();
  surface_area(const std::string &ssurface_areaTitle);

  surface_area(const event_handler_t &evtDispatcher);
  surface_area(const CoordinateList &coordinates);

  surface_area(const CoordinateList &coordinates,
               const std::string &ssurface_areaTitle);
  surface_area(const CoordinateList &coordinates,
               const std::string &ssurface_areaTitle,
               const painter_brush_t &background);

  surface_area(const CoordinateList &coordinates,
               const std::string &ssurface_areaTitle,
               const event_handler_t &evtDispatcher,
               const painter_brush_t &background);
  ~surface_area();

  std::unordered_map<indirect_index_t, std::shared_ptr<display_unit_t>>
      mapped_objects = {};

  /* the macro creates the stream interface for both constant references
  and shared pointers as well as establishes the prototype for the insertion
  function
  */

#define STREAM_INPUT(CLASS_NAME)                                               \
public:                                                                        \
  surface_area &operator<<(const CLASS_NAME &data) {                           \
    stream_input(data);                                                        \
    return *this;                                                              \
  }                                                                            \
  surface_area &operator<<(const std::shared_ptr<CLASS_NAME> data) {           \
    stream_input(data);                                                        \
    return *this;                                                              \
  }                                                                            \
                                                                               \
private:                                                                       \
  surface_area &stream_input(const CLASS_NAME &_val);                          \
  surface_area &stream_input(const std::shared_ptr<CLASS_NAME> _val);

  template <typename T> surface_area &operator<<(const T &data) {
    std::ostringstream s;
    s << data;
    std::string sData = s.str();
    stream_input(sData);
    return *this;
  }

  STREAM_INPUT(std::string)
  STREAM_INPUT(std::stringstream)
  STREAM_INPUT(char *)
  STREAM_INPUT(antialias)
  STREAM_INPUT(text_color)
  STREAM_INPUT(text_outline)
  STREAM_INPUT(text_fill)
  STREAM_INPUT(text_shadow)
  STREAM_INPUT(text_outline_off)
  STREAM_INPUT(text_fill_off)
  STREAM_INPUT(text_shadow_off)
  STREAM_INPUT(text_alignment)
  STREAM_INPUT(alignment_t)

  STREAM_INPUT(coordinates)
  STREAM_INPUT(indent)
  STREAM_INPUT(ellipsize)
  STREAM_INPUT(line_space)
  STREAM_INPUT(tab_stops)
  STREAM_INPUT(text_font)
  STREAM_INPUT(line_width)
  STREAM_INPUT(line_cap)
  STREAM_INPUT(line_join)
  STREAM_INPUT(miter_limit)
  STREAM_INPUT(line_dashes)
  STREAM_INPUT(image_block)
  STREAM_INPUT(stroke_path)
  STREAM_INPUT(fill_path)
  STREAM_INPUT(stroke_path_preserve)
  STREAM_INPUT(fill_path_preserve)
  STREAM_INPUT(close_path)

  STREAM_INPUT(arc)
  STREAM_INPUT(negative_arc)
  STREAM_INPUT(curve)
  STREAM_INPUT(line)
  STREAM_INPUT(hline)
  STREAM_INPUT(vline)
  STREAM_INPUT(move_to)
  STREAM_INPUT(rectangle)
  STREAM_INPUT(tollerance)
  STREAM_INPUT(op)
  STREAM_INPUT(absolute)
  STREAM_INPUT(relative)

  STREAM_INPUT(listener)
  STREAM_INPUT(listen_paint)
  STREAM_INPUT(listen_focus)
  STREAM_INPUT(listen_blur)
  STREAM_INPUT(listen_resize)
  STREAM_INPUT(listen_keydown)
  STREAM_INPUT(listen_keyup)
  STREAM_INPUT(listen_keypress)
  STREAM_INPUT(listen_mouseenter)
  STREAM_INPUT(listen_mousemove)
  STREAM_INPUT(listen_mousedown)
  STREAM_INPUT(listen_mouseup)
  STREAM_INPUT(listen_click)
  STREAM_INPUT(listen_dblclick)
  STREAM_INPUT(listen_contextmenu)
  STREAM_INPUT(listen_wheel)
  STREAM_INPUT(listen_mouseleave)

public:
  display_unit_t &operator[](const indirect_index_t &idx) {
    auto n = mapped_objects.find(idx);
    return *n->second;
  }

  // return display unit associated, update
  display_unit_t &operator[](const std::shared_ptr<std::string> _val) {
    auto n = mapped_objects.find(reinterpret_cast<std::size_t>(_val.get()));
    return *n->second;
  }
  display_unit_t &group(const std::string &sgroupname) {
    auto n = mapped_objects.find(sgroupname);
    return *n->second;
  }

  bool processing(void) { return bProcessing; };
  surface_area &group(std::string &_name);

  surface_area &device_offset(double x, double y);
  surface_area &device_scale(double x, double y);
  surface_area &surface_brush(painter_brush_t &b);
  void clear(void);
  void notify_complete(void);

  surface_area &save(void);
  surface_area &restore(void);

  surface_area &push(content_t _content = content_t::all);
  surface_area &pop(bool bToSource = false);

  surface_area &translate(double x, double y);
  surface_area &rotate(double angle);
  surface_area &scale(double x, double y);
  surface_area &transform(const Matrix &mat);
  surface_area &matrix(const Matrix &mat);
  surface_area &identity(void);
  surface_area &device(double &x, double &y);
  surface_area &device_distance(double &x, double &y);
  surface_area &user(double &x, double &y);
  surface_area &user_distance(double &x, double &y);
  point location(void);

  bounds stroke(void);
  bool in_stroke(double x, double y);
  bool in_fill(double x, double y);

  bounds clip(void);
  void clip(bool bPreserve = false);
  bool in_clip(double x, double y);

private:
  void start_processing(void);
  void draw_caret(const int x, const int y, const int h);
  void message_loop(void);
  void render_loop(void);
  void dispatch_event(const event_t &e);
  void close_window(void);
  bool relative = false;
  void maintain_index(std::shared_ptr<display_unit_t> obj);

  display_context_t context = display_context_t();
  std::atomic<bool> bProcessing = false;
  errorHandler fnError = nullptr;
  event_handler_t fnEvents = nullptr;

  typedef std::list<std::shared_ptr<display_unit_t>> display_unit_storage_t;
  display_unit_storage_t DL = {};
  display_unit_storage_t::iterator itDL_Processed = DL.begin();

  std::atomic_flag DL_readwrite = ATOMIC_FLAG_INIT;

#define DL_SPIN while (DL_readwrite.test_and_set(std::memory_order_acquire))
#define DL_CLEAR DL_readwrite.clear(std::memory_order_release)

  std::list<event_handler_t> onfocus = {};
  std::list<event_handler_t> onblur = {};
  std::list<event_handler_t> onresize = {};
  std::list<event_handler_t> onkeydown = {};
  std::list<event_handler_t> onkeyup = {};
  std::list<event_handler_t> onkeypress = {};
  std::list<event_handler_t> onmouseenter = {};
  std::list<event_handler_t> onmouseleave = {};
  std::list<event_handler_t> onmousemove = {};
  std::list<event_handler_t> onmousedown = {};
  std::list<event_handler_t> onmouseup = {};
  std::list<event_handler_t> onclick = {};
  std::list<event_handler_t> ondblclick = {};
  std::list<event_handler_t> oncontextmenu = {};
  std::list<event_handler_t> onwheel = {};

  std::list<event_handler_t> &get_event_vector(eventType evtType);
};

} // namespace uxdevice
