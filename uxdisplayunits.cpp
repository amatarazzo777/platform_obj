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
    _intersection = {(double)intersection.x, (double)intersection.y,
                     (double)intersection.width, (double)intersection.height};
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
  _intersection = {(double)intersection.x, (double)intersection.y,
                   (double)intersection.width, (double)intersection.height};
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
  if (std::holds_alternative<cairo_function>(_data)) {
    auto &func = std::get<cairo_function>(_data);
    auto optType = func.target_type().hash_code();
    context.current_units._options.remove_if([=](auto &n) {
      auto &funcTarget = std::get<cairo_function>(n->_data);
      return funcTarget.target_type().hash_code() == optType;
    });
    context.current_units._options.emplace_back(this);
  }
}

/**
\internal
\brief creates if need be and sets options that differ.
*/
bool uxdevice::textual_render::set_layout_options(cairo_t *cr) {
  bool ret = false;

  // create layout
  if (!_layout)
    _layout = pango_cairo_create_layout(cr);

  guint layoutSerial = pango_layout_get_serial(_layout);

  const PangoFontDescription *originalDescription =
      pango_layout_get_font_description(_layout);
  if (!originalDescription ||
      !pango_font_description_equal(originalDescription,
                                    _text_font->fontDescription))
    pango_layout_set_font_description(_layout, _text_font->fontDescription);

  if (_text_alignment) {
    _text_alignment->emit(_layout);
  }

  // set the width and height of the layout.
  if (pango_layout_get_width(_layout) != _coordinates->w * PANGO_SCALE)
    pango_layout_set_width(_layout, _coordinates->w * PANGO_SCALE);

  if (pango_layout_get_height(_layout) != _coordinates->h * PANGO_SCALE)
    pango_layout_set_height(_layout, _coordinates->h * PANGO_SCALE);

  std::string_view sinternal = std::string_view(pango_layout_get_text(_layout));
  if (std::get<std::string>(_text->_data).compare(sinternal) != 0)
    pango_layout_set_text(_layout, std::get<std::string>(_text->_data).data(),
                          -1);

  // any changes
  if (layoutSerial != pango_layout_get_serial(_layout)) {
    pango_layout_get_pixel_extents(_layout, &_ink_rect, &_logical_rect);
    int tw = std::min((double)_logical_rect.width, _coordinates->w);
    int th = std::min((double)_logical_rect.height, _coordinates->h);
    ink_rectangle = {(int)_coordinates->x, (int)_coordinates->y, tw, th};
    _ink_rectangle = {(double)ink_rectangle.x, (double)ink_rectangle.y,
                      (double)ink_rectangle.width,
                      (double)ink_rectangle.height};

    has_ink_extents = true;
    ret = true;
  }

  return ret;
}

void uxdevice::textual_render::create_shadow(void) {
  if (!_shadow_image) {
    _shadow_image = cairo_image_surface_create(
        CAIRO_FORMAT_ARGB32, _ink_rectangle.width + _text_shadow->x,
        _ink_rectangle.height + _text_shadow->y);
    _shadow_cr = cairo_create(_shadow_image);
    // offset text by the parameter amounts
    cairo_move_to(_shadow_cr, _text_shadow->x, _text_shadow->y);
    if (set_layout_options(_shadow_cr))
      pango_cairo_update_layout(_shadow_cr, _layout);
    _text_shadow->emit(_shadow_cr);

    pango_cairo_show_layout(_shadow_cr, _layout);

#if defined(USE_STACKBLUR)
    blur_image(_shadow_image, _text_shadow->radius);

#elif defined(USE_SVGREN)
    cairo_surface_t *blurred = blur_image(_shadow_image, _text_shadow->radius);
    cairo_surface_destroy(_shadow_image);
    _shadow_image = blurred;
#endif
  }
}

void uxdevice::textual_render::invoke(display_context_t &context) {
  setup_draw(context);
}

/**
\internal
\brief


*/
void uxdevice::textual_render::setup_draw(display_context_t &context) {
  using namespace std::placeholders;

  // create a linkage snapshot to the shared pointer
  // within the stream context.
  _text_color = context.current_units._text_color;
  _text_outline = context.current_units._text_outline;
  _text_fill = context.current_units._text_fill;
  _text_shadow = context.current_units._text_shadow;
  _coordinates = context.current_units._coordinates;
  _text = context.current_units._text;
  _text_font = context.current_units._text_font;
  _text_alignment = context.current_units._text_alignment;
  options = context.current_units._options;

  // check the context parameters before operating
  if (!((_text_color || _text_outline || _text_fill) && _coordinates && _text &&
        _text_font)) {
    const char *s =
        "A draw text object must include the following "
        "attributes. A text_color or a text_outline or "
        " text_fill. As well, a coordinates, text and text_font object.";
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
  if (_text_outline) {
    bUsePathLayout = true;
    bOutline = true;
  }
  // cairo_get_matrix(context.cr, &mat._matrix);

  // if the text is filled with a texture or gradient
  if (_text_fill) {
    bFilled = true;
    bUsePathLayout = true;
  }

  std::function<void(cairo_t * cr, coordinates & a)> fnShadow;
  std::function<void(cairo_t * cr, coordinates & a)> fn;

  if (_text_shadow) {
    fnShadow = [=](cairo_t *cr, coordinates &a) {
      create_shadow();
      cairo_set_source_surface(cr, _shadow_image, a.x, a.y);
      cairo_rectangle(cr, a.x, a.y, a.w, a.h);
      cairo_fill(cr);
    };
  } else {

    fnShadow = [=](cairo_t *cr, coordinates &a) {};
  }

  // set the drawing function
  if (bUsePathLayout) {
    // set the drawing function to the one that will be used by the rendering
    // options for text. These functions accept five parameters.
    // These are the clipping versions that have coordinates.
    if (bFilled && bOutline) {
      fn = [=](cairo_t *cr, coordinates a) {
        // cairo_set_matrix(context.cr, &mat._matrix);

        drawing_output_t::invoke(cr);
        if (set_layout_options(cr))
          pango_cairo_update_layout(cr, _layout);
        fnShadow(cr, a);
        cairo_move_to(cr, a.x, a.y);
        pango_cairo_layout_path(cr, _layout);
        _text_fill->emit(cr, a.x, a.y, a.w, a.h);
        cairo_fill_preserve(cr);
        _text_outline->emit(cr, a.x, a.y, a.w, a.h);
        cairo_stroke(cr);
      };

      // text is only filled.
    } else if (bFilled) {
      fn = [=](cairo_t *cr, coordinates a) {
        // cairo_set_matrix(context.cr, &mat._matrix);
        drawing_output_t::invoke(cr);
        if (set_layout_options(cr))
          pango_cairo_update_layout(cr, _layout);
        fnShadow(cr, a);
        cairo_move_to(cr, a.x, a.y);
        pango_cairo_layout_path(cr, _layout);
        _text_fill->emit(cr, a.x, a.y, a.w, a.h);
        cairo_fill(cr);
      };

      // text is only outlined.
    } else if (bOutline) {
      fn = [=](cairo_t *cr, coordinates a) {
        // cairo_set_matrix(context.cr, &mat._matrix);
        drawing_output_t::invoke(cr);
        if (set_layout_options(cr))
          pango_cairo_update_layout(cr, _layout);
        fnShadow(cr, a);
        cairo_move_to(cr, a.x, a.y);
        pango_cairo_layout_path(cr, _layout);
        _text_outline->emit(cr, a.x, a.y, a.w, a.h);
        cairo_stroke(cr);
      };
    }

  } else {

    // no outline or fill defined, therefore the pen is used.
    // fastest text display uses the function
    //  which uses the raster of the font system
    //        --- see pango_cairo_show_layout
    fn = [=](cairo_t *cr, coordinates a) {
      // cairo_set_matrix(context.cr, &mat._matrix);
      drawing_output_t::invoke(cr);
      if (set_layout_options(cr))
        pango_cairo_update_layout(cr, _layout);
      fnShadow(cr, a);
      cairo_move_to(cr, a.x, a.y);
      _text_color->emit(cr, a.x, a.y, a.w, a.h);
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

    _buf = context.allocate_buffer(_ink_rectangle.width, _ink_rectangle.height);

    set_layout_options(_buf.cr);
    ERROR_CHECK(_buf.cr);

    coordinates a = *_coordinates;
#if 0
    if(textfill)
      textfill->translate(-a.x,-a.y);
    if(textoutline)
      textoutline->translate(-a.x,-a.y);
#endif // 0
    a.x = 0;
    a.y = 0;

    fn(_buf.cr, a);
    ERROR_CHECK(_buf.cr);

    cairo_surface_flush(_buf.rendered);
    ERROR_CHECK(_buf.rendered);

    auto drawfn = [=](display_context_t &context) {
      // cairo_set_matrix(context.cr, &mat._matrix);
      drawing_output_t::invoke(context.cr);
      cairo_set_source_surface(context.cr, _buf.rendered, _coordinates->x,
                               _coordinates->y);
      double tw, th;
      tw = std::min(_ink_rectangle.width, _coordinates->w);
      th = std::min(_ink_rectangle.height, _coordinates->h);

      cairo_rectangle(context.cr, _ink_rectangle.x, _ink_rectangle.y, tw, th);
      cairo_fill(context.cr);
    };
    auto fnClipping = [=](display_context_t &context) {
      // cairo_set_matrix(context.cr, &mat._matrix);
      drawing_output_t::invoke(context.cr);
      cairo_set_source_surface(context.cr, _buf.rendered, _coordinates->x,
                               _coordinates->y);
      cairo_rectangle(context.cr, _intersection.x, _intersection.y,
                      _intersection.width, _intersection.height);
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
      fn(context.cr, *_coordinates);
      evaluate_cache(context);
    };
    auto fnClipping = [=](display_context_t &context) {
      cairo_rectangle(context.cr, _intersection.x, _intersection.y,
                      _intersection.width, _intersection.height);
      cairo_clip(context.cr);
      drawing_output_t::invoke(context.cr);
      fn(context.cr, *_coordinates);
      cairo_reset_clip(context.cr);
      evaluate_cache(context);
    };
    functors_lock(true);
    fn_draw = std::bind(drawfn, _1);
    fn_draw_clipped = std::bind(fnClipping, _1);
    functors_lock(false);
    if (bRenderBufferCached) {
      context.destroy_buffer(_buf);
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
\brief reads the image_block and creates a cairo surface image_block.
*/
void uxdevice::image_block::invoke(display_context_t &context) {
  using namespace std::placeholders;

  if (is_loaded)
    return;

  _coordinates = context.current_units._coordinates;
  options = context.current_units._options;

  if (!(_coordinates && value.size())) {
    const char *s = "An image_block object must include the following "
                    "attributes. coordinates and an image_block name.";
    ERROR_DRAW_PARAM(s);
    auto fn = [=](display_context_t &context) {};

    fn_base_surface = std::bind(fn, _1);
    fn_cache_surface = std::bind(fn, _1);
    fn_draw = std::bind(fn, _1);
    fn_draw_clipped = std::bind(fn, _1);
    return;
  }

  // set the ink area.
  coordinates &a = *_coordinates;

  auto fnthread = [=, &context, &a]() {
    _image_block = read_image(value, _coordinates->w, _coordinates->h);

    if (_image_block) {

      ink_rectangle = {(int)a.x, (int)a.y, (int)a.w, (int)a.h};
      _ink_rectangle = {(double)ink_rectangle.x, (double)ink_rectangle.y,
                        (double)ink_rectangle.width,
                        (double)ink_rectangle.height};
      has_ink_extents = true;
      is_loaded = true;
    } else {
      const char *s = "The image_block could not be processed or loaded. ";
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
      cairo_set_source_surface(context.cr, _image_block, a.x, a.y);
      cairo_rectangle(context.cr, a.x, a.y, a.w, a.h);
      cairo_fill(context.cr);
    };
    auto fnClipping = [&](display_context_t &context) {
      if (!is_valid())
        return;
      drawing_output_t::invoke(context.cr);
      cairo_set_source_surface(context.cr, _image_block, a.x, a.y);
      cairo_rectangle(context.cr, _intersection.x, _intersection.y,
                      _intersection.width, _intersection.height);
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
}

/**
\internal
\brief reads the image_block and creates a cairo surface image_block.
*/
bool uxdevice::image_block::is_valid(void) { return is_loaded; }

/**
\internal
\brief
*/
void uxdevice::DRAW_function_object_t::invoke(display_context_t &context) {
  using namespace std::placeholders;

  options = context.current_units._options;

  // set the ink area.

  auto fnCache = [=](display_context_t &context) {
    // set directly callable rendering function.
    auto fn = [=](display_context_t &context) {
      if (std::holds_alternative<cairo_function>(_data)) {
        drawing_output_t::invoke(context.cr);
        auto &func = std::get<cairo_function>(_data);
        func(context.cr);
      }
    };
    auto fnClipping = [=](display_context_t &context) {
      if (std::holds_alternative<cairo_function>(_data)) {

        drawing_output_t::invoke(context.cr);
        auto &func = std::get<cairo_function>(_data);
        cairo_rectangle(context.cr, _intersection.x, _intersection.y,
                        _intersection.width, _intersection.height);
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
}
