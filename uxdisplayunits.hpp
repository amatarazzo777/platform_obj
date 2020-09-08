/*
 * This file is part of the PLATFORM_OBJ distribution
 * {https://github.com/amatarazzo777/platform_obj).
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
as part of a base heirarchy of classes. display_unit_t is the base class.
*/

#pragma once

/**
\internal
\typedef indirect_index_display_unit_t as a base member, the key data member
uses this type. All classes can provide an indirect numerical or string key.
The [] operator exposes searching for objects.
 */
namespace uxdevice {
typedef std::variant<std::string, std::size_t> indirect_index_display_unit_t;
}

/**
\internal
\template specializes the std::hash<uxdevice::indirect_index_display_unit_t>
 * std structure for () operator hashing.
 */
template <> struct std::hash<uxdevice::indirect_index_display_unit_t> {
  std::size_t
  operator()(uxdevice::indirect_index_display_unit_t const &o) const noexcept {
    size_t value = o.index();
    uxdevice::hash_combine(value, std::type_index(typeid(o)).hash_code());
    if (auto pval = std::get_if<std::string>(&o))
      uxdevice::hash_combine(value, *pval);
    else if (auto pval = std::get_if<std::size_t>(&o))
      uxdevice::hash_combine(value, *pval);
    return value;
  }
};

/**
\internal

\class index_by_t

\brief A class which is inherited which manages the index key data.
The operators, copy, move constructors are invoked by the class deriving:
display_unit_t.

 */
namespace uxdevice {
class index_by_t {
public:
  /// @brief default constructor
  index_by_t() {}

  /// @brief constructor storing a string as key
  index_by_t(std::string _k) : key(_k) {}

  /// @brief constructor storing a size_t as key
  index_by_t(std::size_t _k) : key(_k) {}

  /// @brief copy assignment operator
  index_by_t &operator=(const index_by_t &other) {
    key = other.key;
    return *this;
  }
  /// @brief move assignment
  index_by_t &operator=(index_by_t &&other) noexcept {
    key = std::move(other.key);
  }
  /// @brief move constructor
  index_by_t(index_by_t &&other) noexcept { key = std::move(other.key); }

  /// @brief copy constructor
  index_by_t(const index_by_t &other) { *this = other; }

  // @brief virtual destructor, all objects must have a destructor
  virtual ~index_by_t() {}

  HASH_OBJECT_MEMBERS(HASH_TYPE_ID_THIS, key)
  indirect_index_display_unit_t key = {};
};
} // namespace uxdevice

/**
\internal
\template specializes the std::hash<uxdevice::index_by_t>
 * std structure for () operator hashing. The object's
 * type id as well as its value is summarized. The object
 * exposes a public function hash_code which is invoked.
 */
template <> struct std::hash<uxdevice::index_by_t> {
  std::size_t operator()(uxdevice::index_by_t const &o) const noexcept {
    size_t value = std::type_index(typeid(o)).hash_code();
    uxdevice::hash_combine(value, o.hash_code());
    return value;
  }
};

/// \typedef cairo_function_t
/// @brief holds a call to a cairo function with parameters bound.
/// The cairo context - cr is provided later.
namespace uxdevice {
typedef std::function<void(cairo_t *cr)> cairo_function_t;
using cairo_function_t = cairo_function_t;

} // namespace uxdevice

/**
\internal
\class display_unit_t
\brief

\brief base class for all display units. defaulted
is the is_output function. Drawing object should override
this and return true. This enables the checking of the surface
for errors after invocation.

*/
namespace uxdevice {
class display_unit_t : public index_by_t {
public:
  display_unit_t() {}

  /// @brief copy assignment operator
  display_unit_t &operator=(const display_unit_t &other) {
    index_by_t::operator=(other);
    is_processed = other.is_processed;
    viewport_inked = other.viewport_inked;
    error_description = other.error_description;
    return *this;
  }
  /// @brief move assignment
  display_unit_t &operator=(display_unit_t &&other) noexcept {
    index_by_t::operator=(other);
    is_processed = std::move(other.is_processed);
    viewport_inked = std::move(other.viewport_inked);
    error_description = std::move(other.error_description);
    return *this;
  }

  /// @brief move constructor
  display_unit_t(display_unit_t &&other) noexcept {
    index_by_t::operator=(other);
    is_processed = std::move(other.is_processed);
    viewport_inked = std::move(other.viewport_inked);
    error_description = std::move(other.error_description);
    return *this;
  }
  /// @brief copy constructor
  display_unit_t(const display_unit_t &other) {
    index_by_t(other);
    is_processed = other.is_processed;
    viewport_inked = other.viewport_inked;
    error_description = other.error_description;
  }

  virtual ~display_unit_t() {}

  virtual void invoke(display_context_t &context) { state_hash_code(); }
  virtual bool is_output(void) { return false; }
  void error(const char *s) { error_description = s; }
  bool valid(void) { return error_description == nullptr; }

  HASH_OBJECT_MEMBERS(HASH_TYPE_ID_THIS, data_storage, error_description)

  bool is_processed = false;
  bool viewport_inked = false;
  const char *error_description = nullptr;
};
} // namespace uxdevice
STD_HASHABLE(uxdevice::display_unit_t);

namespace uxdevice {
/**
\internal
\class drawing_output_t
\brief base class for objects that produce image_block_t drawing commands
The is_output is overridden to return true. As well the object uses
the render work list to determine if a particular image_block_t is on screen.

\details


 */
class drawing_output_t : public display_unit_t {
public:
  typedef display_context_t::context_cairo_region_t context_cairo_region_t;

  /// @brief default constructor
  drawing_output_t() : display_unit_t() {}

  /// @brief move assignment
  drawing_output_t &operator=(drawing_output_t &&other) noexcept {
    display_unit_t::operator=(other);

    internal_buffer = std::move(other.internal_buffer);

    fn_draw = std::move(other.fn_draw);
    fn_draw_clipped = std::move(other.fn_draw_clipped);
    fn_cache_surface = std::move(other.fn_cache_surface);
    fn_base_surface = std::move(other.fn_base_surface);

    options = std::move(other.options);
    ink_rectangle = std::move(other.ink_rectangle);
    intersection_int = std::move(other.intersection_int);
    intersection_double = std::move(other.intersection_double);
  }

  /// @brief copy assignment operator
  drawing_output_t &operator=(const drawing_output_t &other) {
    display_unit_t::operator=(other);

    if (other.internal_buffer.rendered)
      internal_buffer.rendered =
          cairo_surface_reference(other.internal_buffer.rendered);

    if (other.internal_buffer.cr)
      internal_buffer.cr = cairo_reference(other.internal_buffer.cr);

    fn_draw = other.fn_draw;
    fn_draw_clipped = other.fn_draw_clipped;
    fn_cache_surface = other.fn_cache_surface;
    fn_base_surface = other.fn_base_surface;

    std::copy(other.options.begin(), other.options.end(),
              std::back_inserter(options));

    ink_rectangle = other.ink_rectangle;
    intersection_int = other.intersection_int;
    intersection_double = other.intersection_double;

    return *this;
  }

  /// @brief move constructor
  drawing_output_t(drawing_output_t &&other) noexcept {
    display_unit_t(other);
    internal_buffer = std::move(other.internal_buffer);

    fn_draw = std::move(other.fn_draw);
    fn_draw_clipped = std::move(other.fn_draw_clipped);
    fn_cache_surface = std::move(other.fn_cache_surface);
    fn_base_surface = std::move(other.fn_base_surface);

    options = std::move(other.options);
    ink_rectangle = std::move(other.ink_rectangle);
    intersection_int = std::move(other.intersection_int);
    intersection_double = std::move(other.intersection_double);

    return *this;
  }
  /// @brief copy constructor
  drawing_output_t(const drawing_output_t &other) { display_unit_t(other); }

  ~drawing_output_t() { display_context_t::destroy_buffer(internal_buffer); }

  void intersect(cairo_rectangle_t &r);
  void intersect(context_cairo_region_t &r);

  void invoke(cairo_t *cr);
  void invoke(display_context_t &context) {}
  bool is_output(void) { return true; }

  // These functions switch the rendering apparatus from off
  // screen threaded to on screen. all rendering is serialize to the main
  // surface
  //
  std::atomic_flag lockFunctors = ATOMIC_FLAG_INIT;
#define LOCK_FUNCTORS_SPIN                                                     \
  while (lockFunctors.test_and_set(std::memory_order_acquire))

#define LOCK_FUNCTORS_CLEAR lockFunctors.clear(std::memory_order_release)

  void functors_lock(bool b) {
    if (b)
      LOCK_FUNCTORS_SPIN;
    else
      LOCK_FUNCTORS_CLEAR;
  }

  HASH_OBJECT_MEMBERS(HASH_TYPE_ID_THIS, has_ink_extents,
                      last_render_time.time_since_epoch().count(),
                      fn_cache_surface.target_type().name(),
                      fn_base_surface.target_type().name(),
                      fn_draw.target_type().name(),
                      fn_draw_clipped.target_type().name())

  bool has_ink_extents = false;
  cairo_rectangle_int_t c = cairo_rectangle_int_t();
  cairo_region_overlap_t overlap = CAIRO_REGION_OVERLAP_OUT;

  std::atomic<bool> bRenderBufferCached = false;
  draw_buffer_t internal_buffer = {};

  draw_logic_t fn_cache_surface = draw_logic_t();
  draw_logic_t fn_base_surface = draw_logic_t();
  draw_logic_t fn_draw = draw_logic_t();
  draw_logic_t fn_draw_clipped = draw_logic_t();

  // measure processing time
  std::chrono::system_clock::time_point last_render_time = {};
  void evaluate_cache(display_context_t &context);
  bool first_time_rendered = true;
  cairo_option_function_t options = {};
  cairo_rectangle_t ink_rectangle = cairo_rectangle_t();
  cairo_rectangle_int_t intersection_int = cairo_rectangle_int_t();
  cairo_rectangle_t intersection_double = cairo_rectangle_t();
};
} // namespace uxdevice
STD_HASHABLE(uxdevice::drawing_output_t);

/**

\class text_color_t
\brief controls the color of text

\details The text_outline_t and text_fill_t when present take precedent over
this class. The text_fill_off_t,text_outline_off_t,text_shadow_off_t can be
used.

*/
DECLARE_PAINTER_BRUSH_DISPLAY_UNIT(text_color_t)

/**

\class text_outline_t
\brief Controls the outline of the text character stroke, fill pattern. useful
gradients can show simulations of artistic lighting effects.

\details When this item is set, text is drawn onto the
surface using the pango vector based api. The api from cairo calls the stroke,
fill and preserve function when appropriate. This shows the text in an outline
fashion. You can control the line color using the painter_brush_t object api as
this class is a descendant through inheirtance. This object publishes all of the
interfaces for controlling the object and its parameters. When items are
changes, the hashing function internally automatically senses the change and
manufactures allocations, changes or updates to the cairo api. This means that
this text rendering capability is not as fast as using the text_color_t class.


 */
DECLARE_PAINTER_BRUSH_DISPLAY_UNIT(text_outline_t);

/**

\class text_fill_t
\brief controls the color of the text character interior, fill pattern. Crafted
gradients can simulate artistic lighting effects.


\details When this item is set, text is drawn onto the
surface using the pango vector based api. The api from cairo calls the stroke,
fill and preserve function when appropriate. This shows the text in a filled
manner. You can control the fill color using the painter_brush_t object api as
this class is a descendant through inheritance. This object publishes all of the
interfaces for controlling the object and its parameters. When items are
changes, the hashing function internally automatically senses the change and
manufactures allocations, changes or updates to the cairo api. This means that
this text rendering capability is not as fast as using the text_color_t class.

 */
DECLARE_PAINTER_BRUSH_DISPLAY_UNIT(text_fill_t)

/**

\class text_shadow_t
\brief controls the shadow, reflect the pattern source given.

\details


 */
DECLARE_PAINTER_BRUSH_DISPLAY_UNIT(text_shadow_t)

/*
\class text_fill_off_t
\brief turns of text fill rendering capability by resetting the pointer
        connectivity.

\details

*/
DECLARE_MARKER_DISPLAY_UNIT(text_fill_off_t)

void text_fill_off_t::invoke(display_context_t &context) {
  context.current_units.text_fill.reset();
}

/**

\class text_outline_off_t
\brief turns of text outline rendering capability by resetting the
pointer connectivity.

\details

 */
DECLARE_MARKER_DISPLAY_UNIT(text_outline_off_t)
void text_outline_off_t::invoke(display_context_t &context) {
  context.current_units.text_fill.reset();
}

/**

\class text_shadow_off_t
\brief turns of text shadow rendering capability by resetting the
pointer connectivity.

\details


 */
DECLARE_MARKER_DISPLAY_UNIT(text_shadow_off_t)
void text_shadow_off_t::invoke(display_context_t &context) {
  context.current_units.text_shadow.reset();
}

/**

\class text_alignment_t
\brief sets the alignment mode when wrapping, left, right, center,
justified.

\details


 */
DECLARE_STORING_EMITTER_DISPLAY_UNIT(text_alignment_t, text_alignment_options_t)

void text_alignment_t::emit(PangoLayout *layout) {
  PangoAlignment &correlated_value = static_cast<PangoAlignment&>(value);
  if (value == text_alignment_options_t::justified &&
      !pango_layout_get_justify(layout)) {
    pango_layout_set_justify(layout, true);
  } else if (pango_layout_get_alignment(layout) != correlated_value) {
    pango_layout_set_justify(layout, false);
    pango_layout_set_alignment(layout, correlated_value);
  }
}

/**

\class coordinates_t
\brief The class is used to hold a location and clipping width, height.

\details

 */
class coordinate_storage_t {
public:
  double x, y, w, h;
  std::size_t hash_code(void) {}
};
DECLARE_STORING_EMITTER_DISPLAY_UNIT(coordinates_t, coordinate_storage_t)

void coordinates_t::invoke(display_context_t &context) {
  cairo_move_to(context.cr, value.x, value.y);
}

/**

\class text_indent_t
\brief

\details


 */

DECLARE_STORING_EMITTER_DISPLAY_UNIT(text_indent_t, double)
void text_indent_t::emit(PangoLayout *layout) {
  int pangoUnits = value * PANGO_SCALE;
  pango_layout_set_indent(layout, pangoUnits);
}

/**

\class text_ellipsize_t
\brief

\details


 */
DECLARE_STORING_EMITTER_DISPLAY_UNIT(text_ellipsize_t, text_ellipsize_options_t)

void text_ellipsize_t::emit(PangoLayout *layout) {
  PangoEllipsizeMode &correlated_value = static_cast<PangoEllipsizeMode&>(value);
  pango_layout_settext_ellipsize(layout, correlated_value);
}

/**

\class text_line_space_t
\brief

\details


 */
DECLARE_STORING_EMITTER_DISPLAY_UNIT(text_line_space_t, double)

void text_line_space_t::emit(PangoLayout *layout) {
  pango_layout_set_line_spacing(layout, static_cast<float>(value));
}

/**

\class text_tab_stops_t
\brief

\details


*/
DECLARE_STORING_EMITTER_DISPLAY_UNIT(text_tab_stops_t, std::vector<double>)
void text_tab_stops_t::emit(PangoLayout *layout) {
  PangoTabArray *tabs = pango_tab_array_new(value.size(), true);

  int idx = 0;
  for (auto &tabdef : value) {
    int loc = static_cast<int>(tabdef);
    pango_tab_array_set_tab(tabs, idx, PANGO_TAB_LEFT, loc);
  }
  pango_layout_set_tabs(layout, tabs);
  pango_tab_array_free(tabs);
}

/**
\internal
\class text_font_data_storage
\brief

\details


 */
namespace uxdevice {
class uxdevice::text_font_data_storage {
public:
  std::string description;
  std::atomic<PangoFontDescription *> pango_font_ptr;

  // hash function
  std::size_t hash_code(void) {
    std::size_t value = HASH_TYPE_ID_THIS;
    hash_combine(value, description);
    return value;
  }

  void invoke(display_context_t &context) {
    if (!pango_font_ptr) {
      pango_font_ptr = pango_font_description_from_string(description.data());
      if (!pango_font_ptr) {
        std::string s = "Font could not be loaded from description. ( ";
        s += value + ")";
        context.error_state(__func__, __LINE__, __FILE__, std::string_view(s));
      }
    }
  }

  /*********/
  emit(PangoLayout *layout) {}
  /*********/

  ~text_font_data_storage() {
    if (pango_font_ptr)
      pango_font_description_free(pango_font_ptr);
  }
};
} // namespace uxdevice
STD_HASHABLE(text_font_data_storage)

/**

\class text_font_t
\brief

\details


 */
DECLARE_STORING_EMITTER_DISPLAY_UNIT(text_font_t, text_font_data_storage_t)

void text_font_t::emit(display_unit_t &context) { value.emit(context); }


/**

\class text_data_t
\brief class holds the data to the information to display.

\details


 */
DECLARE_STORING_EMITTER_DISPLAY_UNIT(text_data_t, std::string)
void text_data_t::invoke(display_context_t &context) {
  context.current_units.text_data = this;
}

/**

\class textual_render_storage_t
\brief class used to store parameters and options for a textual render. The
object is created as the side effect of inserting text, char *, std string or
a std::shared_ptr<std::string>.
*/
namespace uxdevice {
class textual_render_storage_t {
public:
  bool set_layout_options(cairo_t *cr);
  void create_shadow(void);
  void setup_draw(display_context_t &context);

  std::atomic<cairo_surface_t *> shadow_image = nullptr;
  std::atomic<cairo_t *> shadow_cr = nullptr;
  std::atomic<PangoLayout *> layout = nullptr;
  PangoRectangle ink_rect = PangoRectangle();
  PangoRectangle logical_rect = PangoRectangle();
  Matrix matrix = {};

  // local parameter pointers
  std::shared_ptr<text_color_t> text_color = nullptr;
  std::shared_ptr<text_outline_t> text_outline = nullptr;
  std::shared_ptr<text_fill_t> text_fill = nullptr;
  std::shared_ptr<text_shadow_t> text_shadow = nullptr;
  std::shared_ptr<text_font_t> text_font = nullptr;
  std::shared_ptr<text_alignment_t> text_alignment = nullptr;
  std::shared_ptr<coordinates_t> coordinates = nullptr;
  std::shared_ptr<text_data_t> text_data = nullptr;

  HASH_OBJECT_MEMBERS(drawing_output_t::hash_code(), HASH_TYPE_ID_THIS,
                      HASH_OBJECT_MEMBER_SHARED_PTR(text_color),
                      HASH_OBJECT_MEMBER_SHARED_PTR(text_outline),
                      HASH_OBJECT_MEMBER_SHARED_PTR(text_fill),
                      HASH_OBJECT_MEMBER_SHARED_PTR(text_shadow),
                      HASH_OBJECT_MEMBER_SHARED_PTR(text_font),
                      HASH_OBJECT_MEMBER_SHARED_PTR(text_alignment),
                      HASH_OBJECT_MEMBER_SHARED_PTR(coordinates),
                      HASH_OBJECT_MEMBER_SHARED_PTR(text_data),
                      pango_layout_get_serial(layout), ink_rect.x, ink_rect.y,
                      ink_rect.width, ink_rect.height, matrix.hash_code())
};
} // namespace uxdevice
DECLARE_STORING_EMITTER_DRAWING_FUNCTION(textual_render_t,
                                         textual_render_storage_t)


/**

\class image_block_storage_t
\brief storage class used by the image_block_t object. The oject is
responsible for encapsulating and dynamically allocating, and releasing
memory.

\details


 */
class image_block_storage_t {
public:
  /// @brief default constructor
  image_block_storage_t() {}

  /// @brief move assignment
  image_block_storage_t &operator=(image_block_storage_t &&other) noexcept {
    image_block_ptr = std::move(other.image_block_ptr);
    is_SVG = other.is_SVG;
    if (image_block_ptr)
      is_loaded = true;
    coordinates = std::move(other.coordinates);
    return *this;
  }

  /// @brief copy assignment operator
  image_block_storage_t &operator=(const image_block_storage_t &other) {
    image_block_ptr = cairo_surface_reference(other.image_block_ptr);
    is_SVG = other.is_SVG;
    if (image_block_ptr)
      is_loaded = true;
    coordinates = other.coordinates;
    return *this;
  }

  /// @brief move constructor
  image_block_storage_t(display_unit_t &&other) noexcept {
    index_by_t::operator=(other);

    return *this;
  }
  /// @brief copy constructor
  image_block_storage_t(const display_unit_t &other) { index_by_t(other); }

  virtual ~image_block_storage_t() {
    if (image_block_ptr)
      cairo_surface_destroy(image_block_ptr);
  }

  HASH_OBJECT_MEMBERS(drawing_output_t::hash_code(), HASH_TYPE_ID_THIS, value,
                      is_SVG, coordinates ? coordinates->hash_code() : 0)

  std::string &value;
  std::atomic<cairo_surface_t *> image_block_ptr = {};
  bool is_SVG = {};
  std::atomic<bool> is_loaded = {};
  std::shared_ptr<coordinates_t> coordinates = {};
};

DECLARE_STORING_EMITTER_DRAWING_FUNCTION(image_block_t, image_block_storage_t)

/**
\internal
\class function_object_t
\internal
\brief call previously bound function that was supplied parameter
 using std::bind with the cairo context.

\details

*/
DECLARE_STORING_EMITTER_DISPLAY_UNIT(function_object_t, cairo_function_t)
void function_object_t::invoke(display_context_t &context) {
  value(context.cr);
}

/**
\internal
\class function_object_t
\internal
\brief call previously bound function that was supplied parameter
 using std::bind with the cairo context.

\details

*/
DECLARE_STORING_EMITTER_DISPLAY_UNIT(draw_function_object_t, cairo_function_t)
void draw_function_object_t::invoke(display_context_t &context) {
  value(context.cr);
}

/**

\class option_function_object_t
\brief

\details


 */

DECLARE_STORING_EMITTER_DISPLAY_UNIT(option_function_object_t, cairo_function_t)
void option_function_object_t::invoke(display_context_t &context) {
  value(context.cr);
}

/**

\class antialias_t
\brief

\details


 */
DECLARE_STORING_EMITTER_DISPLAY_UNIT(antialias_t, antialias_options_t)
void antialias_t::invoke(display_context_t &context) {
  cairo_set_antialias(context.cr, static_cast<cairo_antialias_t>(value));
}

/**

\class line_width_t
\brief sets the line width when use during a stroke path operation.
This includes text and line drawing.

\details


 */
DECLARE_STORING_EMITTER_DISPLAY_UNIT(line_width_t, double)

void line_width_t::invoke(display_context_t &context) {
  cairo_set_line_width(context.cr, value);
}

/**

\class line_cap_t
\brief

\details


*/
DECLARE_STORING_EMITTER_DISPLAY_UNIT(line_cap_t, line_cap_options_t)

void line_cap_t::invoke(display_context_t &context) {
  cairo_set_line_cap_t(context.cr,
                       static_cast<cairo_line_cap_options_t>(value));
}

/**

\class line_join_t
\brief

\details


*/
DECLARE_STORING_EMITTER_DISPLAY_UNIT(line_join_t, line_join_options_t)

void line_join_t::invoke(display_context_t &context) {
  cairo_set_line_join_t(context.cr,
                        static_cast<cairo_line_join_options_t>(value));
}

/**

\class miter_limit_t
\brief

\details


 */
DECLARE_STORING_EMITTER_DISPLAY_UNIT(miter_limit_t, double)

void miter_limit_t::invoke(display_context_t &context) {
  cairo_set_miter_limit_t(context.cr, value);
}

/**

\class line_dashes_t
\brief

\details


 */
DECLARE_STORING_EMITTER_DISPLAY_UNIT(line_dashes_t,
                                     std::tuple<std::vector<double>, double>)

void line_dashes_t::invoke(display_context_t &context) {
  cairo_set_dash(context.cr, std::get<0>(value).data(),
                 std::get<0>(value).size(), std::get<1>(value).data());
}


/**

\class tollerance_t
\brief

\details


 */
DECLARE_STORING_EMITTER_DISPLAY_UNIT(tollerance_t, double)

void tollerance_t::invoke(display_context_t &context) {
  cairo_set_tolerance(context.cr, value);
}

/**

\class graphic_operator
\brief

\details


*/
DECLARE_STORING_EMITTER_DISPLAY_UNIT(graphic_operator_t,
                                     graphic_operator_options_t)

void graphic_operator_t::invoke(display_context_t &context) {
  cairo_set_operator(context.cr, static_cast<cairo_operator_t>(value));
}

/**
\internal
\class arc_storage_t
\brief

\details


 */
struct arc_storage_t {
  double xc;
  double yc;
  double radius;
  double angle1;
  double angle2;
};

/**

\class arc_t
\brief

\details


 */
DECLARE_STORING_EMITTER_DRAWING_FUNCTION(arc_t, arc_storage_t)
void arc_t::invoke(display_context_t &context) {
  cairo_arc_t(context.cr, value.xc, value.yc, value.radius, value.angle1,
              value.angle2);
}

/**
\internal
\class negative_arc_storage_t
\brief

\details


 */
struct negative_arc_storage_t {
  double xc, yc, radius, angle1, angle2;
};

/**

\class negative_arc_t
\brief

\details


 */
DECLARE_STORING_EMITTER_DRAWING_FUNCTION(negative_arc_t, negative_arc_storage_t)
void negative_arc_t::invoke(display_context_t &context) {
  cairo_arc_negative(context.cr, value.xc, value.yc, value.radius, value.angle1,
                     value.angle2);
}

/**

\class curve_storage_t
\brief

\details


 */
struct curve_storage_t {
  double x1, y1, x2, y2, x3, y3;
};
DECLARE_STORING_EMITTER_DRAWING_FUNCTION(curve_storage_t, curve_storage_t)
void curve_t::invoke(display_context_t &context) {
  if (context.relative_coordinates)
    cairo_rel_curve_t_to(context.cr, value.x1, value.y1, value.x2, value.y3,
                         value.x3, value.y3);
  else
    cairo_curve_t_to(context.cr, value.x1, value.y1, value.x2, value.y2,
                     value.x3, value.y3);
}

/**

\class line_t
\brief

\details


 */
struct line_storage_t {
  double x, y;
};
DECLARE_STORING_EMITTER_DRAWING_FUNCTION(line_t, line_storage_t)
void line_t::invoke(display_context_t &context) {
  if (context.relative_coordinates)
    cairo_rel_line_to(context.cr, value.x, value.y);
  else
    cairo_line_to(context.cr, value.x, value.y);
}

/**

\class hline_t
\brief

\details


 */
DECLARE_STORING_EMITTER_DRAWING_FUNCTION(hline_t, double)
void hline_t::invoke(display_context_t &context) {
  if (cairo_has_current_point(context.cr)) {
    double curx = 0.0, cury = 0.0;
    cairo_get_current_point(context.cr, &curx, &cury);

    if (context.relative_coordinates)
      cairo_rel_line_to(context.cr, value, 0);
    else
      cairo_line_to(context.cr, value, cury);
  }
}

/**

\class vline_t
\brief

\details


 */
DECLARE_STORING_EMITTER_DRAWING_FUNCTION(vline_t, double)
void vline_t::invoke(display_context_t &context) {
  if (cairo_has_current_point(context.cr)) {
    double curx = 0.0, cury = 0.0;
    cairo_get_current_point(context.cr, &curx, &cury);

    if (context.relative_coordinates)
      cairo_rel_line_to(context.cr, 0, value);
    else
      cairo_line_to(context.cr, curx, value);
  }
}



/**

\class rectangle_t
\brief

\details


 */
struct rectangle_storage_t {
  double x, y, width, height;
};
DECLARE_STORING_EMITTER_DRAWING_FUNCTION(rectangle_t, rectangle_storage_t)
void rectangle_t::invoke(display_context_t &context) {
  cairo_rectangle(context.cr, value.x, value.y, value.width, value.height);
}

/**

\class close_path_t
\brief

\details


 */
DECLARE_MARKER_DISPLAY_UNIT(close_path_t)

void close_path_t::invoke(display_context_t &context) {
  cairo_close_path_t(context.cr);
}

/**

\class stroke_path_t
\brief

\details


*/
DECLARE_STORING_EMITTER_DRAWING_FUNCTION(stroke_path_t,
                                         painter_brush_t)
void stroke_path_t::invoke(display_context_t &context) {
  value.emit(context.cr);
  cairo_stroke(context.cr);
}

/**

\class fill_path_t
\brief

\details


 */
DECLARE_STORING_EMITTER_DRAWING_FUNCTION(fill_path_t, painter_brush_t)
void fill_path_t::invoke(display_context_t &context) {
  value.emit(context.cr);
  cairo_fill(context.cr);
}

/**

\class stroke_fill_path_t
\brief

\details


 */
DECLARE_STORING_EMITTER_DRAWING_FUNCTION(stroke_fill_path_t, std::tuple<painter_brush_t,painter_brush_t>)
void stroke_path_t::invoke(display_context_t &context) {
  std::get<0>(value).emit(context.cr);
  cairo_stroke_preserve(context.cr);
  std::get<01(value).emit(context.cr);
  cairo_fill(context.cr);
}

/**

\class mask_t
\brief

\details


 */
DECLARE_STORING_EMITTER_DRAWING_FUNCTION(mask_t, painter_brush_t)
void mask_t::invoke(display_context_t &context) {}

/**

\class paint_t
\brief

\details


 */
DECLARE_STORING_EMITTER_DRAWING_FUNCTION(paint_t, double)
void paint_t::invoke(display_context_t &context) {
  if (value == 1.0) {
    cairo_paint(context.cr);
  } else {
    cairo_paint_with_alpha(context.cr, value);
  }
}
/**

\class relative_coordinates_t
\brief

\details


 */
DECLARE_NAMED_MARKER_DISPLAY_UNIT(relative_coordinates_t)
void relative_coordinates_t::invoke(display_context_t &context) {
  context.relative_coordinates = true;
}

/**

\class absolute_coordinates_t
\brief

\details


 */
DECLARE_NAMED_MARKER_DISPLAY_UNIT(absolute_coordinates_t)
void absolute_coordinates_t::invoke(display_context_t &context) {
  context.relative_coordinates = false;
}

/**

\class listener_t
\brief

\details

 */
DECLARE_STORING_EMITTER_DISPLAY_UNIT(
    listener_t, std::tuple<std::type_info, event_handler_t>)
void listener_t::invoke(display_context_t &context) {}

/**

\class listen_paint_t
\brief

\details

 */
DECLARE_NAMED_LISTENER_DISPLAY_UNIT(listen_paint_t)

/**

\class listen_focus_t
\brief

\details

 */
DECLARE_NAMED_LISTENER_DISPLAY_UNIT(listen_focus_t)

/**

\class listen_blur_t
\brief

\details

 */
DECLARE_NAMED_LISTENER_DISPLAY_UNIT(listen_blur_t)

/**

\class listen_resize_t
\brief

\details

 */
DECLARE_NAMED_LISTENER_DISPLAY_UNIT(listen_resize_t)

/**

\class listen_keydown_t
\brief

\details

 */
DECLARE_NAMED_LISTENER_DISPLAY_UNIT(listen_keydown_t)

/**

\class listen_keyup_t
\brief

\details

 */
DECLARE_NAMED_LISTENER_DISPLAY_UNIT(listen_keyup_t)

/**

\class listen_keypress_t
\brief

\details

 */
DECLARE_NAMED_LISTENER_DISPLAY_UNIT(listen_keypress_t)

/**

\class listen_mouseenter_t
\brief

\details

 */
DECLARE_NAMED_LISTENER_DISPLAY_UNIT(listen_mouseenter_t)

/**

\class listen_mousemove_t
\brief

\details

 */
DECLARE_NAMED_LISTENER_DISPLAY_UNIT(listen_mousemove_t)

/**

\class listen_mousedown_t
\brief

\details

 */
DECLARE_NAMED_LISTENER_DISPLAY_UNIT(listen_mousedown_t)

/**

\class listen_mouseup_t
\brief

\details

 */
DECLARE_NAMED_LISTENER_DISPLAY_UNIT(listen_mouseup_t)

/**

\class listen_click_t
\brief

\details

 */
DECLARE_NAMED_LISTENER_DISPLAY_UNIT(listen_click_t)

/**

\class listen_dblclick_t
\brief

\details

 */
DECLARE_NAMED_LISTENER_DISPLAY_UNIT(listen_dblclick_t)

/**

\class listen_contextmenu_t
\brief

\details

 */
DECLARE_NAMED_LISTENER_DISPLAY_UNIT(listen_contextmenu_t)

/**

\class listen_wheel_t
\brief

\details

 */
DECLARE_NAMED_LISTENER_DISPLAY_UNIT(listen_wheel_t)

/**

\class listen_mouseleave_t
\brief

\details

 */
DECLARE_NAMED_LISTENER_DISPLAY_UNIT(listen_mouseleave_t)
