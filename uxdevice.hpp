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
options when compiling the source.
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

std::string _errorReport(std::string sourceFile, int ln, std::string sfunc,
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

class SurfaceArea {
public:
  SurfaceArea();
  SurfaceArea(const std::string &sSurfaceAreaTitle);

  SurfaceArea(const eventHandler &evtDispatcher);
  SurfaceArea(const CoordinateList &coordinates);

  SurfaceArea(const CoordinateList &coordinates,
              const std::string &sSurfaceAreaTitle);
  SurfaceArea(const CoordinateList &coordinates,
              const std::string &sSurfaceAreaTitle, const Paint &background);

  SurfaceArea(const CoordinateList &coordinates,
              const std::string &sSurfaceAreaTitle,
              const eventHandler &evtDispatcher, const Paint &background);
  ~SurfaceArea();
  std::unordered_map<std::string, std::shared_ptr<DisplayUnit>> mappedString =
      {};
  std::unordered_map<std::size_t, std::shared_ptr<DisplayUnit>> mappedInteger =
      {};

  /* the macro creates the stream interface for both constant references
  and shared pointers as well as establishes the prototype for the insertion
  function
  */

#define STREAM_INPUT(CLASS_NAME)                                               \
public:                                                                        \
  SurfaceArea &operator<<(const CLASS_NAME &data) {                            \
    stream_input(data);                                                        \
    return *this;                                                              \
  }                                                                            \
  SurfaceArea &operator<<(const std::shared_ptr<CLASS_NAME> data) {            \
    stream_input(data);                                                        \
    return *this;                                                              \
  }                                                                            \
                                                                               \
private:                                                                       \
  SurfaceArea &stream_input(const CLASS_NAME &_val);                           \
  SurfaceArea &stream_input(const std::shared_ptr<CLASS_NAME> _val);

  template <typename T> SurfaceArea &operator<<(const T &data) {
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
  STREAM_INPUT(source)
  STREAM_INPUT(text_outline)
  STREAM_INPUT(text_fill)
  STREAM_INPUT(text_shadow)
  STREAM_INPUT(text_outline_none)
  STREAM_INPUT(text_fill_none)
  STREAM_INPUT(text_shadow_none)
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
  STREAM_INPUT(image)
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
  DisplayUnit &operator[](const std::string s) {
    auto n = mappedString.find(s);
    return *n->second;
  }
  DisplayUnit &operator[](const std::size_t &nval) {
    auto n = mappedInteger.find(nval);
    return *n->second;
  }
  // return display unit associated, update
  DisplayUnit &operator[](const std::shared_ptr<std::string> _val) {
    auto n = mappedInteger.find(reinterpret_cast<std::size_t>(_val.get()));
    return *n->second;
  }
  DisplayUnit &group(const std::string &sgroupname) {
    auto n = mappedString.find(sgroupname);
    return *n->second;
  }

  bool processing(void) { return bProcessing; };
  SurfaceArea &group(std::string &_name);

  SurfaceArea &device_offset(double x, double y);
  SurfaceArea &device_scale(double x, double y);
  SurfaceArea &surface_brush(Paint &b);
  void clear(void);
  void notify_complete(void);

  SurfaceArea &save(void);
  SurfaceArea &restore(void);

  SurfaceArea &push(content_t _content = content_t::all);
  SurfaceArea &pop(bool bToSource = false);

  SurfaceArea &translate(double x, double y);
  SurfaceArea &rotate(double angle);
  SurfaceArea &scale(double x, double y);
  SurfaceArea &transform(const Matrix &mat);
  SurfaceArea &matrix(const Matrix &mat);
  SurfaceArea &identity(void);
  SurfaceArea &device(double &x, double &y);
  SurfaceArea &device_distance(double &x, double &y);
  SurfaceArea &user(double &x, double &y);
  SurfaceArea &user_distance(double &x, double &y);
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
  bool bRelative = false;
  void maintain_index(std::shared_ptr<DisplayUnit> obj);

  DisplayContext context = DisplayContext();
  std::atomic<bool> bProcessing = false;
  errorHandler fnError = nullptr;
  eventHandler fnEvents = nullptr;

  typedef std::list<std::shared_ptr<DisplayUnit>> DisplayUnitStorage;
  DisplayUnitStorage DL = {};
  DisplayUnitStorage::iterator itDL_Processed = DL.begin();

  std::atomic_flag DL_readwrite = ATOMIC_FLAG_INIT;

#define DL_SPIN while (DL_readwrite.test_and_set(std::memory_order_acquire))
#define DL_CLEAR DL_readwrite.clear(std::memory_order_release)

  std::list<eventHandler> onfocus = {};
  std::list<eventHandler> onblur = {};
  std::list<eventHandler> onresize = {};
  std::list<eventHandler> onkeydown = {};
  std::list<eventHandler> onkeyup = {};
  std::list<eventHandler> onkeypress = {};
  std::list<eventHandler> onmouseenter = {};
  std::list<eventHandler> onmouseleave = {};
  std::list<eventHandler> onmousemove = {};
  std::list<eventHandler> onmousedown = {};
  std::list<eventHandler> onmouseup = {};
  std::list<eventHandler> onclick = {};
  std::list<eventHandler> ondblclick = {};
  std::list<eventHandler> oncontextmenu = {};
  std::list<eventHandler> onwheel = {};

  std::list<eventHandler> &get_event_vector(eventType evtType);
};

} // namespace uxdevice
