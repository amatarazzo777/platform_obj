/*
 * This file is part of the PLATFORM_OBJ distribution
 *
 * (https://github.com/amatarazzo777/platform_obj)
 *
 * Copyright (c) 2020 Anthony Matarazzo.
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
\brief The file encapsulates the objects that may be created,
controlled, and rendered by the system. Attributes. externalized data such
as std::shared pointers may also be stored. Each object has a hash
function which is also queried to detect changes. Only on screen related
objects are included within the set. The intersection test is also included
as part of a base hierarchy of classes. display_unit_t is the base class.
*/

#pragma once

/**

\class coordinate_storage_t
\brief storage class used by the coordinate_t object.
\details


 */
namespace uxdevice {
class coordinate_storage_t {
public:
  double x, y, w, h;
  HASH_OBJECT_MEMBERS(HASH_TYPE_ID_THIS, x, y, w, h)
};
} // namespace uxdevice
STD_HASHABLE(uxdevice::coordinate_storage_t);

/**
\internal
\class arc_storage_t
\brief

\details


 */
namespace uxdevice {
struct arc_storage_t {
  double xc, yc, radius, angle1, angle2;
  HASH_OBJECT_MEMBERS(HASH_TYPE_ID_THIS, xc, yc, radius, angle1, angle2)
};
} // namespace uxdevice
STD_HASHABLE(uxdevice::arc_storage_t);

/**
\internal
\class negative_arc_storage_t
\brief

\details


 */
namespace uxdevice {
struct negative_arc_storage_t {
  double xc, yc, radius, angle1, angle2;
  HASH_OBJECT_MEMBERS(HASH_TYPE_ID_THIS, xc, yc, radius, angle1, angle2)
};
} // namespace uxdevice
STD_HASHABLE(uxdevice::negative_arc_storage_t);

/**
\internal
\class rectangle_storage_t
\brief

\details


 */
namespace uxdevice {
struct rectangle_storage_t {
  double x, y, width, height;
  HASH_OBJECT_MEMBERS(HASH_TYPE_ID_THIS, x, y, width, height)
};
} // namespace uxdevice
STD_HASHABLE(uxdevice::rectangle_storage_t);

/**
\internal
\class curve_storage_t
\brief

\details


 */
namespace uxdevice {
struct curve_storage_t {
  double x1, y1, x2, y2, x3, y3;
  HASH_OBJECT_MEMBERS(HASH_TYPE_ID_THIS, x1, y1, x2, y2, x3, y3)
};
} // namespace uxdevice
STD_HASHABLE(uxdevice::curve_storage_t);

/**
\internal
\class line_storage_t
\brief

\details


 */
namespace uxdevice {
struct line_storage_t {
  double x, y;
  HASH_OBJECT_MEMBERS(HASH_TYPE_ID_THIS, x, y)
};
} // namespace uxdevice
STD_HASHABLE(uxdevice::line_storage_t);

/**
\internal
\class stroke_fill_path_storage_t
\brief

\details


 */
namespace uxdevice {
struct stroke_fill_path_storage_t {
  painter_brush_t fill_brush;
  painter_brush_t stroke_brush;

  HASH_OBJECT_MEMBERS(HASH_TYPE_ID_THIS, fill_brush.hash_code(),
                      stroke_brush.hash_code())
};
} // namespace uxdevice
STD_HASHABLE(uxdevice::stroke_fill_path_storage_t);

/**
\internal
\class text_font_data_storage
\brief

\details


 */

namespace uxdevice {
class text_font_data_storage_t {
public:
  std::string description;
  PangoFontDescription *pango_font_ptr;

  // hash function
  std::size_t hash_code(void) const noexcept {
    std::size_t value = HASH_TYPE_ID_THIS;
    hash_combine(value, description);
    return value;
  }

  void invoke(display_context_t &context) {
    if (!pango_font_ptr) {
      pango_font_ptr = pango_font_description_from_string(description.data());
      if (!pango_font_ptr) {
        std::string s = "Font could not be loaded from description. ( ";
        s += description + ")";
        context.error_state(__func__, __LINE__, __FILE__, std::string_view(s));
      }
    }
  }

  /*********/
  void emit(PangoLayout *layout) {}
  /*********/

  ~text_font_data_storage_t() {
    if (pango_font_ptr)
      pango_font_description_free(pango_font_ptr);
  }
};
} // namespace uxdevice
STD_HASHABLE(uxdevice::text_font_data_storage_t);

/**

\typedef line_dash_storage_t
\brief storage alias for the line dashes array. needed for registering the
hashing function.

\details


 */
namespace uxdevice {
class line_dash_storage_t {
public:
  std::vector<double> value = {};
  double offset = {};

  HASH_OBJECT_MEMBERS(HASH_TYPE_ID_THIS, HASH_VECTOR_OBJECTS(value), offset)
};
} // namespace uxdevice

STD_HASHABLE(uxdevice::line_dash_storage_t);

/**

\class image_block_storage_t
\brief storage class used by the image_block_t object. The oject is
responsible for encapsulating and dynamically allocating, and releasing
memory.

\details


 */
namespace uxdevice {
class image_block_storage_t {
public:
  /// @brief default constructor
  image_block_storage_t() {}

  /// @brief move assignment
  image_block_storage_t &operator=(image_block_storage_t &&other) noexcept {
    image_block_ptr = std::move(other.image_block_ptr);
    is_SVG = other.is_SVG;
    is_loaded = other.is_loaded;
    coordinates = std::move(other.coordinates);
    return *this;
  }

  /// @brief copy assignment operator
  image_block_storage_t &operator=(const image_block_storage_t &other) {
    image_block_ptr = cairo_surface_reference(other.image_block_ptr);
    is_SVG = other.is_SVG;
    is_loaded = other.is_loaded;
    coordinates = other.coordinates;
    return *this;
  }

  /// @brief move constructor
  image_block_storage_t(image_block_storage_t &&other) noexcept
      : image_block_ptr(std::move(other.image_block_ptr)),
        is_SVG(std::move(other.is_SVG)), is_loaded(std::move(other.is_loaded)),
        coordinates(std::move(other.coordinates)) {}

  /// @brief copy constructor
  image_block_storage_t(const image_block_storage_t &other)
      : image_block_ptr(cairo_surface_reference(other.image_block_ptr)),
        is_SVG(other.is_SVG), is_loaded(other.is_loaded),
        coordinates(other.coordinates) {}

  virtual ~image_block_storage_t() {
    if (image_block_ptr)
      cairo_surface_destroy(image_block_ptr);
  }

  DECLARE_HASH_MEMBERS_INTERFACE

  cairo_surface_t *image_block_ptr = {};
  bool is_SVG = {};
  bool is_loaded = {};
  std::shared_ptr<coordinates_t> coordinates = {};
};
} // namespace uxdevice
STD_HASHABLE(uxdevice::image_block_storage_t);

/**

\class textual_render_storage_t
\brief class used to store parameters and options for a textual render. The
object is created as the side effect of inserting text, char *, std string or
a std::shared_ptr<std::string>.
*/
namespace uxdevice {
class textual_render_storage_t {
public:
  typedef std::function<void(cairo_t *cr, coordinates_t &a)>
      internal_cairo_function_t;

  bool set_layout_options(cairo_t *cr);
  void create_shadow(void);
  internal_cairo_function_t precise_rendering_function(void);
  void invoke(display_context_t &context);

  DECLARE_TYPE_INDEX_MEMORY(rendering_parameter)
  DECLARE_HASH_MEMBERS_INTERFACE

  cairo_surface_t *shadow_image = nullptr;
  cairo_t *shadow_cr = nullptr;
  PangoLayout *layout = nullptr;
  PangoRectangle ink_rect = PangoRectangle();
  PangoRectangle logical_rect = PangoRectangle();
  Matrix matrix = {};
};
} // namespace uxdevice
STD_HASHABLE(uxdevice::textual_render_storage_t);

/**

\class listener_storage_t
\brief class used to store members of a listener event. This is the base class
storage. Listeners inherit from listener_t  as a base.
*/
namespace uxdevice {
class listener_storage_t {
public:
  std::type_index type = std::type_index(typeid(this));
  event_handler_t dispatch_event = {};

  HASH_OBJECT_MEMBERS(HASH_TYPE_ID_THIS, type, dispatch_event ? 1 : 0)
};
} // namespace uxdevice
STD_HASHABLE(uxdevice::listener_storage_t);

// the unit_memory stores items by type id. A boolean with
// an aliased type can be stored within the list, however it will not
// be a display unit.
DECLARE_PAINTER_BRUSH_DISPLAY_UNIT(surface_area_brush_t)
DECLARE_STORAGE_EMITTER_DISPLAY_UNIT(surface_area_title_t, std::string)

// text
DECLARE_MARKER_DISPLAY_UNIT(text_render_fast_t)
DECLARE_MARKER_DISPLAY_UNIT(text_render_path_t)

DECLARE_STORAGE_EMITTER_DISPLAY_UNIT(text_font_t, text_font_data_storage_t)

DECLARE_PAINTER_BRUSH_DISPLAY_UNIT(text_color_t)
DECLARE_PAINTER_BRUSH_DISPLAY_UNIT(text_outline_t)
DECLARE_PAINTER_BRUSH_DISPLAY_UNIT(text_fill_t)
DECLARE_PAINTER_BRUSH_DISPLAY_UNIT(text_shadow_t)

DECLARE_STORAGE_EMITTER_DISPLAY_UNIT(text_alignment_t, text_alignment_options_t)
DECLARE_STORAGE_EMITTER_DISPLAY_UNIT(text_indent_t, double)
DECLARE_STORAGE_EMITTER_DISPLAY_UNIT(text_ellipsize_t, text_ellipsize_options_t)
DECLARE_STORAGE_EMITTER_DISPLAY_UNIT(text_line_space_t, double)
DECLARE_STORAGE_EMITTER_DISPLAY_UNIT(text_tab_stops_t, std::vector<double>)
DECLARE_STORAGE_EMITTER_DISPLAY_UNIT(text_data_t, std::string)

// image drawing and other block operations

DECLARE_STORAGE_EMITTER_DISPLAY_UNIT(coordinates_t, coordinate_storage_t)

DECLARE_STORAGE_EMITTER_DISPLAY_UNIT(antialias_t, antialias_options_t)
DECLARE_STORAGE_EMITTER_DISPLAY_UNIT(line_width_t, double)
DECLARE_STORAGE_EMITTER_DISPLAY_UNIT(line_cap_t, line_cap_options_t)
DECLARE_STORAGE_EMITTER_DISPLAY_UNIT(line_join_t, line_join_options_t)
DECLARE_STORAGE_EMITTER_DISPLAY_UNIT(miter_limit_t, double)
DECLARE_STORAGE_EMITTER_DISPLAY_UNIT(line_dashes_t, line_dash_storage_t)

DECLARE_STORAGE_EMITTER_DISPLAY_UNIT(tollerance_t, double)
DECLARE_STORAGE_EMITTER_DISPLAY_UNIT(graphic_operator_t,
                                     graphic_operator_options_t)

DECLARE_MARKER_DISPLAY_UNIT(relative_coordinates_t)
DECLARE_MARKER_DISPLAY_UNIT(absolute_coordinates_t)

DECLARE_STORAGE_EMITTER_DISPLAY_UNIT(function_object_t, cairo_function_t)
DECLARE_STORAGE_EMITTER_DISPLAY_UNIT(option_function_object_t, cairo_function_t)

DECLARE_STORAGE_EMITTER_DRAWING_FUNCTION(textual_render_t,
                                         textual_render_storage_t)

DECLARE_STORAGE_EMITTER_DRAWING_FUNCTION(image_block_t, image_block_storage_t)
DECLARE_STORAGE_EMITTER_DRAWING_FUNCTION(draw_function_object_t,
                                         cairo_function_t)
// primitives - drawing functions
DECLARE_STORAGE_EMITTER_DRAWING_FUNCTION(arc_t, arc_storage_t)
DECLARE_STORAGE_EMITTER_DRAWING_FUNCTION(negative_arc_t, negative_arc_storage_t)
DECLARE_STORAGE_EMITTER_DRAWING_FUNCTION(curve_t, curve_storage_t)
DECLARE_STORAGE_EMITTER_DRAWING_FUNCTION(line_t, line_storage_t)
DECLARE_STORAGE_EMITTER_DRAWING_FUNCTION(hline_t, double)
DECLARE_STORAGE_EMITTER_DRAWING_FUNCTION(vline_t, double)
DECLARE_STORAGE_EMITTER_DRAWING_FUNCTION(rectangle_t, rectangle_storage_t)

DECLARE_STORAGE_EMITTER_DRAWING_FUNCTION(stroke_path_t, painter_brush_t)
DECLARE_STORAGE_EMITTER_DRAWING_FUNCTION(fill_path_t, painter_brush_t)

DECLARE_STORAGE_EMITTER_DRAWING_FUNCTION(stroke_fill_path_t,
                                         stroke_fill_path_storage_t)
DECLARE_STORAGE_EMITTER_DRAWING_FUNCTION(mask_t, painter_brush_t)
DECLARE_STORAGE_EMITTER_DRAWING_FUNCTION(paint_t, double)
DECLARE_MARKER_DISPLAY_UNIT(close_path_t)

// event listeners
DECLARE_STORAGE_EMITTER_DISPLAY_UNIT(listener_t, listener_storage_t)

DECLARE_NAMED_LISTENER_DISPLAY_UNIT(listen_paint_t)
DECLARE_NAMED_LISTENER_DISPLAY_UNIT(listen_focus_t)
DECLARE_NAMED_LISTENER_DISPLAY_UNIT(listen_blur_t)
DECLARE_NAMED_LISTENER_DISPLAY_UNIT(listen_resize_t)
DECLARE_NAMED_LISTENER_DISPLAY_UNIT(listen_keydown_t)
DECLARE_NAMED_LISTENER_DISPLAY_UNIT(listen_keyup_t)
DECLARE_NAMED_LISTENER_DISPLAY_UNIT(listen_keypress_t)
DECLARE_NAMED_LISTENER_DISPLAY_UNIT(listen_mouseenter_t)
DECLARE_NAMED_LISTENER_DISPLAY_UNIT(listen_mousemove_t)
DECLARE_NAMED_LISTENER_DISPLAY_UNIT(listen_mousedown_t)
DECLARE_NAMED_LISTENER_DISPLAY_UNIT(listen_mouseup_t)
DECLARE_NAMED_LISTENER_DISPLAY_UNIT(listen_click_t)
DECLARE_NAMED_LISTENER_DISPLAY_UNIT(listen_dblclick_t)
DECLARE_NAMED_LISTENER_DISPLAY_UNIT(listen_contextmenu_t)
DECLARE_NAMED_LISTENER_DISPLAY_UNIT(listen_wheel_t)
DECLARE_NAMED_LISTENER_DISPLAY_UNIT(listen_mouseleave_t)
