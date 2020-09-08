/*
 * This file is part of the PLATFORM_OBJ distribution
 * {https://github.com/amatarazzo777/platform_obj). Copyright (c) 2020 Anthony
 * Matarazzo.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, version 3.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

/**
\author Anthony Matarazzo
\file uxdisplayunits.hpp
\date 9/7/20
\version 1.0
\brief
*/
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
options when compiling the text_color_t.
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
#include "uxdisplayunitbase.hpp"
#include "uxdisplayunits.hpp"

std::string _errorReport(std::string text_color_tFile, int ln,
                         std::string sfunc, std::string cond,
                         std::string ecode);
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
typedef std::list<short int> coordinate_list_t;

class surface_area_t {
public:
  surface_area_t();
  surface_area_t(const std::string &surface_area_title);

  surface_area_t(const event_handler_t &evtDispatcher);
  surface_area_t(const coordinate_list_t &coordinates);

  surface_area_t(const coordinate_list_t &coordinates,
                 const std::string &surface_area_title);
  surface_area_t(const coordinate_list_t &coordinates,
                 const std::string &surface_area_title,
                 const painter_brush_t &background);

  surface_area_t(const coordinate_list_t &coordinates,
                 const std::string &surface_area_title,
                 const event_handler_t &evtDispatcher,
                 const painter_brush_t &background);
  ~surface_area_t();

  template <typename T> surface_area_t &operator<<(const T &data) {
    std::ostringstream s;
    s << data;
    std::string sData = s.str();
    stream_input(sData);
    return *this;
  }

  /*
   Declare interface only.  uxdevice.cpp contains implementation.
   These are the stream interface with a function prototype for the invoke().
   The uxdevice.cpp file contains the implementation.

   surface_area_t &uxdevice::surface_area_t::stream_input(
    const CLASS_NAME _val)

  */
  DECLARE_STREAM_INTERFACE(std::string)
  DECLARE_STREAM_INTERFACE(std::stringstream)
  DECLARE_STREAM_INTERFACE(char *)

  /* declares the interface and implementation for these
   objects
   when these are invoked, the current_units class is also updated.
   When rendering objects are created, text, image or other, these
   these shared pointers are used as a reference local member initialized
   at invoke() public member. The parameters and options are validated as well.
    */
  DECLARE_STREAM_IMPLEMENTATION(coordinates_t)

  DECLARE_STREAM_IMPLEMENTATION(text_font_t)
  DECLARE_STREAM_IMPLEMENTATION(text_color_t)
  DECLARE_STREAM_IMPLEMENTATION(text_fill_t)
  DECLARE_STREAM_IMPLEMENTATION(text_outline_t)
  DECLARE_STREAM_IMPLEMENTATION(text_shadow_t)
  DECLARE_STREAM_IMPLEMENTATION(text_fill_off_t)
  DECLARE_STREAM_IMPLEMENTATION(text_outline_off_t)
  DECLARE_STREAM_IMPLEMENTATION(text_shadow_off_t)
  DECLARE_STREAM_IMPLEMENTATION(text_alignment_t)
  DECLARE_STREAM_IMPLEMENTATION(text_alignment_options_t)

  DECLARE_STREAM_IMPLEMENTATION(text_indent_t)
  DECLARE_STREAM_IMPLEMENTATION(text_ellipsize_t)
  DECLARE_STREAM_IMPLEMENTATION(text_line_space_t)
  DECLARE_STREAM_IMPLEMENTATION(text_tab_stops_t)

  /* these are recorded within the current units structure as well.
   These options persist within the display context and stick.
   They relate to drawing operations and their options on rendering.
    */

  DECLARE_STREAM_IMPLEMENTATION(antialias_t)
  DECLARE_STREAM_IMPLEMENTATION(graphic_operator_t)

  DECLARE_STREAM_IMPLEMENTATION(line_width_t)
  DECLARE_STREAM_IMPLEMENTATION(line_cap_t)
  DECLARE_STREAM_IMPLEMENTATION(line_join_t)
  DECLARE_STREAM_IMPLEMENTATION(line_dashes_t)

  DECLARE_STREAM_IMPLEMENTATION(miter_limit_t)
  DECLARE_STREAM_IMPLEMENTATION(tollerance_t)
  DECLARE_STREAM_IMPLEMENTATION(absolute_coordinates_t)
  DECLARE_STREAM_IMPLEMENTATION(relative_coordinates_t)

  DECLARE_STREAM_IMPLEMENTATION(image_block_t)
  DECLARE_STREAM_IMPLEMENTATION(stroke_path_t)
  DECLARE_STREAM_IMPLEMENTATION(fill_path_t)
  DECLARE_STREAM_IMPLEMENTATION(stroke_fill_path_t)
  DECLARE_STREAM_IMPLEMENTATION(close_path_t)

  DECLARE_STREAM_IMPLEMENTATION(arc_t)
  DECLARE_STREAM_IMPLEMENTATION(negative_arc_t)
  DECLARE_STREAM_IMPLEMENTATION(curve_t)
  DECLARE_STREAM_IMPLEMENTATION(line_t)
  DECLARE_STREAM_IMPLEMENTATION(hline_t)
  DECLARE_STREAM_IMPLEMENTATION(vline_t)
  DECLARE_STREAM_IMPLEMENTATION(move_to_t)
  DECLARE_STREAM_IMPLEMENTATION(rectangle_t)

  DECLARE_STREAM_IMPLEMENTATION(listener_t)
  DECLARE_STREAM_IMPLEMENTATION(listen_paint_t)
  DECLARE_STREAM_IMPLEMENTATION(listen_focus_t)
  DECLARE_STREAM_IMPLEMENTATION(listen_blur_t)
  DECLARE_STREAM_IMPLEMENTATION(listen_resize_t)
  DECLARE_STREAM_IMPLEMENTATION(listen_keydown_t)
  DECLARE_STREAM_IMPLEMENTATION(listen_keyup_t)
  DECLARE_STREAM_IMPLEMENTATION(listen_keypress_t)
  DECLARE_STREAM_IMPLEMENTATION(listen_mouseenter_t)
  DECLARE_STREAM_IMPLEMENTATION(listen_mousemove_t)
  DECLARE_STREAM_IMPLEMENTATION(listen_mousedown_t)
  DECLARE_STREAM_IMPLEMENTATION(listen_mouseup_t)
  DECLARE_STREAM_IMPLEMENTATION(listen_click_t)
  DECLARE_STREAM_IMPLEMENTATION(listen_dblclick_t)
  DECLARE_STREAM_IMPLEMENTATION(listen_contextmenu_t)
  DECLARE_STREAM_IMPLEMENTATION(listen_wheel_t)
  DECLARE_STREAM_IMPLEMENTATION(listen_mouseleave_t)

public:
  display_unit_t &operator[](const indirect_index_display_unit_t &idx) {
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
  surface_area_t &group(std::string &_name);

  surface_area_t &device_offset(double x, double y);
  surface_area_t &device_scale(double x, double y);
  surface_area_t &surface_brush(painter_brush_t &b);
  void clear(void);
  void notify_complete(void);

  surface_area_t &save(void);
  surface_area_t &restore(void);

  surface_area_t &push(content_options_t _content = content_options_t::all);
  surface_area_t &pop(bool bToSource = false);

  surface_area_t &translate(double x, double y);
  surface_area_t &rotate(double angle);
  surface_area_t &scale(double x, double y);
  surface_area_t &transform(const Matrix &mat);
  surface_area_t &matrix(const Matrix &mat);
  surface_area_t &identity(void);
  surface_area_t &device(double &x, double &y);
  surface_area_t &device_distance(double &x, double &y);
  surface_area_t &user(double &x, double &y);
  surface_area_t &user_distance(double &x, double &y);
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
  bool relative_coordinates_t = false;
  void maintain_index(std::shared_ptr<display_unit_t> obj);

private:
  display_context_t context = display_context_t();
  std::atomic<bool> bProcessing = false;
  errorHandler fnError = nullptr;
  event_handler_t fnEvents = nullptr;

  typedef std::list<std::shared_ptr<display_unit_t>> display_unit_list_t;
  display_unit_list_t DL = {};
  display_unit_list_t::iterator itDL_Processed = DL.begin();

  std::unordered_map<indirect_index_display_unit_t,
                     std::shared_ptr<display_unit_t>>
      mapped_objects = {};

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
