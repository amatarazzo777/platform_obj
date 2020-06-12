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
using text_outline = class text_outline : public Paint;
using text_fill = class text_fill : public Paint;
using text_shadow = class text_shadow : public Paint;
using text_alignment = class enum text_alignment = {
  left = PangoAlignment::PANGO_ALIGN_LEFT,
  center = PangoAlignment::PANGO_ALIGN_CENTER,
  right = PangoAlignment::PANGO_ALIGN_RIGHT,
  justified = 4
};
using stroke = class stroke : public Paint;


using indent = class indent {
  indent(double space);
};

using ellipse = class ellipse {
  ellipse(ellipsize_t e);
};

using line_space= class line_space {
  line_space(double dSpace);
};

using tab_stops= class tab_stops {
  tab_stops(const std::vector<double> &tabs);
}


using text_font = class text_font {
  text_font(const std::string &s);
}



class CWindow {
public:
  CWindow();
  CWindow(const std::string &sCWindowTitle);

  CWindow(const eventHandler &evtDispatcher);
  CWindow(const CoordinateList &coordinates);

  CWindow(const CoordinateList &coordinates, const std::string &sCWindowTitle);
  CWindow(const CoordinateList &coordinates, const std::string &sCWindowTitle,
          const Paint &background);

  CWindow(const CoordinateList &coordinates, const std::string &sCWindowTitle,
          const eventHandler &evtDispatcher, const Paint &background);

  CWindow operator<<() { std::ostringstream out = {}; };
  bool processing(void) { return bProcessing; }
  void listen(eventType etype, const eventHandler &evtDispatcher);
  CWindow &group(std::string &_name);

  CWindow &device_offset(double x, double y);
  CWindow &device_scale(double x, double y);
  CWindow &brush(Paint &b);
  void clear(void);
  void notify_complete(void);

  CWindow &text(const std::string &s);
  CWindow &text(const std::stringstream &s);
  CWindow &image(const std::string &s);
CWindow &antialias(alias_t _a);


  CWindow &save(void);
  CWindow &restore(void);

  CWindow &push(content _content = content::all);
  CWindow &pop(bool bToSource = false);

  CWindow &translate(double x, double y);
  CWindow &rotate(double angle);
  CWindow &scale(double x, double y);
  CWindow &transform(const Matrix &mat);
  CWindow &matrix(const Matrix &mat);
  CWindow &identity(void);
  CWindow &device(double &x, double &y);
  CWindow &device_distance(double &x, double &y);
  CWindow &user(double &x, double &y);
  CWindow &user_distance(double &x, double &y);

  CWindow &source(Paint &p);

  CWindow &cap(lineCap c);
  CWindow &join(lineJoin j);
  CWindow &line_width(double dWidth);
  CWindow &miter_limit(double dLimit);
  CWindow &dashes(const std::vector<double> &dashes, double offset);
  CWindow &tollerance(double _t);
  CWindow &op(op_t _op);

  CWindow &arc(double xc, double yc, double radius, double angle1,
               double angle2);
  CWindow &arc_neg(double xc, double yc, double radius, double angle1,
                   double angle2);
  CWindow &curve(double x1, double y1, double x2, double y2, double x3,
                 double y3);
  CWindow &close_path(void);
  CWindow &line(double x, double);
  CWindow &move(double x, double y);
  CWindow &rectangle(double x, double y, double width, double height);
  point location(void);

  CWindow &mask(Paint &p);
  CWindow &mask(Paint &p, double x, double y);
  CWindow &paint(double alpha = 1.0);

  void stroke(const Paint &p, bool bPreserve = false);
  void stroke(u_int32_t c, bool bPreserve = false);
  void stroke(const std::string &c, bool bPreserve = false);
  void stroke(const std::string &c, double w, double h, bool bPreserve = false);
  void stroke(double _r, double _g, double _b, bool bPreserve = false);
  void stroke(double _r, double _g, double _b, double _a,
              bool bPreserve = false);
  void stroke(double x0, double y0, double x1, double y1, const ColorStops &cs,
              bool bPreserve = false);
  void stroke(double cx0, double cy0, double radius0, double cx1, double cy1,
              double radius1, const ColorStops &cs, bool bPreserve = false);

  void fill(const Paint &p, bool bPreserve = false);
  void fill(u_int32_t c, bool bPreserve = false);
  void fill(const std::string &c, bool bPreserve = false);
  void fill(const std::string &c, double w, double h, bool bPreserve = false);
  void fill(double _r, double _g, double _b, bool bPreserve = false);
  void fill(double _r, double _g, double _b, double _a, bool bPreserve = false);
  void fill(double x0, double y0, double x1, double y1, const ColorStops &cs,
            bool bPreserve = false);
  void fill(double cx0, double cy0, double radius0, double cx1, double cy1,
            double radius1, const ColorStops &cs, bool bPreserve = false);
  bounds stroke(void);
  bool in_stroke(double x, double y);
  bool in_fill(double x, double y);

  bounds clip(void);
  void clip(bool bPreserve = false);
  bool in_clip(double x, double y);

private:
  void start_processing(void);
  void draw_Caret(const int x, const int y, const int h);
  void message_loop(void);
  void render_loop(void);
  void dispatch_event(const event &e);

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

  std::vector<eventHandler> &get_event_vector(eventType evtType);
};

} // namespace uxdevice
