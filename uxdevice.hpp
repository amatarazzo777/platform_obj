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

#define PAINT_OBJ(X) \
using X = class X : public Paint { \
public: \
  X(u_int32_t c): Paint(u_int32_t c){} \
  X(double r, double g, double b): Paint(double r, double g, double b){}\
  X(double r, double g, double b, double a): Paint(double r, double g, double b, double a){}\
  X(const std::string &n): Paint(const std::string &n){}\
  X(const std::string &n, double width, double height): Paint(const std::string &n, double width, double height){}\
  X(double x0, double y0, double x1, double y1, const ColorStops &_cs): Paint(double x0, double y0, double x1, double y1, const ColorStops &_cs){}\
  X(double cx0, double cy0, double radius0, double cx1, double cy1,\
        double radius1, const ColorStops &cs)):\
         Paint(double cx0, double cy0, double radius0, double cx1, double cy1,\
        double radius1, const ColorStops &cs){}\
\
}
PAINT_OBJ(source);
PAINT_OBJ(text_outline);
PAINT_OBJ(text_fill);
PAINT_OBJ(text_shadow);

using text_alignment = enum class text_alignment {
  left = PangoAlignment::PANGO_ALIGN_LEFT,
  center = PangoAlignment::PANGO_ALIGN_CENTER,
  right = PangoAlignment::PANGO_ALIGN_RIGHT,
  justified = 4
};
using coordinates = class coordinates {
public:
  coordinates(double _x, double _y) : x(_x),y(_y) {}
  coordinates(double _x, double _y, double _w,double _h) : x(_x),y(_y),w(_w),h(_h) {}
  double x = 0, y = 0, w = 0, h = 0;

};
using index_by = class index_by {
public:
  index_by(std::string) {}
  index_by(std::size_t) {}
};
using line_width = class line_width {
public:
  line_width(double lw) {}
};


using indent = class indent {
public:
  indent(double space);
};

using ellipse = class ellipse {
public:
  ellipse(ellipsize_t e);
};

using line_space= class line_space {
public:
  line_space(double dSpace);
};

using tab_stops= class tab_stops {
public:
  tab_stops(const std::vector<double> &tabs);
};


using text_font = class text_font {
public:
  text_font(const std::string &s);
};



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

  template <typename T> CWindow& operator<<(const T &data) {
    std::ostringstream s;
    s << data;
    std::string sData = s.str();


    return *this;
  }
  CWindow& operator<<(const source &data) {
    return *this;
  }

  bool processing(void) {
    return bProcessing;
  }
  void listen(eventType etype, const eventHandler &evtDispatcher);
  CWindow &group(std::string &_name);

  CWindow &device_offset(double x, double y);
  CWindow &device_scale(double x, double y);
  CWindow &brush(Paint &b);
  void clear(void);
  void notify_complete(void);

  template<typename T, typename ...Args>
  CWindow &text(const T&val, const Args&... args) {
    text(val);
    text(args...);
    return *this;
  }

  CWindow &text(const std::string &_val);
  CWindow &text(const std::stringstream &_val);
  CWindow &text(const source &_val);
  CWindow &text(const text_outline &_val);
  CWindow &text(const text_fill &_val);
  CWindow &text(const text_shadow &_val);
  CWindow &text(const text_alignment &_val);
  CWindow &text(const coordinates &_val);
  CWindow &text(const index_by &_val);
  CWindow &text(const line_width &_val);

  CWindow &text(const indent &_val);

  CWindow &text(const ellipse &_val);
  CWindow &text(const line_space &_val);

  CWindow &text(const tab_stops &_val);

  CWindow &text(const text_font &_val);

  CWindow &image(const std::string &s);
  CWindow &antialias(alias_t _a);


  CWindow &save(void);
  CWindow &restore(void);

  CWindow &push(content_t _content = content_t::all);
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

  CWindow &cap(line_cap_t c);
  CWindow &join(line_join_t j);
  CWindow &line_width(double dWidth);
  CWindow &miter_limit(double dLimit);
  CWindow &dashes(const std::vector<double> &dashes, double offset);
  CWindow &tollerance(double _t);
  CWindow &op(op_t _op);

  CWindow &close_path(void);
  CWindow &arc(double xc, double yc, double radius, double angle1,
               double angle2);
  CWindow &arc_neg(double xc, double yc, double radius, double angle1,
                   double angle2);
  CWindow &curve(double x1, double y1, double x2, double y2, double x3,
                 double y3);
  CWindow &line(double x, double y);
  CWindow &hline(double x);
  CWindow &vline(double y);
  CWindow &move(double x, double y);
  CWindow &rectangle(double x, double y, double width, double height);
  point location(void);

  CWindow &mask(Paint &p);
  CWindow &mask(Paint &p, double x, double y);
  CWindow &paint(double alpha = 1.0);
  CWindow &relative(void);
  CWindow &absolute(void);

  void stroke(const Paint &p);
  void stroke(u_int32_t c);
  void stroke(const std::string &c);
  void stroke(const std::string &c, double w, double h);
  void stroke(double _r, double _g, double _b);
  void stroke(double _r, double _g, double _b, double _a);
  void stroke(double x0, double y0, double x1, double y1, const ColorStops &cs);
  void stroke(double cx0, double cy0, double radius0, double cx1, double cy1,
              double radius1, const ColorStops &cs);

  void fill(const Paint &p);
  void fill(u_int32_t c);
  void fill(const std::string &c);
  void fill(const std::string &c, double w, double h);
  void fill(double _r, double _g, double _b);
  void fill(double _r, double _g, double _b, double _a);
  void fill(double x0, double y0, double x1, double y1, const ColorStops &cs);
  void fill(double cx0, double cy0, double radius0, double cx1, double cy1,
            double radius1, const ColorStops &cs);
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
