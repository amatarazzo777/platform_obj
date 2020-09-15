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
\version 1.0

\brief The modules extends the uxdevice namespace. The objects
provided are the base objects for which the caller may instantiate to
draw. While most of these objects are parameters to drawing functions,
the implementation with in this file provides the functional logic.
All objects derive from the display_unit_t class which contains the
virtual invoke method. Objects that provide drawing operations
can inherit from the drawing_output_t base class which enables visibility
query. As well, a particular note is that parameters that describe colors,
shading or texturing derive and publish the painter_brush_t class interface.

*/

#include "uxdevice.hpp"

/**

\fn text_render_normal_t::emit

\param display_context_t &context

\brief

\details

 */
void uxdevice::text_render_normal_t::emit(display_context_t &context) {
  context.text_path_rendering = false;
}

/**

\fn text_render_normal_t::emit

\param display_context_t &context

\brief

\details

 */
void uxdevice::text_render_path_t::emit(display_context_t &context) {
  context.text_path_rendering = true;
}

/**

\fn surface_area_title_t(painter_brush_t &b)
\param painter_brush_t &b

\brief Sets the background brush of the surface window.

\details WHen rendering occurs, this is painted as the background.
For flexibility, the background may be a color, gradient or image.
The painter_brush_t object interface is used.


 */
void uxdevice::surface_area_title_t::emit(display_context_t &context) {
  // set window title
  xcb_change_property(context.connection, XCB_PROP_MODE_REPLACE, context.window,
                      XCB_ATOM_WM_NAME, XCB_ATOM_STRING, 8, value.size(),
                      value.data());
}

void uxdevice::surface_area_brush_t::emit(display_context_t &context) {
  context.surface_brush(*this);
}

/**

\class text_alignment_t
\brief sets the alignment mode when wrapping, left, right, center,
justified.

\details


 */
void uxdevice::text_alignment_t::emit(PangoLayout *layout) {
  PangoAlignment correlated_value = static_cast<PangoAlignment>(value);
  if (value == text_alignment_options_t::justified &&
      !pango_layout_get_justify(layout)) {
    pango_layout_set_justify(layout, true);
  } else if (pango_layout_get_alignment(layout) != correlated_value) {
    pango_layout_set_justify(layout, false);
    pango_layout_set_alignment(layout, correlated_value);
  }
}

/**

\class coordinate_t
\brief The class is used to hold a location and clipping width, height.

\details

 */
void uxdevice::coordinate_t::emit(cairo_t *cr) { cairo_move_to(cr, x, y); }

/**

\class text_indent_t
\brief

\details


 */
void uxdevice::text_indent_t::emit(PangoLayout *layout) {
  int pangoUnits = value * PANGO_SCALE;
  pango_layout_set_indent(layout, pangoUnits);
}

/**

\class text_ellipsize_t
\brief

\details


 */
void uxdevice::text_ellipsize_t::emit(PangoLayout *layout) {
  pango_layout_set_ellipsize(layout, static_cast<PangoEllipsizeMode>(value));
}

/**

\class text_line_space_t
\brief

\details


 */
void uxdevice::text_line_space_t::emit(PangoLayout *layout) {
  pango_layout_set_line_spacing(layout, static_cast<float>(value));
}

/**

\fn textual_render_storage_t::hash_code(void)
\param ...b

\brief

\details


 */
std::size_t uxdevice::textual_render_storage_t::hash_code(void) const noexcept {
  std::size_t __value = {};
  hash_combine(__value,
               std::type_index(typeid(textual_render_storage_t)).hash_code(),
               pango_layout_get_serial(layout), ink_rect.x, ink_rect.y,
               ink_rect.width, ink_rect.height, matrix.hash_code(),
               unit_memory<text_color_t>()->hash_code(),
               unit_memory<text_outline_t>()->hash_code(),
               unit_memory<text_fill_t>()->hash_code(),
               unit_memory<text_shadow_t>()->hash_code(),
               unit_memory<text_alignment_t>()->hash_code(),
               unit_memory<text_indent_t>()->hash_code(),
               unit_memory<text_ellipsize_t>()->hash_code(),
               unit_memory<text_line_space_t>()->hash_code(),
               unit_memory<text_tab_stops_t>()->hash_code(),
               unit_memory<text_font_t>()->hash_code(),
               unit_memory<text_data_t>()->hash_code(),
               unit_memory<coordinate_t>()->hash_code(),
               unit_memory<antialias_t>()->hash_code(),
               unit_memory<line_width_t>()->hash_code(),
               unit_memory<line_cap_t>()->hash_code(),
               unit_memory<line_join_t>()->hash_code(),
               unit_memory<miter_limit_t>()->hash_code(),
               unit_memory<line_dashes_t>()->hash_code(),
               unit_memory<tollerance_t>()->hash_code(),
               unit_memory<graphic_operator_t>()->hash_code());
  return __value;
}

/**

\class text_tab_stops_t
\brief

\details


*/
void uxdevice::text_tab_stops_t::emit(PangoLayout *layout) {
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

\class text_font_t::emit
\brief

\details


 */
void uxdevice::text_font_t::emit(PangoLayout *layout) {
  if (!font_ptr) {
    font_ptr = pango_font_description_from_string(description.data());
    if (!font_ptr) {
      std::string s = "Font could not be loaded from description. ( ";
      s += description + ")";
      //      context.error_state(__func__, __LINE__, __FILE__,
      //      std::string_view(s));
    }
  }
  if (font_ptr)
    pango_layout_set_font_description(layout, font_ptr);
}

/**
\internal
\fn set_layout_options
\brief manages the layout options.

\details


 */
bool uxdevice::textual_render_storage_t::set_layout_options(cairo_t *cr) {
  bool ret = false;

  // create layout
  if (!layout)
    layout = pango_cairo_create_layout(cr);

  guint layoutSerial = pango_layout_get_serial(layout);

  const PangoFontDescription *originalDescription =
      pango_layout_get_font_description(layout);
  if (!originalDescription ||
      !pango_font_description_equal(originalDescription,
                                    unit_memory<text_font_t>()->font_ptr))
    pango_layout_set_font_description(layout,
                                      unit_memory<text_font_t>()->font_ptr);

  if (unit_memory<text_alignment_t>()) {
    unit_memory<text_alignment_t>()->emit(layout);
  }

  // set the width and height of the layout.
  auto coordinate = unit_memory<coordinate_t>();
  if (pango_layout_get_width(layout) != coordinate->w * PANGO_SCALE)
    pango_layout_set_width(layout, coordinate->w * PANGO_SCALE);

  if (pango_layout_get_height(layout) != coordinate->h * PANGO_SCALE)
    pango_layout_set_height(layout, coordinate->h * PANGO_SCALE);

  auto text_data = unit_memory<text_data_t>();
  std::string_view sinternal = std::string_view(pango_layout_get_text(layout));
  if (text_data->value.compare(sinternal) != 0)
    pango_layout_set_text(layout, text_data->value.data(), -1);

  // any changes
  if (layoutSerial != pango_layout_get_serial(layout)) {
    pango_layout_get_pixel_extents(layout, &ink_rect, &logical_rect);
    int tw = std::min((double)logical_rect.width, coordinate->w);
    int th = std::min((double)logical_rect.height, coordinate->h);
    ink_rectangle = {(int)coordinate->x, (int)coordinate->y, tw, th};
    ink_rectangle_double = {(double)ink_rectangle.x, (double)ink_rectangle.y,
                            (double)ink_rectangle.width,
                            (double)ink_rectangle.height};

    has_ink_extents = true;
    ret = true;
  }

  return ret;
}

/**
\internal
\fn create_shadow
\brief creates an image of the shadowed text

\details


 */
void uxdevice::textual_render_storage_t::create_shadow(void) {
  if (!shadow_image) {
    auto text_shadow = unit_memory<text_shadow_t>();
    shadow_image = cairo_image_surface_create(
        CAIRO_FORMAT_ARGB32, ink_rectangle.width + text_shadow->x,
        ink_rectangle_double.height + text_shadow->y);
    shadow_cr = cairo_create(shadow_image);
    // offset text by the parameter amounts
    cairo_move_to(shadow_cr, text_shadow->x, text_shadow->y);
    if (set_layout_options(shadow_cr))
      pango_cairo_update_layout(shadow_cr, layout);
    text_shadow->emit(shadow_cr);
    pango_cairo_show_layout(shadow_cr, layout);

#if defined(USE_STACKBLUR)
    blur_image(shadow_image, text_shadow->radius);

#elif defined(USE_SVGREN)
    cairo_surface_t *blurred = blur_image(shadow_image, text_shadow->radius);
    cairo_surface_destroy(shadow_image);
    _shadow_image = blurred;
#endif
  }
}

/**
\internal
\fn invoke
\brief creates linkages to the drawing functions and rendering parameters

\details


 */
uxdevice::textual_render_storage_t::internal_cairo_function_t
uxdevice::textual_render_storage_t::precise_rendering_function(void) {
  enum text_rendering_lambda_t {
    text_rendering_default,
    text_rendering_fast_lambda,
    text_rendering_fast_shadowed_lambda,
    text_rendering_fill_lambda,
    text_rendering_outline_lambda,
    text_rendering_fill_outline_lambda,
    text_rendering_fill_shadowed_lambda,
    text_rendering_outline_shadowed_lambda,
    text_rendering_fill_outline_shadowed_lambda
  };
  text_rendering_lambda_t text_render_type = {};
  internal_cairo_function_t fn;

  if (unit_memory<text_render_path_t>()) {
    if (unit_memory<text_fill_t>() && unit_memory<text_outline_t>() &&
        unit_memory<text_shadow_t>())
      text_render_type = text_rendering_fill_outline_shadowed_lambda;

    else if (unit_memory<text_fill_t>() && unit_memory<text_outline_t>())
      text_render_type = text_rendering_fill_outline_lambda;

    else if (unit_memory<text_fill_t>() && unit_memory<text_shadow_t>())
      text_render_type = text_rendering_fill_shadowed_lambda;

    else if (unit_memory<text_outline_t>() && unit_memory<text_shadow_t>())
      text_render_type = text_rendering_outline_shadowed_lambda;

    else if (unit_memory<text_fill_t>())
      text_render_type = text_rendering_fill_lambda;

    else if (unit_memory<text_outline_t>())
      text_render_type = text_rendering_outline_lambda;
  } else {
    if (unit_memory<text_color_t>() && unit_memory<text_shadow_t>())
      text_render_type = text_rendering_fast_shadowed_lambda;

    else if (unit_memory<text_color_t>())
      text_render_type = text_rendering_fast_lambda;
  }

#define FN_SHADOW                                                              \
  create_shadow();                                                             \
  cairo_set_source_surface(cr, shadow_image, a.x, a.y);                        \
  cairo_rectangle(cr, a.x, a.y, a.w, a.h);                                     \
  cairo_fill(cr);

  // set the drawing function to the one that will be used by the rendering
  // options for text. These functions accept five parameters.
  // These are the clipping versions that have coordinate_t.

  switch (text_render_type) {
  case text_rendering_default:
  case text_rendering_fast_lambda: {
    fn = [=](cairo_t *cr, coordinate_t a) {
      // cairo_set_matrix(context.cr, &mat._matrix);
      // drawing_output_t::invoke(cr);

      a.emit(cr);
      unit_memory<text_color_t>()->emit(cr, a);
      pango_cairo_show_layout(cr, layout);
    };
  } break;

  case text_rendering_fast_shadowed_lambda: {
    fn = [=](cairo_t *cr, coordinate_t a) {
      // cairo_set_matrix(context.cr, &mat._matrix);

      // drawing_output_t::invoke(cr);
      if (set_layout_options(cr))
        pango_cairo_update_layout(cr, layout);

      FN_SHADOW

      a.emit(cr);
      unit_memory<text_color_t>()->emit(cr, a);
      pango_cairo_show_layout(cr, layout);
    };

  } break;

  case text_rendering_fill_lambda: {
    fn = [=](cairo_t *cr, coordinate_t a) {
      // cairo_set_matrix(context.cr, &mat._matrix);
      // drawing_output_t::invoke(cr);

      if (set_layout_options(cr))
        pango_cairo_update_layout(cr, layout);
      a.emit(cr);

      pango_cairo_layout_path(cr, layout);
      unit_memory<text_fill_t>()->emit(cr, a);
      cairo_fill(cr);
    };

  } break;

  case text_rendering_outline_lambda: {
    fn = [=](cairo_t *cr, coordinate_t a) {
      // cairo_set_matrix(context.cr, &mat._matrix);
      // drawing_output_t::invoke(cr);

      if (set_layout_options(cr))
        pango_cairo_update_layout(cr, layout);

      a.emit(cr);
      pango_cairo_layout_path(cr, layout);
      unit_memory<text_outline_t>()->emit(cr, a);
      cairo_stroke(cr);
    };
  } break;

  case text_rendering_fill_outline_lambda: {
    fn = [=](cairo_t *cr, coordinate_t a) {
      // cairo_set_matrix(context.cr, &mat._matrix);
      // drawing_output_t::invoke(cr);

      if (set_layout_options(cr))
        pango_cairo_update_layout(cr, layout);

      a.emit(cr);
      pango_cairo_layout_path(cr, layout);
      unit_memory<text_fill_t>()->emit(cr, a);
      cairo_fill_preserve(cr);
      unit_memory<text_outline_t>()->emit(cr, a);
      cairo_stroke(cr);
    };
  } break;

  case text_rendering_fill_shadowed_lambda: {
    fn = [=](cairo_t *cr, coordinate_t a) {
      // cairo_set_matrix(context.cr, &mat._matrix);
      // drawing_output_t::invoke(cr);

      if (set_layout_options(cr))
        pango_cairo_update_layout(cr, layout);

      FN_SHADOW

      a.emit(cr);
      pango_cairo_layout_path(cr, layout);
      unit_memory<text_fill_t>()->emit(cr, a);
      cairo_fill(cr);
    };
  } break;

  case text_rendering_outline_shadowed_lambda: {
    fn = [=](cairo_t *cr, coordinate_t a) {
      // cairo_set_matrix(context.cr, &mat._matrix);
      // drawing_output_t::invoke(cr);
      if (set_layout_options(cr))
        pango_cairo_update_layout(cr, layout);

      FN_SHADOW

      a.emit(cr);
      pango_cairo_layout_path(cr, layout);
      unit_memory<text_outline_t>()->emit(cr, a);
      cairo_stroke(cr);
    };
  } break;

  case text_rendering_fill_outline_shadowed_lambda: {

    fn = [=](cairo_t *cr, coordinate_t a) {
      // cairo_set_matrix(context.cr, &mat._matrix);
      // drawing_output_t::invoke(cr);

      if (set_layout_options(cr))
        pango_cairo_update_layout(cr, layout);

      FN_SHADOW

      a.emit(cr);
      pango_cairo_layout_path(cr, layout);
      unit_memory<text_fill_t>()->emit(cr, a);
      cairo_fill_preserve(cr);
      unit_memory<text_outline_t>()->emit(cr, a);
      cairo_stroke(cr);
    };

  } break;
  }

  return fn;
}

/**
\internal
\fn invoke
\brief creates linkages to the drawing functions and rendering parameters

\details


 */
void uxdevice::textual_render_t::emit(display_context_t &context) {
  using namespace std::placeholders;

  // create a linkage snapshot to the shared pointers stored in unit memory
  // within the stream context.
  copy_unit_memory(context);

  // check the context parameters before operating
  if (!((unit_memory<text_color_t>() || unit_memory<text_outline_t>() ||
         unit_memory<text_fill_t>()) &&
        unit_memory<coordinate_t>() && unit_memory<text_data_t>() &&
        unit_memory<text_font_t>())) {
    const char *s =
        "A draw text object must include the following "
        "attributes. A text_color_t or a text_outline_t or "
        " text_fill_t. As well, a coordinate_t, text and text_font_t object.";
    UX_ERROR_DESC(s);
    auto fn = [=](display_context_t &context) {};

    fn_base_surface = std::bind(fn, _1);
    fn_cache_surface = std::bind(fn, _1);
    fn_draw = std::bind(fn, _1);
    fn_draw_clipped = std::bind(fn, _1);
    return;
  }

  // cairo_get_matrix(context.cr, &mat._matrix);

  auto fn = precise_rendering_function();
  auto &coordinate = *unit_memory<coordinate_t>();

  // two function provide mode switching for the rendering.
  // a cache surface is a new xcb surface that can be threaded in creation
  // base surface issues the drawing commands to the base window drawing cairo
  // context. base surface creation is not threaded.
  fn_cache_surface = [=](display_context_t &context) {
    // if the item is already cached, return.
    if (bRenderBufferCached)
      return;

    // create off screen buffer
    context.lock(true);
    set_layout_options(context.cr);
    UX_ERROR_CHECK(context.cr);
    context.lock(false);

    internal_buffer = context.allocate_buffer(ink_rect.width, ink_rect.height);

    set_layout_options(internal_buffer.cr);
    UX_ERROR_CHECK(internal_buffer.cr);

    coordinate_t a = coordinate;
#if 0
    if(textfill)
      textfill->translate(-a.x,-a.y);
    if(textoutline)
      textoutline->translate(-a.x,-a.y);
#endif // 0
    a.x = 0;
    a.y = 0;

    fn(internal_buffer.cr, a);
    UX_ERROR_CHECK(internal_buffer.cr);

    cairo_surface_flush(internal_buffer.rendered);
    UX_ERROR_CHECK(internal_buffer.rendered);

    auto drawfn = [=](display_context_t &context) {
      // cairo_set_matrix(context.cr, &mat._matrix);
      drawing_output_t::emit(context);

      cairo_set_source_surface(context.cr, internal_buffer.rendered,
                               coordinate.x, coordinate.y);
      double tw, th;
      tw = std::min(ink_rectangle_double.width, coordinate.w);
      th = std::min(ink_rectangle_double.height, coordinate.h);

      cairo_rectangle(context.cr, ink_rectangle_double.x,
                      ink_rectangle_double.y, tw, th);
      cairo_fill(context.cr);
    };
    auto fnClipping = [=](display_context_t &context) {
      auto &coordinate = *unit_memory<coordinate_t>();
      // cairo_set_matrix(context.cr, &mat._matrix);
      drawing_output_t::emit(context);
      cairo_set_source_surface(context.cr, internal_buffer.rendered,
                               coordinate.x, coordinate.y);
      cairo_rectangle(context.cr, intersection_double.x, intersection_double.y,
                      intersection_double.width, intersection_double.height);
      cairo_fill(context.cr);
    };
    functors_lock(true);
    fn_draw = std::bind(drawfn, _1);
    fn_draw_clipped = std::bind(fnClipping, _1);
    functors_lock(false);
    bRenderBufferCached = true;
  };

  // the base option rendered contains two functions that rendering using the
  // cairo api to the base surface context. One is for clipping and one without.
  auto fnBase = [=](display_context_t &context) {
    auto drawfn = [=](display_context_t &context) {
      drawing_output_t::emit(context);
      fn(context.cr, coordinate);
      evaluate_cache(context);
    };
    auto fnClipping = [=](display_context_t &context) {
      cairo_rectangle(context.cr, intersection_double.x, intersection_double.y,
                      intersection_double.width, intersection_double.height);
      cairo_clip(context.cr);
      drawing_output_t::emit(context);
      fn(context.cr, coordinate);
      cairo_reset_clip(context.cr);
      evaluate_cache(context);
    };
    functors_lock(true);
    fn_draw = std::bind(drawfn, _1);
    fn_draw_clipped = std::bind(fnClipping, _1);
    functors_lock(false);
    if (bRenderBufferCached) {
      context.destroy_buffer(internal_buffer);
      bRenderBufferCached = false;
    }
  };
  context.lock(true);
  set_layout_options(context.cr);
  context.lock(false);
  fn_base_surface = fnBase;
  fn_base_surface(context);

  is_processed = true;
}

/**
\internal
\brief
*/
void uxdevice::image_block_t::emit(display_context_t &context) {
  using namespace std::placeholders;

  if (is_loaded)
    return;

  auto coordinate = context.unit_memory<coordinate_t>();
  auto options = context.unit_memory<cairo_option_function_t>();

  if (!(coordinate && description.size())) {
    const char *s = "An image_block_t object must include the following "
                    "attributes. coordinate_t and an image_block_t name.";
    UX_ERROR_DESC(s);
    auto fn = [=](display_context_t &context) {};

    fn_base_surface = std::bind(fn, _1);
    fn_cache_surface = std::bind(fn, _1);
    fn_draw = std::bind(fn, _1);
    fn_draw_clipped = std::bind(fn, _1);
    return;
  }

  // set the ink area.
  coordinate_t &a = *coordinate;

  auto fnthread = [=, &context, &a]() {
    image_block_ptr = read_image(description, coordinate->w, coordinate->h);

    if (image_block_ptr) {

      ink_rectangle = {(int)a.x, (int)a.y, (int)a.w, (int)a.h};
      ink_rectangle_double = {(double)ink_rectangle.x, (double)ink_rectangle.y,
                              (double)ink_rectangle.width,
                              (double)ink_rectangle.height};
      has_ink_extents = true;
      is_loaded = true;
    } else {
      const char *s = "The image_block_t could not be processed or loaded. ";
      UX_ERROR_DESC(s);
      UX_ERROR_DESC(description);
    }
  };

  fnthread();

  auto fnCache = [&](display_context_t &context) {
    // set directly callable rendering function.
    auto fn = [&](display_context_t &context) {
      if (!is_valid())
        return;
      drawing_output_t::emit(context);
      cairo_set_source_surface(context.cr, image_block_ptr, a.x, a.y);
      cairo_rectangle(context.cr, a.x, a.y, a.w, a.h);
      cairo_fill(context.cr);
    };
    auto fnClipping = [&](display_context_t &context) {
      if (!is_valid())
        return;
      drawing_output_t::emit(context);
      cairo_set_source_surface(context.cr, image_block_ptr, a.x, a.y);
      cairo_rectangle(context.cr, intersection_double.x, intersection_double.y,
                      intersection_double.width, intersection_double.height);
      cairo_fill(context.cr);
    };
    functors_lock(true);
    fn_draw = std::bind(fn, _1);
    fn_draw_clipped = std::bind(fnClipping, _1);
    functors_lock(false);
    bRenderBufferCached = true;
  };

  // two function provide mode switching for the rendering.
  // a cache surface is a new xcb surface that can be threaded in creation
  // base surface issues the drawing commands to the base window drawing cairo
  // context
  fn_cache_surface = fnCache;
  fn_base_surface = fnCache;
  fn_base_surface(context);

  is_processed = true;
  state_hash_code();
}

/**
\internal
\class function_object_t
\internal
\brief call previously bound function that was supplied parameter
 using std::bind with the cairo context.

\details

*/
void uxdevice::function_object_t::emit(cairo_t *cr) { value(cr); }

/**

\class option_function_object_t
\brief

\details


 */
void uxdevice::cairo_option_function_t::emit(cairo_t *cr) {
  //  value(context.cr);
}

/**
\internal
\brief
*/
void uxdevice::draw_function_object_t::emit(display_context_t &context) {
  using namespace std::placeholders;

  // get the drawing options for the context.
  auto options = context.unit_memory<cairo_option_function_t>();

  // the cache functions
  auto fnCache = [=](display_context_t &context) {
    // set directly callable rendering function.
    auto fn = [=](display_context_t &context) {
      drawing_output_t::emit(context);
      value(context.cr);
    };

    auto fnClipping = [=](display_context_t &context) {
      drawing_output_t::emit(context);

      cairo_rectangle(context.cr, intersection_double.x, intersection_double.y,
                      intersection_double.width, intersection_double.height);
      cairo_clip(context.cr);
      value(context.cr);
      cairo_reset_clip(context.cr);
      evaluate_cache(context);
    };

    functors_lock(true);
    fn_draw = std::bind(fn, _1);
    fn_draw_clipped = std::bind(fnClipping, _1);
    functors_lock(false);
    bRenderBufferCached = true;
  };

  // two function provide mode switching for the rendering.
  // a cache surface is a new xcb surface that can be threaded in creation
  // base surface issues the drawing commands to the base window drawing cairo
  // context
  fn_cache_surface = fnCache;
  fn_base_surface = fnCache;
  fn_base_surface(context);

  is_processed = true;
  state_hash_code();
}

/**

\class antialias_t
\brief

\details


*/
void uxdevice::antialias_t::emit(cairo_t *cr) {
  cairo_set_antialias(cr, static_cast<cairo_antialias_t>(value));
}

/**

\class line_width_t
\brief sets the line width when used during a stroke path operation.
This includes text and line drawing.

\details


 */
void uxdevice::line_width_t::emit(cairo_t *cr) {
  cairo_set_line_width(cr, value);
}

/**

\class line_cap_t
\brief

\details


*/
void uxdevice::line_cap_t::emit(cairo_t *cr) {
  cairo_set_line_cap(cr, static_cast<cairo_line_cap_t>(value));
}

/**

\class line_join_t
\brief

\details


*/
void uxdevice::line_join_t::emit(cairo_t *cr) {
  cairo_set_line_join(cr, static_cast<cairo_line_join_t>(value));
}
/**

\class miter_limit_t
\brief

\details


 */
void uxdevice::miter_limit_t::emit(cairo_t *cr) {
  cairo_set_miter_limit(cr, value);
}
/**

\class line_dash_storage_t - friend of line_deshes_t
\brief

\details


 */
void uxdevice::line_dashes_t::emit(cairo_t *cr) {
  cairo_set_dash(cr, value.data(), value.size(), offset);
}

/**

\class tollerance_t
\brief

\details


 */
void uxdevice::tollerance_t::emit(cairo_t *cr) {
  cairo_set_tolerance(cr, value);
}

/**

\class graphic_operator
\brief

\details


*/
void uxdevice::graphic_operator_t::emit(cairo_t *cr) {
  cairo_set_operator(cr, static_cast<cairo_operator_t>(value));
}

/**

\class arc_storage_t
\brief

\details


*/
void uxdevice::arc_t::emit(cairo_t *cr) {
  cairo_arc(cr, xc, yc, radius, angle1, angle2);
}

/**

\class negative_arc_t
\brief

\details


 */
void uxdevice::negative_arc_t::emit(cairo_t *cr) {
  cairo_arc_negative(cr, xc, yc, radius, angle1, angle2);
}

/**

\class curve_storage_t
\brief

\details


 */
void uxdevice::curve_t::emit_relative(cairo_t *cr) {
  cairo_rel_curve_to(cr, x1, y1, x2, y3, x3, y3);
}

void uxdevice::curve_t::emit_absolute(cairo_t *cr) {
  cairo_curve_to(cr, x1, y1, x2, y2, x3, y3);
}

/**

\class line_t
\brief

\details


 */
void uxdevice::line_t::emit_relative(cairo_t *cr) {
  cairo_rel_line_to(cr, x, y);
}

void uxdevice::line_t::emit_absolute(cairo_t *cr) { cairo_line_to(cr, x, y); }

/**

\class hline_t
\brief

\details


 */

void uxdevice::hline_t::emit_relative(cairo_t *cr) {
  if (cairo_has_current_point(cr)) {
    double curx = 0.0, cury = 0.0;
    cairo_get_current_point(cr, &curx, &cury);
    cairo_rel_line_to(cr, value, 0);
  }
}

void uxdevice::hline_t::emit_absolute(cairo_t *cr) {

  if (cairo_has_current_point(cr)) {
    double curx = 0.0, cury = 0.0;
    cairo_get_current_point(cr, &curx, &cury);
    cairo_line_to(cr, value, cury);
  }
}
/**

\class vline_t
\brief

\details


 */

void uxdevice::vline_t::emit_relative(cairo_t *cr) {
  if (cairo_has_current_point(cr)) {
    double curx = 0.0, cury = 0.0;
    cairo_get_current_point(cr, &curx, &cury);
    cairo_rel_line_to(cr, 0, value);
  }
}

void uxdevice::vline_t::emit_absolute(cairo_t *cr) {

  if (cairo_has_current_point(cr)) {
    double curx = 0.0, cury = 0.0;
    cairo_get_current_point(cr, &curx, &cury);
    cairo_line_to(cr, curx, value);
  }
}
/**

\class rectangle_t
\brief

\details


 */
void uxdevice::rectangle_t::emit(cairo_t *cr) {
  cairo_rectangle(cr, x, y, width, height);
}
/**

\class close_path_t
\brief

\details


 */
void uxdevice::close_path_t::emit(cairo_t *cr) { cairo_close_path(cr); }

/**

\class stroke_path_t
\brief

\details


*/
void uxdevice::stroke_path_t::emit(cairo_t *cr) {
  painter_brush_t::emit(cr);
  cairo_stroke(cr);
}

/**

\class fill_path_t
\brief

\details


 */
void uxdevice::fill_path_t::emit(cairo_t *cr) {
  painter_brush_t::emit(cr);
  cairo_fill(cr);
}

/**

\class stroke_fill_path_storage_t
\brief

\details


 */
void uxdevice::stroke_fill_path_t::emit(cairo_t *cr) {
  stroke_brush.emit(cr);
  cairo_stroke_preserve(cr);
  fill_brush.emit(cr);
  cairo_fill(cr);
}

/**

\class mask_t
\brief

\details


 */
void uxdevice::mask_t::emit(cairo_t *cr) {}

/**

\class paint_t
\brief

\details


 */
void uxdevice::paint_t::emit(cairo_t *cr) {
  if (value == 1.0) {
    cairo_paint(cr);
  } else {
    cairo_paint_with_alpha(cr, value);
  }
}
/**

\class relative_coordinate_t
\brief

\details


 */
void uxdevice::relative_coordinate_t::emit(display_context_t &context) {
  context.relative_coordinate = true;
}

/**

\class absolute_coordinate_t
\brief

\details


 */
void uxdevice::absolute_coordinate_t::emit(display_context_t &context) {
  context.relative_coordinate = false;
}
