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
controlled, and rendered by the system. The system provides for std::shared
pointers to these object to be shared within the rendering cycle. Each object
has a hash function which is queried to detect changes within the underlying
data. Only on screen related objects are included within the set. The
intersection test is included as part of a base hierarchy of classes for objects
that are declared as such. The class objects are separated into two distinct
entity names. One that is the API name and a data storage class.


painter_brush_emitter_t
marker_emitter_t
storage_emitter_t
class_storage_emitter_t
storage_drawing_function_t
class_storage_drawing_function_t


*/

#pragma once

/**

\class coordinate_storage_t
\brief storage class used by the coordinate_t object.

\details The constructor interface is inherited by objects
that are display units.


 */
namespace uxdevice {
class coordinate_storage_t : virtual public hash_members_t {
public:
  coordinate_storage_t() {}
  coordinate_storage_t(double _x, double _y, double _w, double _h)
      : x(_x), y(_y), w(_w), h(_h) {}
  coordinate_storage_t(double _x, double _y) : x(_x), y(_y) {}
  virtual ~coordinate_storage_t() {}

  std::size_t hash_code(void) const noexcept {
    std::size_t __value = {};
    hash_combine(__value, std::type_index(typeid(coordinate_storage_t)), x, y,
                 w, h);
    return __value;
  }

  double x = {};
  double y = {};
  double w = {};
  double h = {};
};
} // namespace uxdevice
UX_REGISTER_STD_HASH_SPECIALIZATION(uxdevice::coordinate_storage_t);

/**
\internal
\class arc_storage_t
\brief

\details


 */
namespace uxdevice {
class arc_storage_t : virtual public hash_members_t {
public:
  arc_storage_t() {}
  arc_storage_t(double _xc, double _yc, double _radius, double _angle1,
                double _angle2)
      : xc(_xc), yc(_yc), radius(_radius), angle1(_angle1), angle2(_angle2) {}
  virtual ~arc_storage_t() {}

  std::size_t hash_code(void) const noexcept {
    std::size_t __value = {};
    hash_combine(__value, std::type_index(typeid(arc_storage_t)), xc, yc,
                 radius, angle1, angle2);
    return __value;
  }

  double xc = {};
  double yc = {};
  double radius = {};
  double angle1 = {};
  double angle2 = {};
};
} // namespace uxdevice
UX_REGISTER_STD_HASH_SPECIALIZATION(uxdevice::arc_storage_t);

/**
\internal
\class negative_arc_storage_t
\brief

\details


 */
namespace uxdevice {
class negative_arc_storage_t : virtual public hash_members_t {
public:
  negative_arc_storage_t() {}
  negative_arc_storage_t(double _xc, double _yc, double _radius, double _angle1,
                         double _angle2)
      : xc(_xc), yc(_yc), radius(_radius), angle1(_angle1), angle2(_angle2) {}

  virtual ~negative_arc_storage_t() {}

  std::size_t hash_code(void) const noexcept {
    std::size_t __value = {};
    hash_combine(__value, std::type_index(typeid(negative_arc_storage_t)), xc,
                 yc, radius, angle1, angle2);
    return __value;
  }

  double xc = {};
  double yc = {};
  double radius = {};
  double angle1 = {};
  double angle2 = {};
};
} // namespace uxdevice
UX_REGISTER_STD_HASH_SPECIALIZATION(uxdevice::negative_arc_storage_t);

/**
\internal
\class rectangle_storage_t
\brief

\details


 */
namespace uxdevice {
class rectangle_storage_t : virtual public hash_members_t {
public:
  rectangle_storage_t() {}
  rectangle_storage_t(double _x, double _y, double _width, double _height)
      : x(_x), y(_y), width(_width), height(_height) {}
  virtual ~rectangle_storage_t() {}

  std::size_t hash_code(void) const noexcept {
    std::size_t __value = {};
    hash_combine(__value, std::type_index(typeid(rectangle_storage_t)), x, y,
                 width, height);
    return __value;
  }

  double x = {};
  double y = {};
  double width = {};
  double height = {};
};
} // namespace uxdevice
UX_REGISTER_STD_HASH_SPECIALIZATION(uxdevice::rectangle_storage_t);

/**
\internal
\class curve_storage_t
\brief

\details


 */
namespace uxdevice {
class curve_storage_t : virtual public hash_members_t {
public:
  curve_storage_t() {}
  curve_storage_t(double _x1, double _y1, double _x2, double _y2, double _x3,
                  double _y3)
      : x1(_x1), y1(_y1), x2(_x2), y2(_y2), x3(_x3), y3(_y3) {}
  virtual ~curve_storage_t() {}

  std::size_t hash_code(void) const noexcept {
    std::size_t __value = {};
    hash_combine(__value, std::type_index(typeid(curve_storage_t)), x1, y1, x2,
                 y2, x3, y3);
    return __value;
  }

  double x1 = {};
  double y1 = {};
  double x2 = {};
  double y2 = {};
  double x3 = {};
  double y3 = {};
};
} // namespace uxdevice
UX_REGISTER_STD_HASH_SPECIALIZATION(uxdevice::curve_storage_t);

/**
\internal
\class line_storage_t
\brief

\details


 */
namespace uxdevice {
class line_storage_t : virtual public hash_members_t {
public:
  line_storage_t() {}
  line_storage_t(double _x, double _y) : x(_x), y(_y) {}
  virtual ~line_storage_t() {}

  std::size_t hash_code(void) const noexcept {
    std::size_t __value = {};
    hash_combine(__value, std::type_index(typeid(line_storage_t)), x, y);
    return __value;
  }

  double x = {};
  double y = {};
};
} // namespace uxdevice
UX_REGISTER_STD_HASH_SPECIALIZATION(uxdevice::line_storage_t);

/**
\internal
\class stroke_fill_path_storage_t
\brief

\details


 */
namespace uxdevice {
class stroke_fill_path_storage_t : virtual public hash_members_t {
public:
  painter_brush_t fill_brush = {};
  painter_brush_t stroke_brush = {};

  stroke_fill_path_storage_t() {}

  stroke_fill_path_storage_t(const painter_brush_t &f, const painter_brush_t &s)
      : fill_brush(f), stroke_brush(s) {}
  virtual ~stroke_fill_path_storage_t() {}

  std::size_t hash_code(void) const noexcept {
    std::size_t __value = {};
    hash_combine(__value, std::type_index(typeid(stroke_fill_path_storage_t)),
                 fill_brush.hash_code(), stroke_brush.hash_code());
    return __value;
  }
};
} // namespace uxdevice
UX_REGISTER_STD_HASH_SPECIALIZATION(uxdevice::stroke_fill_path_storage_t);

/**

\typedef line_dash_storage_t
\brief storage alias for the line dashes array. needed for registering the
hashing function.

\details


 */
namespace uxdevice {
class line_dash_storage_t : virtual public hash_members_t {
public:
  line_dash_storage_t() {}
  line_dash_storage_t(const std::vector<double> &_value, const double &_offset)
      : value(_value), offset(_offset) {}

  virtual ~line_dash_storage_t() {}

  std::size_t hash_code(void) const noexcept {
    std::size_t __value = {};
    hash_combine(__value, std::type_index(typeid(line_dash_storage_t)), offset);
    for (auto n : value)
      hash_combine(__value, n);
    hash_combine(__value, value.size());

    return __value;
  }

  std::vector<double> value = {};
  double offset = {};
};
} // namespace uxdevice

UX_REGISTER_STD_HASH_SPECIALIZATION(uxdevice::line_dash_storage_t);

/**

\class image_block_storage_t
\brief storage class used by the image_block_t object. The oject is
responsible for encapsulating and dynamically allocating, and releasing
memory.

\details


 */
namespace uxdevice {
class image_block_storage_t : virtual public hash_members_t {
public:
  /// @brief default constructor
  image_block_storage_t()
      : description{}, image_block_ptr{}, is_SVG{}, is_loaded{}, coordinate{} {}

  image_block_storage_t(const std::string &_description)
      : description(_description) {}

  /// @brief move assignment
  image_block_storage_t &operator=(image_block_storage_t &&other) noexcept {
    description = std::move(other.description);
    image_block_ptr = std::move(other.image_block_ptr);
    is_SVG = other.is_SVG;
    is_loaded = other.is_loaded;
    coordinate = std::move(other.coordinate);
    return *this;
  }

  /// @brief copy assignment operator
  image_block_storage_t &operator=(const image_block_storage_t &other) {
    description = other.description;
    image_block_ptr = cairo_surface_reference(other.image_block_ptr);
    is_SVG = other.is_SVG;
    is_loaded = other.is_loaded;
    coordinate = other.coordinate;
    return *this;
  }

  /// @brief move constructor
  image_block_storage_t(image_block_storage_t &&other) noexcept
      : description(std::move(other.description)),
        image_block_ptr(std::move(other.image_block_ptr)),
        is_SVG(std::move(other.is_SVG)), is_loaded(std::move(other.is_loaded)),
        coordinate(std::move(other.coordinate)) {}

  /// @brief copy constructor
  image_block_storage_t(const image_block_storage_t &other)
      : description(other.description),
        image_block_ptr(cairo_surface_reference(other.image_block_ptr)),
        is_SVG(other.is_SVG), is_loaded(other.is_loaded),
        coordinate(other.coordinate) {}

  virtual ~image_block_storage_t() {
    if (image_block_ptr)
      cairo_surface_destroy(image_block_ptr);
  }

  bool is_valid(void) { return image_block_ptr != nullptr; }

  std::size_t hash_code(void) const noexcept {
    std::size_t __value = {};
    hash_combine(__value, std::type_index(typeid(image_block_storage_t)),
                 description, is_SVG, is_loaded, coordinate);

    return __value;
  }

  std::string description = {};
  cairo_surface_t *image_block_ptr = {};
  bool is_SVG = {};
  bool is_loaded = {};
  std::shared_ptr<coordinate_t> coordinate = {};
};
} // namespace uxdevice
UX_REGISTER_STD_HASH_SPECIALIZATION(uxdevice::image_block_storage_t);

/**
\internal
\class text_font_data_storage
\brief storage for fonts.

\details


 */

namespace uxdevice {
class text_font_storage_t : virtual public hash_members_t {
public:
  // these become public members of the base class.
  text_font_storage_t() : description{}, font_ptr(nullptr) {}
  text_font_storage_t(const std::string &_description)
      : description(_description), font_ptr(nullptr) {}

  /// @brief move constructor
  text_font_storage_t(text_font_storage_t &&other) noexcept
      : description(other.description), font_ptr(other.font_ptr) {}

  /// @brief copy constructor
  text_font_storage_t(const text_font_storage_t &other)
      : description(other.description), font_ptr(nullptr) {}

  virtual ~text_font_storage_t() {
    if (font_ptr)
      pango_font_description_free(font_ptr);
  }

  text_font_storage_t &operator=(const text_font_storage_t &&other) noexcept {
    description = other.description;
    return *this;
  }
  text_font_storage_t &operator=(const text_font_storage_t &other) {
    description = other.description;
    return *this;
  }

  text_font_storage_t &operator=(const std::string &_desc) {
    description = _desc;
    return *this;
  }

  /// @brief move assignment
  text_font_storage_t &operator=(const std::string &&_desc) noexcept {
    description = _desc;
    return *this;
  }

  std::size_t hash_code(void) const noexcept {
    std::size_t __value = {};
    hash_combine(__value, std::type_index(typeid(text_font_storage_t)),
                 description);

    return __value;
  }

  std::string description = {};
  PangoFontDescription *font_ptr = {};
};
} // namespace uxdevice
UX_REGISTER_STD_HASH_SPECIALIZATION(uxdevice::text_font_storage_t);

/**

\class textual_render_storage_t
\brief class used to store parameters and options for a textual render. The
object is created as the side effect of inserting text, char *, std string or
a std::shared_ptr<std::string>. The clas inherits from drawing_output_t
which notes the ink rectangle of the object. The drawing_output_class_t class
also exposes intersection method for the render system to decide which function
to invoke, a standard draw or a clipped draw.
*/
namespace uxdevice {
class textual_render_storage_t : virtual public drawing_output_t,
                                 virtual public hash_members_t,
                                 public unit_memory_storage_t {
public:
  typedef std::function<void(cairo_t *cr, const coordinate_t &a)>
      internal_cairo_function_t;
  textual_render_storage_t() {}

  virtual ~textual_render_storage_t() {
    if (shadow_image)
      cairo_surface_destroy(shadow_image);

    if (shadow_cr)
      cairo_destroy(shadow_cr);

    if (layout)
      g_object_unref(layout);
  }

  /// @brief move constructor
  textual_render_storage_t(textual_render_storage_t &&other) noexcept
      : shadow_image(other.shadow_image), shadow_cr(other.shadow_cr),
        layout(other.layout), ink_rect(other.ink_rect),
        logical_rect(other.logical_rect), matrix(other.matrix) {}

  /// @brief copy constructor
  textual_render_storage_t(const textual_render_storage_t &other)
      : shadow_image(cairo_surface_reference(other.shadow_image)),
        shadow_cr(cairo_reference(other.shadow_cr)), ink_rect(other.ink_rect),
        logical_rect(other.logical_rect), matrix(other.matrix) {
    if (other.layout)
      layout = pango_layout_copy(other.layout);
  }

  textual_render_storage_t &operator=(const textual_render_storage_t &other) {
    shadow_image = cairo_surface_reference(other.shadow_image);
    shadow_cr = cairo_reference(other.shadow_cr);
    if (other.layout)
      layout = pango_layout_copy(other.layout);
    ink_rect = other.ink_rect;
    logical_rect = other.logical_rect;
    matrix = other.matrix;
    return *this;
  }

  textual_render_storage_t &
  operator=(const textual_render_storage_t &&other) noexcept {
    shadow_image = other.shadow_image;
    shadow_cr = other.shadow_cr;
    layout = other.layout;
    ink_rect = other.ink_rect;
    logical_rect = other.logical_rect;
    matrix = other.matrix;
    return *this;
  }

  std::size_t hash_code(void) const noexcept;

  cairo_surface_t *shadow_image = nullptr;
  cairo_t *shadow_cr = nullptr;
  PangoLayout *layout = nullptr;
  PangoRectangle ink_rect = PangoRectangle();
  PangoRectangle logical_rect = PangoRectangle();
  matrix_t matrix = {};

  bool set_layout_options(cairo_t *cr);
  void create_shadow(void);
  internal_cairo_function_t precise_rendering_function(void);
};
} // namespace uxdevice
UX_REGISTER_STD_HASH_SPECIALIZATION(uxdevice::textual_render_storage_t);

/**

\class text_tab_stops_storage_t
\brief sets the tab stops on the layout
*/

namespace uxdevice {
class text_tab_stops_storage_t : virtual public hash_members_t {
public:
  text_tab_stops_storage_t() {}
  text_tab_stops_storage_t(const std::vector<double> &_value) : value(_value) {}
  virtual ~text_tab_stops_storage_t() {}

  std::size_t hash_code(void) const noexcept {
    std::size_t __value = {};
    hash_combine(__value, std::type_index(typeid(text_tab_stops_storage_t)));
    for (auto n : value)
      hash_combine(__value, n);
    return __value;
  }

  std::vector<double> value = {};
};
} // namespace uxdevice
UX_REGISTER_STD_HASH_SPECIALIZATION(uxdevice::text_tab_stops_storage_t);

/********************************************************************************

                      API objects

*********************************************************************************

The following definitions are the exact name that appears within the API. These
class templates provide factories that create objects compatible with the
system. These objects might have invoke and emit methods depending upon the emit
abstract interfaces named for inheritance. The base services for
these objects to be functional for the display are provided. These objects
have all of the move and copy operators implemented for ease and efficiency of
use. There are several types of objects that may be created and inserted by
using the template parameters. Please refer to the class template for a more
detailed explanation. The following templates are provided for use:


marker_emitter_t
painter_brush_emitter_t
storage_emitter_t
class_storage_emitter_t
storage_drawing_function_t
class_storage_drawing_function_t


*********************************************************************************/

/**
\class surface_area_brush_t
\brief stores and emits the background of the surface. The class inherits from
  painter_brush_emitter_t.

*/
namespace uxdevice {
using surface_area_brush_t = class surface_area_brush_t
    : public painter_brush_emitter_t<surface_area_brush_t,
                                     emit_display_context_abstract_t> {
public:
  using painter_brush_emitter_t::painter_brush_emitter_t;

  void emit(display_context_t &context);
};
} // namespace uxdevice

UX_REGISTER_STD_HASH_SPECIALIZATION(uxdevice::surface_area_brush_t);

/**
\class text_font_t
\brief
*/
namespace uxdevice {
using text_font_t = class text_font_t
    : public class_storage_emitter_t<text_font_t, text_font_storage_t,
                                     attribute_display_context_memory_t,
                                     emit_pango_abstract_t> {
public:
  using class_storage_emitter_t::class_storage_emitter_t;
  void emit(PangoLayout *layout);
};
} // namespace uxdevice
UX_REGISTER_STD_HASH_SPECIALIZATION(uxdevice::text_font_t);

/**
\class
\brief
*/
namespace uxdevice {
using surface_area_title_t = class surface_area_title_t
    : public storage_emitter_t<surface_area_title_t, std::string,
                               attribute_display_context_memory_t,
                               emit_display_context_abstract_t> {
public:
  using storage_emitter_t::storage_emitter_t;

  void emit(display_context_t &context);
};
} // namespace uxdevice
UX_REGISTER_STD_HASH_SPECIALIZATION(uxdevice::surface_area_title_t);

/**
\class
\brief
*/
namespace uxdevice {
using text_render_normal_t = class text_render_normal_t
    : public marker_emitter_t<text_render_normal_t,
                              attribute_display_context_memory_t,
                              emit_display_context_abstract_t> {
public:
  using marker_emitter_t::marker_emitter_t;
  void emit(display_context_t &context);
};
} // namespace uxdevice
UX_REGISTER_STD_HASH_SPECIALIZATION(uxdevice::text_render_normal_t);

/**
\class
\brief
*/
namespace uxdevice {
using text_render_path_t = class text_render_path_t
    : public marker_emitter_t<text_render_path_t,
                              attribute_display_context_memory_t,
                              emit_display_context_abstract_t> {
public:
  using marker_emitter_t::marker_emitter_t;

  void emit(display_context_t &context);
};
} // namespace uxdevice
UX_REGISTER_STD_HASH_SPECIALIZATION(uxdevice::text_render_path_t);

/**
\class
\brief
*/
namespace uxdevice {
using text_color_t = class text_color_t
    : public painter_brush_emitter_t<text_color_t,
                                     attribute_display_context_memory_t,
                                     emit_cairo_coordinate_abstract_t> {
public:
  using painter_brush_emitter_t::painter_brush_emitter_t;
};
} // namespace uxdevice
UX_REGISTER_STD_HASH_SPECIALIZATION(uxdevice::text_color_t);

/**
\class
\brief
*/
namespace uxdevice {
using text_outline_t = class text_outline_t
    : public painter_brush_emitter_t<text_outline_t,
                                     attribute_display_context_memory_t> {
public:
  using painter_brush_emitter_t::painter_brush_emitter_t;
};
} // namespace uxdevice
UX_REGISTER_STD_HASH_SPECIALIZATION(uxdevice::text_outline_t);

/**
\class
\brief
*/
namespace uxdevice {
using text_fill_t = class text_fill_t
    : public painter_brush_emitter_t<text_fill_t,
                                     attribute_display_context_memory_t> {
public:
  using painter_brush_emitter_t::painter_brush_emitter_t;
};
} // namespace uxdevice
UX_REGISTER_STD_HASH_SPECIALIZATION(uxdevice::text_fill_t);

/**
\class
\brief
*/
namespace uxdevice {
using text_shadow_t = class text_shadow_t
    : public painter_brush_emitter_t<text_shadow_t,
                                     attribute_display_context_memory_t> {
public:
  using painter_brush_emitter_t::painter_brush_emitter_t;
};
} // namespace uxdevice
UX_REGISTER_STD_HASH_SPECIALIZATION(uxdevice::text_shadow_t);

/**
\class
\brief
*/
namespace uxdevice {
using text_alignment_t = class text_alignment_t
    : public storage_emitter_t<text_alignment_t, text_alignment_options_t,
                               attribute_display_context_memory_t,
                               emit_pango_abstract_t> {
public:
  using storage_emitter_t::storage_emitter_t;

  void emit(PangoLayout *layout);
};
} // namespace uxdevice
UX_REGISTER_STD_HASH_SPECIALIZATION(uxdevice::text_alignment_t);

/**
\class
\brief
*/
namespace uxdevice {
using text_indent_t = class text_indent_t
    : public storage_emitter_t<text_indent_t, double,
                               attribute_display_context_memory_t,
                               emit_pango_abstract_t> {
public:
  using storage_emitter_t::storage_emitter_t;

  void emit(PangoLayout *layout);
};
} // namespace uxdevice
UX_REGISTER_STD_HASH_SPECIALIZATION(uxdevice::text_indent_t);

/**
\class
\brief
*/
namespace uxdevice {
using text_ellipsize_t = class text_ellipsize_t
    : public storage_emitter_t<text_ellipsize_t, text_ellipsize_options_t,
                               attribute_display_context_memory_t,
                               emit_pango_abstract_t> {
public:
  using storage_emitter_t::storage_emitter_t;

  void emit(PangoLayout *layout);
};
} // namespace uxdevice
UX_REGISTER_STD_HASH_SPECIALIZATION(uxdevice::text_ellipsize_t);

/**
\class
\brief
*/
namespace uxdevice {
using text_line_space_t = class text_line_space_t
    : public storage_emitter_t<text_line_space_t, double,
                               attribute_display_context_memory_t,
                               emit_pango_abstract_t> {
public:
  using storage_emitter_t::storage_emitter_t;

  void emit(PangoLayout *layout);
};
} // namespace uxdevice
UX_REGISTER_STD_HASH_SPECIALIZATION(uxdevice::text_line_space_t);

/**
\class
\brief
*/
namespace uxdevice {
using text_tab_stops_t = class text_tab_stops_t
    : public class_storage_emitter_t<text_tab_stops_t, text_tab_stops_storage_t,
                                     attribute_display_context_memory_t,
                                     emit_pango_abstract_t> {
public:
  using class_storage_emitter_t::class_storage_emitter_t;

  void emit(PangoLayout *layout);
};
} // namespace uxdevice
UX_REGISTER_STD_HASH_SPECIALIZATION(uxdevice::text_tab_stops_t);

/**
\class
\brief
*/
namespace uxdevice {
typedef std::variant<std::string, std::shared_ptr<std::string>,
                     std::string_view, std::shared_ptr<std::string_view>,
                     std::shared_ptr<std::stringstream>>
    text_data_storage_t;

using text_data_t = class text_data_t
    : public storage_emitter_t<text_data_t, text_data_storage_t,
                               attribute_display_context_memory_t,
                               emit_pango_abstract_t> {
public:
  using storage_emitter_t::storage_emitter_t;
  std::size_t hash_code(void) const noexcept;
  void emit(PangoLayout *layout);
};
} // namespace uxdevice
UX_REGISTER_STD_HASH_SPECIALIZATION(uxdevice::text_data_t);

// image drawing and other block operations
/**
\class
\brief
*/
namespace uxdevice {
using coordinate_t = class coordinate_t
    : public class_storage_emitter_t<
          coordinate_t, coordinate_storage_t,
          attribute_display_context_memory_t, emit_cairo_abstract_t,
          emit_cairo_relative_coordinate_abstract_t, emit_pango_abstract_t> {
public:
  using class_storage_emitter_t::class_storage_emitter_t;
  void emit(cairo_t *cr) { emit_absolute(cr); }
  void emit_relative(cairo_t *cr);
  void emit_absolute(cairo_t *cr);
  void emit(PangoLayout *layout);
};
} // namespace uxdevice
UX_REGISTER_STD_HASH_SPECIALIZATION(uxdevice::coordinate_t);

/**
\class
\brief
*/
namespace uxdevice {
using antialias_t = class antialias_t
    : public storage_emitter_t<antialias_t, antialias_options_t,
                               attribute_display_context_memory_t,
                               emit_cairo_abstract_t> {
public:
  using storage_emitter_t::storage_emitter_t;
  void emit(cairo_t *cr);
};
} // namespace uxdevice
UX_REGISTER_STD_HASH_SPECIALIZATION(uxdevice::antialias_t);

/**
\class
\brief
*/
namespace uxdevice {
using line_width_t = class line_width_t
    : public storage_emitter_t<line_width_t, double,
                               attribute_display_context_memory_t,
                               emit_cairo_abstract_t> {
public:
  using storage_emitter_t::storage_emitter_t;

  void emit(cairo_t *cr);
};
} // namespace uxdevice
UX_REGISTER_STD_HASH_SPECIALIZATION(uxdevice::line_width_t);

/**
\class
\brief
*/
namespace uxdevice {
using line_cap_t = class line_cap_t
    : public storage_emitter_t<line_cap_t, line_cap_options_t,
                               attribute_display_context_memory_t,
                               emit_cairo_abstract_t> {
public:
  using storage_emitter_t::storage_emitter_t;

  void emit(cairo_t *cr);
};
} // namespace uxdevice
UX_REGISTER_STD_HASH_SPECIALIZATION(uxdevice::line_cap_t);

/**
\class
\brief
*/
namespace uxdevice {
using line_join_t = class line_join_t
    : public storage_emitter_t<line_join_t, line_join_options_t,
                               attribute_display_context_memory_t,
                               emit_cairo_abstract_t> {
public:
  using storage_emitter_t::storage_emitter_t;

  void emit(cairo_t *cr);
};
} // namespace uxdevice
UX_REGISTER_STD_HASH_SPECIALIZATION(uxdevice::line_join_t);

/**
\class
\brief
*/
namespace uxdevice {
using miter_limit_t = class miter_limit_t
    : public storage_emitter_t<miter_limit_t, double,
                               attribute_display_context_memory_t,
                               emit_cairo_abstract_t> {
public:
  using storage_emitter_t::storage_emitter_t;

  void emit(cairo_t *cr);
};
} // namespace uxdevice
UX_REGISTER_STD_HASH_SPECIALIZATION(uxdevice::miter_limit_t);

/**
\class
\brief
*/
namespace uxdevice {
using line_dashes_t = class line_dashes_t
    : public class_storage_emitter_t<line_dashes_t, line_dash_storage_t,
                                     attribute_display_context_memory_t,
                                     emit_cairo_abstract_t> {
public:
  using class_storage_emitter_t::class_storage_emitter_t;

  void emit(cairo_t *cr);
};
} // namespace uxdevice
UX_REGISTER_STD_HASH_SPECIALIZATION(uxdevice::line_dashes_t);

/**
\class
\brief
*/
namespace uxdevice {
using tollerance_t = class tollerance_t
    : public storage_emitter_t<tollerance_t, double,
                               attribute_display_context_memory_t,
                               emit_cairo_abstract_t> {
public:
  using storage_emitter_t::storage_emitter_t;

  void emit(cairo_t *cr);
};
} // namespace uxdevice
UX_REGISTER_STD_HASH_SPECIALIZATION(uxdevice::tollerance_t);

/**
\class
\brief
*/
namespace uxdevice {
using graphic_operator_t = class graphic_operator_t
    : public storage_emitter_t<graphic_operator_t, graphic_operator_options_t,
                               attribute_display_context_memory_t,
                               emit_cairo_abstract_t> {
public:
  using storage_emitter_t::storage_emitter_t;

  void emit(cairo_t *cr);
};
} // namespace uxdevice
UX_REGISTER_STD_HASH_SPECIALIZATION(uxdevice::graphic_operator_t);

/**
\class
\brief
*/
namespace uxdevice {
using relative_coordinate_t = class relative_coordinate_t
    : public marker_emitter_t<relative_coordinate_t,
                              attribute_display_context_memory_t,
                              emit_display_context_abstract_t> {
public:
  using marker_emitter_t::marker_emitter_t;

  void emit(display_context_t &context);
};
} // namespace uxdevice
UX_REGISTER_STD_HASH_SPECIALIZATION(uxdevice::relative_coordinate_t);

/**
\class
\brief
*/
namespace uxdevice {
using absolute_coordinate_t = class absolute_coordinate_t
    : public marker_emitter_t<absolute_coordinate_t,
                              attribute_display_context_memory_t,
                              emit_display_context_abstract_t> {
public:
  using marker_emitter_t::marker_emitter_t;

  void emit(display_context_t &context);
};
} // namespace uxdevice
UX_REGISTER_STD_HASH_SPECIALIZATION(uxdevice::absolute_coordinate_t);

/**
\class
\brief
*/
namespace uxdevice {
using function_object_t = class function_object_t
    : public storage_emitter_t<function_object_t, cairo_function_t,
                               emit_cairo_abstract_t> {
public:
  using storage_emitter_t::storage_emitter_t;

  void emit(cairo_t *cr);
};
} // namespace uxdevice
UX_REGISTER_STD_HASH_SPECIALIZATION(uxdevice::function_object_t);

/**
\class
\brief
*/
namespace uxdevice {
using draw_function_object_t = class draw_function_object_t
    : public storage_drawing_function_t<draw_function_object_t,
                                        cairo_function_t,
                                        emit_display_context_abstract_t> {
public:
  using storage_drawing_function_t::storage_drawing_function_t;

  void emit(display_context_t &context);
};
} // namespace uxdevice
UX_REGISTER_STD_HASH_SPECIALIZATION(uxdevice::draw_function_object_t);
/**
\class
\brief
*/
namespace uxdevice {
using option_function_object_t = class option_function_object_t
    : public class_storage_emitter_t<option_function_object_t,
                                     cairo_option_function_t,
                                     emit_cairo_abstract_t> {
public:
  using class_storage_emitter_t::class_storage_emitter_t;

  void emit(cairo_t *cr);
};
} // namespace uxdevice
UX_REGISTER_STD_HASH_SPECIALIZATION(uxdevice::option_function_object_t);

/**
\class
\brief
*/
namespace uxdevice {
using textual_render_t = class textual_render_t
    : public class_storage_drawing_function_t<textual_render_t,
                                              textual_render_storage_t,
                                              emit_display_context_abstract_t> {
public:
  using class_storage_drawing_function_t::class_storage_drawing_function_t;

  void emit(display_context_t &context);
};
} // namespace uxdevice
UX_REGISTER_STD_HASH_SPECIALIZATION(uxdevice::textual_render_t);

/**
\class
\brief
*/
namespace uxdevice {
using image_block_t = class image_block_t
    : public class_storage_drawing_function_t<image_block_t,
                                              image_block_storage_t,
                                              emit_display_context_abstract_t> {
public:
  using class_storage_drawing_function_t::class_storage_drawing_function_t;

  void emit(display_context_t &context);
};
} // namespace uxdevice
UX_REGISTER_STD_HASH_SPECIALIZATION(uxdevice::image_block_t);

/**
\class
\brief
*/
// primitives - drawing functions
namespace uxdevice {
using arc_t = class arc_t
    : public class_storage_drawing_function_t<arc_t, arc_storage_t,
                                              emit_cairo_abstract_t> {
public:
  using class_storage_drawing_function_t::class_storage_drawing_function_t;

  void emit(cairo_t *cr);
};
} // namespace uxdevice
UX_REGISTER_STD_HASH_SPECIALIZATION(uxdevice::arc_t);

/**
\class
\brief
*/
namespace uxdevice {
using negative_arc_t = class negative_arc_t
    : public class_storage_drawing_function_t<
          negative_arc_t, negative_arc_storage_t, emit_cairo_abstract_t> {
public:
  using class_storage_drawing_function_t::class_storage_drawing_function_t;

  void emit(cairo_t *cr);
};
} // namespace uxdevice
UX_REGISTER_STD_HASH_SPECIALIZATION(uxdevice::negative_arc_t);

/**
\class
\brief
*/
namespace uxdevice {
using curve_t = class curve_t
    : public class_storage_drawing_function_t<
          curve_t, curve_storage_t, emit_cairo_relative_coordinate_abstract_t> {
public:
  using class_storage_drawing_function_t::class_storage_drawing_function_t;

  void emit_relative(cairo_t *cr);
  void emit_absolute(cairo_t *cr);
};
} // namespace uxdevice
UX_REGISTER_STD_HASH_SPECIALIZATION(uxdevice::curve_t);

/**
\class
\brief
*/
namespace uxdevice {
using line_t = class line_t
    : public class_storage_drawing_function_t<
          line_t, line_storage_t, emit_cairo_relative_coordinate_abstract_t> {
public:
  using class_storage_drawing_function_t::class_storage_drawing_function_t;

  void emit_relative(cairo_t *cr);
  void emit_absolute(cairo_t *cr);
};
} // namespace uxdevice
UX_REGISTER_STD_HASH_SPECIALIZATION(uxdevice::line_t);

/**
\class
\brief
*/
namespace uxdevice {
using vline_t = class vline_t
    : public storage_drawing_function_t<
          vline_t, double, emit_cairo_relative_coordinate_abstract_t> {
public:
  using storage_drawing_function_t::storage_drawing_function_t;

  void emit_relative(cairo_t *cr);
  void emit_absolute(cairo_t *cr);
};
} // namespace uxdevice
UX_REGISTER_STD_HASH_SPECIALIZATION(uxdevice::vline_t);

/**
\class
\brief
*/
namespace uxdevice {
using hline_t = class hline_t
    : public storage_drawing_function_t<
          hline_t, double, emit_cairo_relative_coordinate_abstract_t> {
public:
  using storage_drawing_function_t::storage_drawing_function_t;

  void emit_relative(cairo_t *cr);
  void emit_absolute(cairo_t *cr);
};
} // namespace uxdevice
UX_REGISTER_STD_HASH_SPECIALIZATION(uxdevice::hline_t);

/**
\class
\brief
*/
namespace uxdevice {
using rectangle_t = class rectangle_t
    : public class_storage_drawing_function_t<rectangle_t, rectangle_storage_t,
                                              emit_cairo_abstract_t> {
public:
  using class_storage_drawing_function_t::class_storage_drawing_function_t;

  void emit(cairo_t *cr);
};
} // namespace uxdevice
UX_REGISTER_STD_HASH_SPECIALIZATION(uxdevice::rectangle_t);

/**
\class
\brief
*/
namespace uxdevice {
using stroke_path_t = class stroke_path_t
    : public class_storage_drawing_function_t<stroke_path_t, painter_brush_t,
                                              emit_cairo_abstract_t> {
public:
  using class_storage_drawing_function_t::class_storage_drawing_function_t;

  void emit(cairo_t *cr);
};
} // namespace uxdevice
UX_REGISTER_STD_HASH_SPECIALIZATION(uxdevice::stroke_path_t);

/**
\class
\brief
*/
namespace uxdevice {
using fill_path_t = class fill_path_t
    : public class_storage_drawing_function_t<fill_path_t, painter_brush_t,
                                              emit_cairo_abstract_t> {
public:
  using class_storage_drawing_function_t::class_storage_drawing_function_t;

  void emit(cairo_t *cr);
};
} // namespace uxdevice
UX_REGISTER_STD_HASH_SPECIALIZATION(uxdevice::fill_path_t);

/**
\class
\brief
*/
namespace uxdevice {
using stroke_fill_path_t = class stroke_fill_path_t
    : public class_storage_drawing_function_t<stroke_fill_path_t,
                                              stroke_fill_path_storage_t,
                                              emit_cairo_abstract_t> {
public:
  using class_storage_drawing_function_t::class_storage_drawing_function_t;

  void emit(cairo_t *cr);
};
} // namespace uxdevice
UX_REGISTER_STD_HASH_SPECIALIZATION(uxdevice::stroke_fill_path_t);

/**
\class
\brief
*/
namespace uxdevice {
using mask_t = class mask_t
    : public class_storage_drawing_function_t<mask_t, painter_brush_t,
                                              emit_cairo_abstract_t> {
public:
  using class_storage_drawing_function_t::class_storage_drawing_function_t;

  void emit(cairo_t *cr);
};
} // namespace uxdevice
UX_REGISTER_STD_HASH_SPECIALIZATION(uxdevice::mask_t);

/**
\class
\brief
*/
namespace uxdevice {
using paint_t =
    class paint_t : public storage_drawing_function_t<paint_t, double,
                                                      emit_cairo_abstract_t> {
public:
  using storage_drawing_function_t::storage_drawing_function_t;

  void emit(cairo_t *cr);
};
} // namespace uxdevice
UX_REGISTER_STD_HASH_SPECIALIZATION(uxdevice::paint_t);

/**
\class
\brief
*/
namespace uxdevice {
using close_path_t = class close_path_t
    : public marker_emitter_t<close_path_t, emit_cairo_abstract_t> {
public:
  using marker_emitter_t::marker_emitter_t;

  void emit(cairo_t *cr);
};
} // namespace uxdevice
UX_REGISTER_STD_HASH_SPECIALIZATION(uxdevice::close_path_t);

// event listeners
/**
\class
\brief
*/
namespace uxdevice {
using listen_paint_t =
    class listen_paint_t : public listener_t<listen_paint_t> {};
} // namespace uxdevice
UX_REGISTER_STD_HASH_SPECIALIZATION(uxdevice::listen_paint_t);

/**
\class
\brief
*/
namespace uxdevice {
using listen_focus_t =
    class listen_focus_t : public listener_t<listen_focus_t> {};
} // namespace uxdevice
UX_REGISTER_STD_HASH_SPECIALIZATION(uxdevice::listen_focus_t);

/**
\class
\brief
*/
namespace uxdevice {
using listen_blur_t = class listen_blur_t : public listener_t<listen_blur_t> {};
} // namespace uxdevice
UX_REGISTER_STD_HASH_SPECIALIZATION(uxdevice::listen_blur_t);

/**
\class
\brief
*/
namespace uxdevice {
using listen_resize_t =
    class listen_resize_t : public listener_t<listen_resize_t> {};
} // namespace uxdevice
UX_REGISTER_STD_HASH_SPECIALIZATION(uxdevice::listen_resize_t);

/**

\class
\brief
*/
namespace uxdevice {
using listen_keydown_t =
    class listen_keydown_t : public listener_t<listen_keydown_t> {};
} // namespace uxdevice
UX_REGISTER_STD_HASH_SPECIALIZATION(uxdevice::listen_keydown_t);

/**
\class
\brief
*/
namespace uxdevice {
using listen_keyup_t =
    class listen_keyup_t : public listener_t<listen_keyup_t> {
public:
  using listener_t::listener_t;
};
} // namespace uxdevice
UX_REGISTER_STD_HASH_SPECIALIZATION(uxdevice::listen_keyup_t);

/**
\class
\brief
*/
namespace uxdevice {
using listen_keypress_t =
    class listen_keypress_t : public listener_t<listen_keypress_t> {
public:
  using listener_t::listener_t;
};
} // namespace uxdevice
UX_REGISTER_STD_HASH_SPECIALIZATION(uxdevice::listen_keypress_t);

/**
\class
\brief
*/
namespace uxdevice {
using listen_mouseenter_t =
    class listen_mouseenter_t : public listener_t<listen_mouseenter_t> {
public:
  using listener_t::listener_t;
};
} // namespace uxdevice
UX_REGISTER_STD_HASH_SPECIALIZATION(uxdevice::listen_mouseenter_t);

/**
\class
\brief
*/
namespace uxdevice {
using listen_mousemove_t =
    class listen_mousemove_t : public listener_t<listen_mousemove_t> {
public:
  using listener_t::listener_t;
};
} // namespace uxdevice
UX_REGISTER_STD_HASH_SPECIALIZATION(uxdevice::listen_mousemove_t);

/**
\class
\brief
*/
namespace uxdevice {
using listen_mousedown_t =
    class listen_mousedown_t : public listener_t<listen_mousedown_t> {
public:
  using listener_t::listener_t;
};
} // namespace uxdevice
UX_REGISTER_STD_HASH_SPECIALIZATION(uxdevice::listen_mousedown_t);

/**
\class
\brief
*/
namespace uxdevice {
using listen_mouseup_t =
    class listen_mouseup_t : public listener_t<listen_mouseup_t> {
public:
  using listener_t::listener_t;
};
} // namespace uxdevice
UX_REGISTER_STD_HASH_SPECIALIZATION(uxdevice::listen_mouseup_t);

/**
\class
\brief
*/
namespace uxdevice {
using listen_click_t =
    class listen_click_t : public listener_t<listen_click_t> {
public:
  using listener_t::listener_t;
};
} // namespace uxdevice
UX_REGISTER_STD_HASH_SPECIALIZATION(uxdevice::listen_click_t);

/**
\class
\brief
*/
namespace uxdevice {
using listen_dblclick_t =
    class listen_dblclick_t : public listener_t<listen_dblclick_t> {
public:
  using listener_t::listener_t;
};
} // namespace uxdevice
UX_REGISTER_STD_HASH_SPECIALIZATION(uxdevice::listen_dblclick_t);

/**
\class
\brief
*/
namespace uxdevice {
using listen_contextmenu_t =
    class listen_contextmenu_t : public listener_t<listen_contextmenu_t> {
public:
  using listener_t::listener_t;
};
} // namespace uxdevice
UX_REGISTER_STD_HASH_SPECIALIZATION(uxdevice::listen_contextmenu_t);

/**
\class
\brief
*/
namespace uxdevice {
using listen_wheel_t =
    class listen_wheel_t : public listener_t<listen_wheel_t> {
public:
  using listener_t::listener_t;
};
} // namespace uxdevice
UX_REGISTER_STD_HASH_SPECIALIZATION(uxdevice::listen_wheel_t);

/**
\class
\brief
*/
namespace uxdevice {
using listen_mouseleave_t =
    class listen_mouseleave_t : public listener_t<listen_mouseleave_t> {
public:
  using listener_t::listener_t;
};
} // namespace uxdevice
UX_REGISTER_STD_HASH_SPECIALIZATION(uxdevice::listen_mouseleave_t);
