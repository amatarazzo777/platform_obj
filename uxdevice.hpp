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

//#define CLIP_OUTLINE
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
  template <typename T> SurfaceArea &operator<<(const T &data) {
    std::ostringstream s;
    s << data;
    std::string sData = s.str();

    return *this;
  }
  SurfaceArea &operator<<(const std::string &data) {
    stream_input(data);
    return *this;
  }
  SurfaceArea &operator<<(const char *data) {
    stream_input(data);
    return *this;
  }
  SurfaceArea &operator<<(const source &data) {
    stream_input(data);
    return *this;
  }
  SurfaceArea &operator<<(const text_outline &data) {
    stream_input(data);
    return *this;
  }
  SurfaceArea &operator<<(const text_fill &data) {
    stream_input(data);
    return *this;
  }
  SurfaceArea &operator<<(const text_shadow &data) {
    stream_input(data);
    return *this;
  }
  SurfaceArea &operator<<(const text_outline_none &data) {
    stream_input(data);
    return *this;
  }
  SurfaceArea &operator<<(const text_fill_none &data) {
    stream_input(data);
    return *this;
  }
  SurfaceArea &operator<<(const text_shadow_none &data) {
    stream_input(data);
    return *this;
  }
  SurfaceArea &operator<<(const text_alignment &data) {
    stream_input(data);
    return *this;
  }
  SurfaceArea &operator<<(const text_alignment::setting &data) {
    stream_input(data);
    return *this;
  }
  SurfaceArea &operator<<(const alignment_t &data) {
    stream_input(static_cast<text_alignment::setting>(data));
    return *this;
  }
  SurfaceArea &operator<<(const coordinates &data) {
    stream_input(data);
    return *this;
  }
  SurfaceArea &operator<<(const index_by &data) { return *this; }

  SurfaceArea &operator<<(const indent &data) {
    stream_input(data);
    return *this;
  }

  SurfaceArea &operator<<(const ellipse &data) {
    stream_input(data);
    return *this;
  }
  SurfaceArea &operator<<(const line_space &data) {
    stream_input(data);
    return *this;
  }
  SurfaceArea &operator<<(const tab_stops &data) {
    stream_input(data);
    return *this;
  }
  SurfaceArea &operator<<(const text_font &data) {
    stream_input(data);
    return *this;
  }
  SurfaceArea &operator<<(const line_width &data) {
    stream_input(data);
    return *this;
  }
  SurfaceArea &operator<<(const stroke_path &data) {
    stream_input(data);
    return *this;
  }
  SurfaceArea &operator<<(const fill_path &data) {
    stream_input(data);
    return *this;
  }
  SurfaceArea &operator<<(const line_cap &data) {
    stream_input(data);
    return *this;
  }
  SurfaceArea &operator<<(const line_join &data) {
    stream_input(data);
    return *this;
  }
  SurfaceArea &operator<<(const miter_limit &data) {
    stream_input(data);
    return *this;
  }
  SurfaceArea &operator<<(const line_dashes &data) {
    stream_input(data);
    return *this;
  }

  SurfaceArea &operator[](const std::string &s) {return *this;}
  SurfaceArea &operator[](const std::size_t &s) {return *this;}
  SurfaceArea &group(const std::string &sgroupname) {return *this;}
  template <typename T> T &get(void) { return *this; }

  bool processing(void) { return bProcessing; }
  void listen(eventType etype, const eventHandler &evtDispatcher);
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

  SurfaceArea &tollerance(double _t);
  SurfaceArea &op(op_t _op);

  SurfaceArea &close_path(void);
  SurfaceArea &arc(double xc, double yc, double radius, double angle1,
                   double angle2);
  SurfaceArea &negative_arc(double xc, double yc, double radius,
                                        double angle1, double angle2);
  SurfaceArea &curve(double x1, double y1, double x2, double y2, double x3,
                     double y3);
  SurfaceArea &line(double x, double y);
  SurfaceArea &hline(double x);
  SurfaceArea &vline(double y);
  SurfaceArea &move_to(double x, double y);
  SurfaceArea &rectangle(double x, double y, double width, double height);
  point location(void);

  SurfaceArea &mask(Paint &p);
  SurfaceArea &mask(Paint &p, double x, double y);
  SurfaceArea &paint(double alpha = 1.0);
  SurfaceArea &relative(void);
  SurfaceArea &absolute(void);


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
  bool bRelative=false;

  SurfaceArea &stream_input(const antialias &_val);
  SurfaceArea &stream_input(const std::string &_val);
  SurfaceArea &stream_input(const std::stringstream &_val);
  SurfaceArea &stream_input(const source &_val);
  SurfaceArea &stream_input(const text_outline &_val);
  SurfaceArea &stream_input(const text_fill &_val);
  SurfaceArea &stream_input(const text_shadow &_val);
  SurfaceArea &stream_input(const text_outline_none &_val);
  SurfaceArea &stream_input(const text_fill_none &_val);
  SurfaceArea &stream_input(const text_shadow_none &_val);
  SurfaceArea &stream_input(const text_alignment &_val);
  SurfaceArea &stream_input(const text_alignment::setting &_val);
  SurfaceArea &stream_input(const coordinates &_val);
  SurfaceArea &stream_input(const index_by &_val);
  SurfaceArea &stream_input(const line_width &_val);
  SurfaceArea &stream_input(const indent &_val);
  SurfaceArea &stream_input(const ellipse &_val);
  SurfaceArea &stream_input(const line_space &_val);
  SurfaceArea &stream_input(const tab_stops &_val);
  SurfaceArea &stream_input(const text_font &_val);
  SurfaceArea &stream_input(const stroke_path &_val);
  SurfaceArea &stream_input(const fill_path &_val);
  SurfaceArea &stream_input(const line_cap &_val);
  SurfaceArea &stream_input(const line_join &_val);
  SurfaceArea &stream_input(const miter_limit &_val);
  SurfaceArea &stream_input(const line_dashes &val);

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
