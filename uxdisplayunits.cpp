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

void uxdevice::text_render_fast_t::invoke(display_context_t &context) {
  context.unit_memory<text_rendering_path_t>(false);
}

void uxdevice::text_render_path_t::invoke(display_context_t &context) {
  context.unit_memory<text_rendering_path_t>(true);
}

/**

\fn surface_brush(painter_brush_t &b)
\param painter_brush_t &b

\brief Sets the background brush of the surface window.

\details WHen rendering occurs, this is painted as the background.
For flexibility, the background may be a color, gradient or image.
The painter_brush_t object interface is used.


 */
DECLARE_HASH_MEMBERS_IMPLEMENTATION(
    textual_render_storage_t, HASH_TYPE_ID_THIS,
    pango_layout_get_serial(layout), ink_rect.x, ink_rect.y, ink_rect.width,
    ink_rect.height, matrix.hash_code(),
    rendering_parameter<text_color_t>()->hash_code(),
    rendering_parameter<text_outline_t>()->hash_code(),
    rendering_parameter<text_fill_t>()->hash_code(),
    rendering_parameter<text_shadow_t>()->hash_code(),
    rendering_parameter<text_alignment_t>()->hash_code(),
    rendering_parameter<text_indent_t>()->hash_code(),
    rendering_parameter<text_ellipsize_t>()->hash_code(),
    rendering_parameter<text_line_space_t>()->hash_code(),
    rendering_parameter<text_tab_stops_t>()->hash_code(),
    rendering_parameter<text_font_t>()->hash_code(),
    rendering_parameter<text_data_t>()->hash_code(),
    rendering_parameter<coordnates_t>()->hash_code(),
    rendering_parameter<antialias_t>()->hash_code(),
    rendering_parameter<line_width_t>()->hash_code(),
    rendering_parameter<line_cap_t>()->hash_code(),
    rendering_parameter<line_join_t>()->hash_code(),
    rendering_parameter<miter_limit_t>()->hash_code(),
    rendering_parameter<line_dashes_t>()->hash_code(),
    rendering_parameter<tollerance_t>()->hash_code(),
    rendering_parameter<graphic_operator_t>()->hash_code())

DECLARE_HASH_MEMBERS_IMPLEMENTATION(image_block_storage_t, HASH_TYPE_ID_THIS,
                                    is_SVG,
                                    coordinates ? coordinates->hash_code() : 0)

/**

\fn surface_brush(painter_brush_t &b)
\param painter_brush_t &b

\brief Sets the background brush of the surface window.

\details WHen rendering occurs, this is painted as the background.
For flexibility, the background may be a color, gradient or image.
The painter_brush_t object interface is used.


 */
surface_area_t &
uxdevice::surface_area_brush_t::invoke(display_context_t &context) {
  context.unit_memory<surface_area_brush_t>(shared_from_this());
  context.surface_brush(b);
  return *this;
  `
}
void uxdevice::surface_area_brush_t::emit(display_context_t &context) {
  value.emit(context.cr);
}

surface_area_title_t

    /**

    \class text_color_t
    \brief controls the color of text

    \details The text_outline_t and text_fill_t when present take precedent over
    this class. The text_fill_off_t,text_outline_off_t,text_shadow_off_t can be
    used.

    */
    void
    text_color_t::invoke(display_context_t &context) {
  context.unit_memory<text_color_t>(shared_from_this());
}
void text_color_t::emit(display_context_t &context) { value.emit(context.cr); }
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

void text_outline_t::invoke(display_context_t &context) {
  context.unit_memory<text_outline_t>(shared_from_this());
}

void text_outline_t::emit(display_context_t &context) {
  value.emit(context.cr);
}
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
void text_fill_t::invoke(display_context_t &context) {
  context.unit_memory<text_fill_t>(shared_from_this());
}
void text_fill_t::emit(display_context_t &context) { value.emit(context.cr); }

/**

\class text_shadow_t
\brief controls the shadow, reflect the pattern source given.

\details


 */
void text_shadow_t::invoke(display_context_t &context) {
  context.unit_memory<text_shadow_t>(shared_from_this());
}
void text_shadow_t::emit(display_context_t &context) { value.emit(context.cr); }

/**

\class text_alignment_t
\brief sets the alignment mode when wrapping, left, right, center,
justified.

\details


 */
void text_alignment_t::invoke(display_context_t &context {
  context.unit_memory<text_alignment_t>(shared_from_this());
}

void text_alignment_t::emit(PangoLayout *layout) {
  PangoAlignment &correlated_value = static_cast<PangoAlignment &>(value);
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
 void coordinates_t::invoke(display_context_t &context {
  context.unit_memory<coordinates_t>(shared_from_this());
}
void coordinates_t::emit(display_context_t &context) {
  cairo_move_to(context.cr, value.x, value.y);
}

/**

\class text_indent_t
\brief

\details


 */
void text_indent_t::invoke(display_context_t &context {
  context.unit_memory<text_indent_t>(shared_from_this());
}
void text_indent_t::emit(PangoLayout *layout) {
  int pangoUnits = value * PANGO_SCALE;
  pango_layout_set_indent(layout, pangoUnits);
}

/**

\class text_ellipsize_t
\brief

\details


 */
void text_ellipsize_t::invoke(display_context_t &context {
  context.unit_memory<text_ellipsize_t>(shared_from_this());
}
void text_ellipsize_t::emit(PangoLayout *layout) {
  PangoEllipsizeMode &correlated_value =
      static_cast<PangoEllipsizeMode &>(value);
  pango_layout_settext_ellipsize(layout, correlated_value);
}

/**

\class text_line_space_t
\brief

\details


 */
void text_line_space_t::invoke(display_context_t &context {
  context.unit_memory<text_line_space_t>(shared_from_this());
}
void text_line_space_t::emit(PangoLayout *layout) {
  pango_layout_set_line_spacing(layout, static_cast<float>(value));
}

/**

\class text_tab_stops_t
\brief

\details


*/
void text_tab_stops_t::invoke(display_context_t &context {
  context.unit_memory<text_tab_stops_t>(shared_from_this());
}

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

\class text_font_t
\brief

\details


 */
void text_font_t::invoke(display_context_t &context {
  context.unit_memory<text_font_t>(shared_from_this());
}

void text_font_t::emit(display_context_t &context) {
  value.emit(context);
}


/**
\internal
\class text_data_t
\brief class holds the data to the information to display.

\details


 */
void text_data_t::invoke(display_context_t &context) {
  context.unit_memory<text_data_t>(shared_from_this());
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
                                    text_font->fontDescription))
    pango_layout_set_font_description(layout, text_font->fontDescription);

  if (text_alignment) {
    text_alignment->emit(layout);
  }

  // set the width and height of the layout.
  if (pango_layout_get_width(layout) != coordinates->w * PANGO_SCALE)
    pango_layout_set_width(layout, coordinates->w * PANGO_SCALE);

  if (pango_layout_get_height(layout) != coordinates->h * PANGO_SCALE)
    pango_layout_set_height(layout, coordinates->h * PANGO_SCALE);

  std::string_view sinternal = std::string_view(pango_layout_get_text(layout));
  if (std::get<std::string>(text->_data).compare(sinternal) != 0)
    pango_layout_set_text(_layout, std::get<std::string>(_text->_data).data(),
                          -1);

  // any changes
  if (layoutSerial != pango_layout_get_serial(_layout)) {
    pango_layout_get_pixel_extents(_layout, &_ink_rect, &_logical_rect);
    int tw = std::min((double)_logical_rect.width, coordinates->w);
    int th = std::min((double)_logical_rect.height, coordinates->h);
    ink_rectangle = {(int)coordinates->x, (int)coordinates->y, tw, th};
    _ink_rectangle = {(double)ink_rectangle.x, (double)ink_rectangle.y,
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
    auto text_shadow = rendering_parameter<text_shadow_t>();
    shadow_image = cairo_image_surface_create(
        CAIRO_FORMAT_ARGB32, ink_rectangle.width + text_shadow->x,
        _ink_rectangle.height + text_shadow->y);
    shadow_cr = cairo_create(shadow_image);
    // offset text by the parameter amounts
    cairo_move_to(shadow_cr, text_shadow->x, text_shadow->y);
    if (set_layout_options(_shadow_cr))
      pango_cairo_update_layout(shadow_cr, layout);
    text_shadow->emit(cr, a);
    pango_cairo_show_layout(shadow_cr, layout);

#if defined(USE_STACKBLUR)
    blur_image(_shadow_image, _text_shadow_t->radius);

#elif defined(USE_SVGREN)
    cairo_surface_t *blurred =
        blur_image(_shadow_image, _text_shadow_t->radius);
    cairo_surface_destroy(_shadow_image);
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
internal_cairo_function_t uxdevice::textual_render_storage_t::precise_rendering_function(void) {
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

  if (context.unit_memory<text_rendering_path_t>()) {
    if (rendering_parameter<text_fill_t>() &&
        rendering_parameter<text_outline_t>() &&
        rendering_parameter<text_shadow>())
      text_render_type = text_rendering_fill_outline_shadowed_lambda;

    else if (rendering_parameter<text_fill_t>() &&
             rendering_parameter<text_outline_t>())
      text_render_type = text_rendering_fill_outline_lambda;

    else if (rendering_parameter<text_fill_t> &&
             rendering_parameter<text_shadow_t>())
      text_render_type = text_rendering_fill_shadowed_lambda;

    else if (rendering_parameter<text_outline_t>() &&
             rendering_parameter<text_shadow_t>())
      text_render_type = text_rendering_outline_shadowed_lambda;

    else if (rendering_parameter<text_fill_t>())
      text_render_type = text_rendering_fill__lambda;

    else if (rendering_parameter<text_outline_t>())
      text_render_type = text_rendering_outline_lambda;
  } else {
    if (rendering_parameter<text_color_t>() &&
        rendering_parameter<text_shadow_t>())
      text_render_type = text_rendering_fast_shadowed_lambda;

    else if (rendering_parameter<text_color_t>())
      text_render_type = text_rendering_fast_lambda;
  }

#define FN_SHADOW                                                              \
  create_shadow();                                                             \
  cairo_set_source_surface(context.cr, shadow_image_ptr, a.x, a.y);            \
  cairo_rectangle(context.cr, a.x, a.y, a.w, a.h);                             \
  cairo_fill(context.cr);

  // set the drawing function to the one that will be used by the rendering
  // options for text. These functions accept five parameters.
  // These are the clipping versions that have coordinates_t.

  switch (text_render_type) {
  case text_rendering_default:
  case text_rendering_fast_lambda: {
    fn = [=](cairo_t *cr, coordinates_t a) {
      // cairo_set_matrix(context.cr, &mat._matrix);
      drawing_output_t::invoke(cr);

      a.emit(cr);
      rendering_parameter<text_color_t>()->emit(cr, a);
      pango_cairo_show_layout(cr, layout);
    };
  } break;

  case text_rendering_fast_shadowed_lambda: {
    fn = [=](cairo_t *cr, coordinates_t a) {
      // cairo_set_matrix(context.cr, &mat._matrix);

      drawing_output_t::invoke(cr);
      if (set_layout_options(cr))
        pango_cairo_update_layout(cr, _layout);

      FN_SHADOW

      a.emit(cr);
      rendering_parameter<text_color_t>()->emit(cr, a);
      pango_cairo_show_layout(cr, _layout);
    };

  } break;

  case text_rendering_fill_lambda: {
    fn = [=](cairo_t *cr, coordinates_t a) {
      // cairo_set_matrix(context.cr, &mat._matrix);
      drawing_output_t::invoke(cr);

      if (set_layout_options(cr))
        pango_cairo_update_layout(cr, layout);
      a.emit(cr);

      pango_cairo_layout_path(cr, layout);
      rendering_parameter<text_fill_t>()->emit(cr, a);
      cairo_fill(context.cr);
    };

  } break;

  case text_rendering_outline_lambda: {
    fn = [=](cairo_t *cr, coordinates_t a) {
      // cairo_set_matrix(context.cr, &mat._matrix);
      drawing_output_t::invoke(cr);

      if (set_layout_options(cr))
        pango_cairo_update_layout(cr, layout);

      a.emit(context);
      pango_cairo_layout_path(cr, layout);
      rendering_parameter<text_outline_t>()->emit(cr, a);
      cairo_stroke(cr);
    };
  } break;

  case text_rendering_fill_outline_lambda: {
    fn = [=](cairo_t *cr, coordinates_t a) {
      // cairo_set_matrix(context.cr, &mat._matrix);
      drawing_output_t::invoke(cr);

      if (set_layout_options(context))
        pango_cairo_update_layout(cr, layout);

      a.emit(context);
      pango_cairo_layout_path(context, layout);
      rendering_parameter<text_fill_t>()->emit(cr, a);
      cairo_fill_preserve(cr);
      rendering_parameter<text_outline_t>()->emit(cr, a);
      cairo_stroke(context.cr);
    };
  } break;

  case text_rendering_fill_shadowed_lambda: {
    fn = [=](cairo_t *cr, coordinates_t a) {
      // cairo_set_matrix(context.cr, &mat._matrix);
      drawing_output_t::invoke(cr);

      if (set_layout_options(cr))
        pango_cairo_update_layout(cr, layout);

      FN_SHADOW

      a.emit(context);
      pango_cairo_layout_path(cr, layout);
      rendering_parameter<text_fill_t>()->emit(cr, a);
      cairo_fill(cr);
    };
  } break;

  case text_rendering_outline_shadowed_lambda: {
    fn = [=](cairo_t *cr, coordinates_t a) {
      // cairo_set_matrix(context.cr, &mat._matrix);
      drawing_output_t::invoke(cr);
      if (set_layout_options(cr))
        pango_cairo_update_layout(cr, _layout);

      FN_SHADOW

      a.emit(cr);
      pango_cairo_layout_path(cr, _layout);
      rendering_parameter<text_outline_t>()->emit(cr, a);
      cairo_stroke(cr);
    };
  } break;

  case text_rendering_fill_outline_shadowed_lambda: {

    fn = [=](cairo_t *cr, coordinates_t a) {
      // cairo_set_matrix(context.cr, &mat._matrix);
      drawing_output_t::invoke(cr);

      if (set_layout_options(cr))
        pango_cairo_update_layout(cr, _layout);

      FN_SHADOW

      a.emit(cr);
      pango_cairo_layout_path(cr, _layout);
      rendering_parameter<text_fill_t>()->emit(cr, a);
      cairo_fill_preserve(cr);
      rendering_parameter<text_outline_t>()->emit(cr, a);
      cairo_stroke(context.cr);
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
void uxdevice::textual_render_storage_t::invoke(display_context_t &context) {
  using namespace std::placeholders;

  // create a linkage snapshot to the shared pointer
  // within the stream context.
  rendering_parameter_storage = context.unit_memory_storage;

  // check the context parameters before operating
  if (!((rendering_parameter<text_color_t>() ||
         rendering_parameter<text_outline_t>() ||
         rendering_parameter<text_fill_t>()) &&
        rendering_parameter<coordinates_t>() &&
        rendering_parameter<text_data_t>() &&
        rendering_parameter<text_font>())) {
    const char *s =
        "A draw text object must include the following "
        "attributes. A text_color_t or a text_outline_t or "
        " text_fill_t. As well, a coordinates_t, text and text_font_t object.";
    ERROR_DRAW_PARAM(s);
    auto fn = [=](display_context_t &context) {};

    fn_base_surface = std::bind(fn, _1);
    fn_cache_surface = std::bind(fn, _1);
    fn_draw = std::bind(fn, _1);
    fn_draw_clipped = std::bind(fn, _1);
    return;
  }

  // cairo_get_matrix(context.cr, &mat._matrix);

  fn = precise_drawing_function();

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
    ERROR_CHECK(context).cr;
    context.lock(false);

    internal_buffer =
        context.allocate_buffer(ink_rectangle.width, ink_rectangle.height);

    set_layout_options(internal_buffer.cr);
    ERROR_CHECK(internal_buffer.cr);

    coordinates_t a = *coordinates;
#if 0
    if(textfill)
      textfill->translate(-a.x,-a.y);
    if(textoutline)
      textoutline->translate(-a.x,-a.y);
#endif // 0
    a.x = 0;
    a.y = 0;

    fn(internal_buffer.cr, a);
    ERROR_CHECK(internal_buffer.cr);

    cairo_surface_flush(internal_buffer.rendered);
    ERROR_CHECK(internal_buffer.rendered);

    auto drawfn = [=](display_context_t &context) {
      // cairo_set_matrix(context.cr, &mat._matrix);
      drawing_output_t::invoke(context.cr);
      auto &coordinate = rendering_parameter<coordinate_t>();

      cairo_set_source_surface(context.cr, internal_buffer.rendered,
                               coordinate.value.x,
                               rendering_parameter<coordinate_t>()->value.y);
      double tw, th;
      tw = std::min(_ink_rectangle.width, coordinates->w);
      th = std::min(_ink_rectangle.height, coordinates->h);

      cairo_rectangle(context.cr, _ink_rectangle.x, _ink_rectangle.y, tw, th);
      cairo_fill(context.cr);
    };
    auto fnClipping = [=](display_context_t &context) {
      // cairo_set_matrix(context.cr, &mat._matrix);
      drawing_output_t::invoke(context.cr);
      cairo_set_source_surface(context.cr, internal_buffer.rendered,
                               coordinates->x, coordinates->y);
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
      drawing_output_t::invoke(context.cr);
      fn(context.cr, *coordinates);
      evaluate_cache(context);
    };
    auto fnClipping = [=](display_context_t &context) {
      cairo_rectangle(context.cr, intersection_double.x, intersection_double.y,
                      intersection_double.width, intersection_double.height);
      cairo_clip(context.cr);
      drawing_output_t::invoke(context.cr);
      fn(context.cr, *coordinates);
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



\
/**
\internal
\brief reads the image_block_t and creates a cairo surface image_block_t.
*/
void uxdevice::image_block_storage_t::invoke(display_context_t &context) {
  using namespace std::placeholders;

  if (is_loaded)
    return;

  auto coordinates = context.unit_memory<coordinates_t>();
  auto options = context.unit_memory<drawing_options_fn_t>();

  if (!(coordinates && value.size())) {
    const char *s = "An image_block_t object must include the following "
                    "attributes. coordinates_t and an image_block_t name.";
    ERROR_DRAW_PARAM(s);
    auto fn = [=](display_context_t &context) {};

    fn_base_surface = std::bind(fn, _1);
    fn_cache_surface = std::bind(fn, _1);
    fn_draw = std::bind(fn, _1);
    fn_draw_clipped = std::bind(fn, _1);
    return;
  }

  // set the ink area.
  coordinates_t &a = *coordinates;

  auto fnthread = [=, &context, &a]() {
    image_block_ptr = read_image(value, coordinates->w, coordinates->h);

    if (image_block_ptr) {

      ink_rectangle = {(int)a.x, (int)a.y, (int)a.w, (int)a.h};
      _ink_rectangle = {(double)ink_rectangle.x, (double)ink_rectangle.y,
                        (double)ink_rectangle.width,
                        (double)ink_rectangle.height};
      has_ink_extents = true;
      is_loaded = true;
    } else {
      const char *s = "The image_block_t could not be processed or loaded. ";
      ERROR_DRAW_PARAM(s);
      ERROR_DESC(value);
    }
  };

  fnthread();

  auto fnCache = [&](display_context_t &context) {
    // set directly callable rendering function.
    auto fn = [&](display_context_t &context) {
      if (!is_valid())
        return;
      drawing_output_t::invoke(context.cr);
      cairo_set_source_surface(context.cr, image_block_ptr, a.x, a.y);
      cairo_rectangle(context.cr, a.x, a.y, a.w, a.h);
      cairo_fill(context.cr);
    };
    auto fnClipping = [&](display_context_t &context) {
      if (!is_valid())
        return;
      drawing_output_t::invoke(context.cr);
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
\brief reads the image_block_t and creates a cairo surface image_block_t.
*/
bool uxdevice::image_block_storage_t::is_valid(void) {
  return is_loaded; }

/**
\internal
\class function_object_t
\internal
\brief call previously bound function that was supplied parameter
 using std::bind with the cairo context.

\details

*/
void function_object_t::invoke(display_context_t &context) {
  value(context.cr);
}

/**

\class option_function_object_t
\brief

\details


 */
void option_function_object_t::invoke(display_context_t &context) {
  value(context.cr);
}

/**
\internal
\brief
*/
void uxdevice::draw_function_object_t::invoke(display_context_t &context) {
  using namespace std::placeholders;

  auto options = context.unit_memory<drawing_options_fn>();
  // set the ink area.

  auto fnCache = [=](display_context_t &context) {
    // set directly callable rendering function.
    auto fn = [=](display_context_t &context) {
      drawing_output_t::invoke(context.cr);
      value(context.cr);
    }
  };
  auto fnClipping = [=](display_context_t &context) {
    drawing_output_t::invoke(context.cr);
    ;
    cairo_rectangle(context.cr, intersection_double.x, intersection_double.y,
                    intersection_double.width, intersection_double.height);
    cairo_clip(context.cr);
    value(context.cr);
    cairo_reset_clip(context.cr);
    evaluate_cache(context);
  }
};
functors_lock(true);
fn_draw = std::bind(fn, _1);
fn_draw_clipped = std::bind(fnClipping, _1);
functors_lock(false);
bRenderBufferCached = true;
}
;

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
void antialias_t::invoke(display_context_t &context {
  context.unit_memory<antialias_t>(shared_from_this());
}

void antialias_t::emit(display_context_t &context) {
  cairo_set_antialias(context.cr, static_cast<cairo_antialias_t>(value));
}

/**

\class line_width_t
\brief sets the line width when use during a stroke path operation.
This includes text and line drawing.

\details


 */
void line_width_t::invoke(display_context_t &context {
  context.unit_memory<line_width_t>(shared_from_this());
}

void line_width_t::emit(display_context_t &context) {
  cairo_set_line_width(context.cr, value);
}


/**

\class line_cap_t
\brief

\details


*/
void line_cap_t::invoke(display_context_t &context {
  context.unit_memory<line_cap_t>(shared_from_this());
}
void line_cap_t::emit(display_context_t &context) {
  cairo_set_line_cap_t(context.cr,
                       static_cast<cairo_line_cap_options_t>(value));
}


/**

\class line_join_t
\brief

\details


*/
void line_join_t::invoke(display_context_t &context {
  context.unit_memory<line_join_t>(shared_from_this());
}
void line_join_t::emit(display_context_t &context) {
  cairo_set_line_join_t(context.cr,
                        static_cast<cairo_line_join_options_t>(value));
}
/**

\class miter_limit_t
\brief

\details


 */
void miter_limit_t::invoke(display_context_t &context {
  context.unit_memory<miter_limit_t>(shared_from_this());
}
void miter_limit_t::emit(display_context_t &context) {
  cairo_set_miter_limit_t(context.cr, value);
}
/**

\class line_dashes_t
\brief

\details


 */
void line_dashes_t::invoke(display_context_t &context {
  context.unit_memory<line_dashes_t>(shared_from_this());
}
void line_dashes_t::emit(display_context_t &context) {
  cairo_set_dash(context.cr, std::get<0>(value).data(),
                 std::get<0>(value).size(), std::get<1>(value).data());
}


/**

\class tollerance_t
\brief

\details


 */
void tollerance_t::invoke(display_context_t &context {
  context.unit_memory<tollerance_t>(shared_from_this());
}
void tollerance_t::emit(display_context_t &context){
  cairo_set_tolerance(context.cr, value);
}

/**

\class graphic_operator
\brief

\details


*/
void graphic_operator_t::invoke(display_context_t &context {
  context.unit_memory<graphic_operator_t>(shared_from_this());
}
void graphic_operator_t::emit(display_context_t &context){
  cairo_set_operator(context.cr, static_cast<cairo_operator_t>(value));
}

/**

\class arc_t
\brief

\details


*/
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
void negative_arc_t::invoke(display_context_t &context) {
  cairo_arc_negative(context.cr, value.xc, value.yc, value.radius, value.angle1,
                     value.angle2);
}

/**

\class curve_storage_t
\brief

\details


 */
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
void rectangle_t::invoke(display_context_t &context) {
  cairo_rectangle(context.cr, value.x, value.y, value.width, value.height);
}

/**

\class close_path_t
\brief

\details


 */
void close_path_t::invoke(display_context_t &context) {
  cairo_close_path_t(context.cr);
}

/**

\class stroke_path_t
\brief

\details


*/
void stroke_path_t::invoke(display_context_t &context) {
  value.emit(context.cr);
  cairo_stroke(context.cr);
}

/**

\class fill_path_t
\brief

\details


 */
void fill_path_t::invoke(display_context_t &context) {
  value.emit(context.cr);
  cairo_fill(context.cr);
}

/**

\class stroke_fill_path_t
\brief

\details


 */
void stroke_path_t::invoke(display_context_t &context) {
  stroke_fill_path_storage_t value.stroke_brush.emit(context.cr);
  cairo_stroke_preserve(context.cr);
  value.fill_brush.emit(context.cr);
  cairo_fill(context.cr);
}

/**

\class mask_t
\brief

\details


 */
void mask_t::invoke(display_context_t &context) {}

/**

\class paint_t
\brief

\details


 */
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
void relative_coordinates_t::invoke(display_context_t &context) {
  context.relative_coordinates = true;
}

/**

\class absolute_coordinates_t
\brief

\details


 */
void absolute_coordinates_t::invoke(display_context_t &context) {
  context.relative_coordinates = false;
}

/**

\class listener_t
\brief

\details

 */
void listener_t::invoke(display_context_t &context) {}

/**

\class listen_paint_t
\brief

\details

 */

/**

\class listen_focus_t
\brief

\details

 */

/**

\class listen_blur_t
\brief

\details

 */

/**

\class listen_resize_t
\brief

\details

 */

/**

\class listen_keydown_t
\brief

\details

 */

/**

\class listen_keyup_t
\brief

\details

 */

/**

\class listen_keypress_t
\brief

\details

 */

/**

\class listen_mouseenter_t
\brief

\details

 */

/**

\class listen_mousemove_t
\brief

\details

 */

/**

\class listen_mousedown_t
\brief

\details

 */

/**

\class listen_mouseup_t
\brief

\details

 */

/**

\class listen_click_t
\brief

\details

 */

/**

\class listen_dblclick_t
\brief

\details

 */

/**

\class listen_contextmenu_t
\brief

\details

 */

/**

\class listen_wheel_t
\brief

\details

 */

/**

\class listen_mouseleave_t
\brief

\details

 */
