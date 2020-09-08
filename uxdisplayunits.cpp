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
\file uxdisplay_unit_ts.cpp

\author Anthony Matarazzo

\date 5/12/20
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

// error check for this file
#ifdef ERROR_CHECK
#undef ERROR_CHECK
#endif // ERROR_CHECK

#define ERROR_CHECK(obj)                                                       \
  {                                                                            \
    cairo_status_t stat = context.error_check(obj);                            \
    if (stat)                                                                  \
      context.error_state(__func__, __LINE__, __FILE__, stat);                 \
  }
#define ERROR_DRAW_PARAM(s)                                                    \
  context.error_state(__func__, __LINE__, __FILE__, std::string_view(s));      \
  error(s);

#define ERROR_DESC(s)                                                          \
  context.error_state(__func__, __LINE__, __FILE__, std::string_view(s));

void uxdevice::drawing_output_t::invoke(cairo_t *cr) {

  for (auto &fn : options)
    fn->invoke(cr);
  is_processed = true;
  state_hash_code();
}

void uxdevice::drawing_output_t::intersect(cairo_rectangle_t &r) {
  if (!has_ink_extents)
    return;
  cairo_rectangle_int_t rInt = {(int)r.x, (int)r.y, (int)r.width,
                                (int)r.height};
  cairo_region_t *rectregion = cairo_region_create_rectangle(&rInt);
  cairo_rectangle_int_t objrect = {ink_rectangle.x, ink_rectangle.y,
                                   ink_rectangle.width, ink_rectangle.height};

  overlap = cairo_region_contains_rectangle(rectregion, &objrect);
  if (overlap == CAIRO_REGION_OVERLAP_PART) {
    cairo_region_t *dst = cairo_region_create_rectangle(&objrect);
    cairo_region_intersect(dst, rectregion);
    cairo_region_get_extents(dst, &intersection);
    intersection_double = {(double)intersection.x, (double)intersection.y,
                           (double)intersection.width,
                           (double)intersection.height};
    cairo_region_destroy(dst);
  }

  cairo_region_destroy(rectregion);
}
void uxdevice::drawing_output_t::intersect(context_cairo_region_t &rectregion) {
  if (!has_ink_extents)
    return;

  cairo_region_t *dst = cairo_region_create_rectangle(&ink_rectangle);
  cairo_region_intersect(dst, rectregion._ptr);
  cairo_region_get_extents(dst, &intersection);
  intersection_double = {(double)intersection.x, (double)intersection.y,
                         (double)intersection.width,
                         (double)intersection.height};
  cairo_region_destroy(dst);
}

void uxdevice::drawing_output_t::evaluate_cache(display_context_t &context) {
  return;
  if (bRenderBufferCached) {
    last_render_time = std::chrono::high_resolution_clock::now();
    if (oncethread)
      oncethread.reset();
    return;
  }

  // evaluate Rendering from cache
  if (first_time_rendered) {
    first_time_rendered = false;

  } else if (!oncethread) {
    auto currentPoint = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> diff =
        currentPoint - last_render_time;
    // if rendering requests are for more than 2 frames
    bool useCache = diff.count() < context.cache_threshold;
    if (useCache) {
      oncethread = std::make_unique<std::thread>(
          [=, &context]() { fn_cache_surface(context); });
      oncethread->detach();
    }
  }
  last_render_time = std::chrono::high_resolution_clock::now();
}

void uxdevice::option_function_object_t::invoke(display_context_t &context) {
  if (std::holds_alternative<cairo_function_t>(_data)) {
    auto &func = std::get<cairo_function_t>(_data);
    auto optType = func.target_type().hash_code();
    context.current_units.drawing_options_fn.remove_if([=](auto &n) {
      auto &funcTarget = std::get<cairo_function_t>(n->_data);
      return funcTarget.target_type().hash_code() == optType;
    });
    context.current_units.drawing_options_fn.emplace_back(this);
  }
  state_hash_code();
}

/**
\internal
\brief creates if need be and sets options that differ.
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

void uxdevice::textual_render_storage_t::create_shadow(void) {
  if (!shadow_image) {
    shadow_image = cairo_image_surface_create(
        CAIRO_FORMAT_ARGB32, ink_rectangle.width + text_shadow->x,
        _ink_rectangle.height + text_shadow->y);
    shadow_cr = cairo_create(shadow_image);
    // offset text by the parameter amounts
    cairo_move_to(shadow_cr, text_shadow->x, text_shadow->y);
    if (set_layout_options(_shadow_cr))
      pango_cairo_update_layout(shadow_cr, layout);
    text_shadow->emit(shadow_cr);

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

void uxdevice::textual_render_storage_t::invoke(display_context_t &context) {
  setup_draw(context);
}

/**
\internal
\brief


*/
void uxdevice::textual_render_storage_t::setup_draw(display_context_t &context) {
  using namespace std::placeholders;

  // create a linkage snapshot to the shared pointer
  // within the stream context.
  text_color = context.current_units.text_color;
  text_outline = context.current_units.text_outline;
  text_fill = context.current_units.text_fill;
  text_shadow = context.current_units.text_shadow;
  coordinates = context.current_units.coordinates;
  text = context.current_units.text;
  text_font = context.current_units._text_font;
  text_alignment = context.current_units.text_alignment;
  options = context.current_units.drawing_options_fn;

  // check the context parameters before operating
  if (!((text_color || text_outline || text_fill) && coordinates && text &&
        text_font)) {
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
  // not using the path layout is faster
  // these options change rendering and pango api usage
  bool bUsePathLayout = false;
  bool bOutline = false;
  bool bFilled = false;

  // if the text is drawn with an outline
  if (text_outline_t) {
    bUsePathLayout = true;
    bOutline = true;
  }
  // cairo_get_matrix(context.cr, &mat._matrix);

  // if the text is filled with a texture or gradient
  if (_text_fill_t) {
    bFilled = true;
    bUsePathLayout = true;
  }

  std::function<void(cairo_t * cr, coordinates_t & a)> fnShadow;
  std::function<void(cairo_t * cr, coordinates_t & a)> fn;

  if (_text_shadow_t) {
    fnShadow = [=](cairo_t *cr, coordinates_t &a) {
      create_shadow();
      cairo_set_source_surface(cr, _shadow_image, a.x, a.y);
      cairo_rectangle(cr, a.x, a.y, a.w, a.h);
      cairo_fill(cr);
    };
  } else {

    fnShadow = [=](cairo_t *cr, coordinates_t &a) {};
  }

  // set the drawing function
  if (bUsePathLayout) {
    // set the drawing function to the one that will be used by the rendering
    // options for text. These functions accept five parameters.
    // These are the clipping versions that have coordinates_t.
    if (bFilled && bOutline) {
      fn = [=](cairo_t *cr, coordinates_t a) {
        // cairo_set_matrix(context.cr, &mat._matrix);

        drawing_output_t::invoke(cr);
        if (set_layout_options(cr))
          pango_cairo_update_layout(cr, _layout);
        fnShadow(cr, a);
        cairo_move_to(cr, a.x, a.y);
        pango_cairo_layout_path(cr, _layout);
        _text_fill_t->emit(cr, a.x, a.y, a.w, a.h);
        cairo_fill_preserve(cr);
        text_outline_t->emit(cr, a.x, a.y, a.w, a.h);
        cairo_stroke(cr);
      };

      // text is only filled.
    } else if (bFilled) {
      fn = [=](cairo_t *cr, coordinates_t a) {
        // cairo_set_matrix(context.cr, &mat._matrix);
        drawing_output_t::invoke(cr);
        if (set_layout_options(cr))
          pango_cairo_update_layout(cr, _layout);
        fnShadow(cr, a);
        cairo_move_to(cr, a.x, a.y);
        pango_cairo_layout_path(cr, _layout);
        _text_fill_t->emit(cr, a.x, a.y, a.w, a.h);
        cairo_fill(cr);
      };

      // text is only outlined.
    } else if (bOutline) {
      fn = [=](cairo_t *cr, coordinates_t a) {
        // cairo_set_matrix(context.cr, &mat._matrix);
        drawing_output_t::invoke(cr);
        if (set_layout_options(cr))
          pango_cairo_update_layout(cr, _layout);
        fnShadow(cr, a);
        cairo_move_to(cr, a.x, a.y);
        pango_cairo_layout_path(cr, _layout);
        text_outline_t->emit(cr, a.x, a.y, a.w, a.h);
        cairo_stroke(cr);
      };
    }

  } else {

    // no outline or fill defined, therefore the pen is used.
    // fastest text display uses the function
    //  which uses the raster of the font system
    //        --- see pango_cairo_show_layout
    fn = [=](cairo_t *cr, coordinates_t a) {
      // cairo_set_matrix(context.cr, &mat._matrix);
      drawing_output_t::invoke(cr);
      if (set_layout_options(cr))
        pango_cairo_update_layout(cr, _layout);
      fnShadow(cr, a);
      cairo_move_to(cr, a.x, a.y);
      text_color->emit(cr, a.x, a.y, a.w, a.h);
      pango_cairo_show_layout(cr, _layout);
    };
  }

  auto fnCache = [=](display_context_t &context) {
    // if the item is already cached, return.
    if (bRenderBufferCached)
      return;

    // create off screen buffer
    context.lock(true);
    set_layout_options(context.cr);
    ERROR_CHECK(context.cr);
    context.lock(false);

    internal_buffer =
        context.allocate_buffer(_ink_rectangle.width, _ink_rectangle.height);

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
      cairo_set_source_surface(context.cr, internal_buffer.rendered,
                               coordinates->x, coordinates->y);
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

  // two function provide mode switching for the rendering.
  // a cache surface is a new xcb surface that can be threaded in creation
  // base surface issues the drawing commands to the base window drawing cairo
  // context. base surface creation is not threaded.
  fn_cache_surface = fnCache;

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

/**
\internal
\brief reads the image_block_t and creates a cairo surface image_block_t.
*/
void uxdevice::image_block_storage_t::invoke(display_context_t &context) {
  using namespace std::placeholders;

  if (is_loaded)
    return;

  coordinates = context.current_units.coordinates;
  options = context.current_units.drawing_options_fn;

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
bool uxdevice::image_block_storage_t::is_valid(void) { return is_loaded; }

/**
\internal
\brief
*/
void uxdevice::draw_function_object_t::invoke(display_context_t &context) {
  using namespace std::placeholders;

  options = context.current_units.drawing_options_fn;

  // set the ink area.

  auto fnCache = [=](display_context_t &context) {
    // set directly callable rendering function.
    auto fn = [=](display_context_t &context) {
      if (std::holds_alternative<cairo_function_t>(_data)) {
        drawing_output_t::invoke(context.cr);
        auto &func = std::get<cairo_function_t>(_data);
        func(context.cr);
      }
    };
    auto fnClipping = [=](display_context_t &context) {
      if (std::holds_alternative<cairo_function_t>(_data)) {

        drawing_output_t::invoke(context.cr);
        auto &func = std::get<cairo_function_t>(_data);
        cairo_rectangle(context.cr, intersection_double.x,
                        intersection_double.y, intersection_double.width,
                        intersection_double.height);
        cairo_clip(context.cr);
        func(context.cr);
        cairo_reset_clip(context.cr);
        evaluate_cache(context);
      }
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
