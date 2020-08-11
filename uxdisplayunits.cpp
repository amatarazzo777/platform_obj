/**
\file uxdisplayunits.cpp

\author Anthony Matarazzo

\date 5/12/20
\version 1.0

\brief The modules extends the uxdevice namespace. The objects
provided are the base objects for which the caller may instantiate to
draw. While most of these objects are parameters to drawing functions,
the implementation with in this file provides the functional logic.
All objects derive from the DisplayUnit class which contains the
virtual invoke method. Objects that provide drawing operations
can inherit from the DrawingOutput base class which enables visibility
query. As well, a particular note is that parameters that describe colors,
shading or texturing derive and publish the Paint class interface.

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

void uxdevice::DrawingOutput::invoke(cairo_t *cr) {

  for (auto &fn : options)
    fn->fnOption(cr);
  bprocessed = true;
}

void uxdevice::DrawingOutput::intersect(cairo_rectangle_t &r) {
  if (!hasInkExtents)
    return;
  cairo_rectangle_int_t rInt = {(int)r.x, (int)r.y, (int)r.width,
                                (int)r.height
                               };
  cairo_region_t *rectregion = cairo_region_create_rectangle(&rInt);
  cairo_rectangle_int_t objrect = {inkRectangle.x, inkRectangle.y,
                                   inkRectangle.width, inkRectangle.height
                                  };

  overlap = cairo_region_contains_rectangle(rectregion, &objrect);
  if (overlap == CAIRO_REGION_OVERLAP_PART) {
    cairo_region_t *dst = cairo_region_create_rectangle(&objrect);
    cairo_region_intersect(dst, rectregion);
    cairo_region_get_extents(dst, &intersection);
    _intersection = {(double)intersection.x, (double)intersection.y,
                     (double)intersection.width, (double)intersection.height
                    };
    cairo_region_destroy(dst);
  }

  cairo_region_destroy(rectregion);
}
void uxdevice::DrawingOutput::intersect(CairoRegion &rectregion) {
  if (!hasInkExtents)
    return;

  cairo_region_t *dst = cairo_region_create_rectangle(&inkRectangle);
  cairo_region_intersect(dst, rectregion._ptr);
  cairo_region_get_extents(dst, &intersection);
  _intersection = {(double)intersection.x, (double)intersection.y,
                   (double)intersection.width, (double)intersection.height
                  };
  cairo_region_destroy(dst);
}

void uxdevice::DrawingOutput::evaluate_cache(DisplayContext &context) {
  return;
  if (bRenderBufferCached) {
    lastRenderTime = std::chrono::high_resolution_clock::now();
    if (oncethread)
      oncethread.reset();
    return;
  }

  // evaluate Rendering from cache
  if (bFirstTimeRendered) {
    bFirstTimeRendered = false;

  } else if (!oncethread) {
    auto currentPoint = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> diff =
      currentPoint - lastRenderTime;
    // if rendering requests are for more than 2 frames
    bool useCache = diff.count() < context.cacheThreshold;
    if (useCache) {
      oncethread = std::make_unique<std::thread>(
      [=, &context]() {
        fnCacheSurface(context);
      });
      oncethread->detach();
    }
  }
  lastRenderTime = std::chrono::high_resolution_clock::now();
}

void uxdevice::OPTION_FUNCTION::invoke(DisplayContext &context) {
  auto optType = fnOption.target_type().hash_code();

  context.currentUnits._options.remove_if([=](auto &n) {
    return n->fnOption.target_type().hash_code() == optType;
  });

  context.currentUnits._options.emplace_back(this);
}

/**
\internal
\brief creates if need be and sets options that differ.
*/
bool uxdevice::TEXT_RENDER::set_layout_options(cairo_t *cr) {
  bool ret = false;

  // create layout
  if (!_layout)
    _layout = pango_cairo_create_layout(cr);

  guint layoutSerial = pango_layout_get_serial(_layout);

  const PangoFontDescription *originalDescription =
    pango_layout_get_font_description(_layout);
  if (!originalDescription ||
      !pango_font_description_equal(originalDescription, _text_font->fontDescription))
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
  if (_text->compare(sinternal) != 0)
    pango_layout_set_text(_layout, _text->data.data(), -1);

  // any changes
  if (layoutSerial != pango_layout_get_serial(_layout)) {
    pango_layout_get_pixel_extents(_layout, &_ink_rect, &_logical_rect);
    int tw = std::min((double)_logical_rect.width, _coordinates->w);
    int th = std::min((double)_logical_rect.height, _coordinates->h);
    inkRectangle = {(int)_coordinates->x, (int)_coordinates->y, tw, th};
    _inkRectangle = {(double)inkRectangle.x, (double)inkRectangle.y,
                     (double)inkRectangle.width, (double)inkRectangle.height
                    };

    hasInkExtents = true;
    ret = true;
  }

  return ret;
}

void uxdevice::TEXT_RENDER::create_shadow(void) {
  if (!_shadow_image) {
    _shadow_image = cairo_image_surface_create(
                    CAIRO_FORMAT_ARGB32, _inkRectangle.width + _text_shadow->x,
                    _inkRectangle.height + _text_shadow->y);
    _shadow_cr = cairo_create(_shadow_image);
    // offset text by the parameter amounts
    cairo_move_to(_shadow_cr, _text_shadow->x, _text_shadow->y);
    if (set_layout_options(_shadow_cr))
      pango_cairo_update_layout(_shadow_cr, _layout);
    _text_shadow->emit(_shadow_cr);

    pango_cairo_show_layout(_shadow_cr, _layout);

#if defined(USE_STACKBLUR)
    blurImage(_shadow_image, _text_shadow->radius);

#elif defined(USE_SVGREN)
    cairo_surface_t *blurred = blurImage(_shadow_image, _text_shadow->radius);
    cairo_surface_destroy(_shadow_image);
    _shadow_image = blurred;
#endif
  }
}

/**
\internal
\brief


*/
void uxdevice::TEXT_RENDER::setup_draw(DisplayContext &context) {
  using namespace std::placeholders;
  _source = context.currentUnits._source;
  _text_outline = context.currentUnits._text_outline;
  _text_fill = context.currentUnits._text_fill;
  _text_shadow = context.currentUnits._text_shadow;
  _coordinates = context.currentUnits._coordinates;
  _text = context.currentUnits._text;
  _text_font = context.currentUnits._text_font;
  _text_alignment = context.currentUnits._text_alignment;
  options = context.currentUnits._options;

  // check the context parameters before operating
  if (!((_source || _text_outline || _text_fill) && _coordinates && _text && _text_font)) {
    const char *s = "A draw text object must include the following "
                    "attributes. A source or a text_outline or "
                    " text_fill. As well, a coordinates, text and text_font object.";
    ERROR_DRAW_PARAM(s);
    auto fn = [=](DisplayContext &context) {};

    fnBaseSurface = std::bind(fn, _1);
    fnCacheSurface = std::bind(fn, _1);
    fnDraw = std::bind(fn, _1);
    fnDrawClipped = std::bind(fn, _1);
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
  cairo_get_matrix(context.cr, &mat._matrix);

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
        cairo_set_matrix(context.cr, &mat._matrix);

        DrawingOutput::invoke(cr);
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
        cairo_set_matrix(context.cr, &mat._matrix);
        DrawingOutput::invoke(cr);
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
        cairo_set_matrix(context.cr, &mat._matrix);
        DrawingOutput::invoke(cr);
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
      cairo_set_matrix(context.cr, &mat._matrix);
      DrawingOutput::invoke(cr);
      if (set_layout_options(cr))
        pango_cairo_update_layout(cr, _layout);
      fnShadow(cr, a);
      cairo_move_to(cr, a.x, a.y);
      _source->emit(cr, a.x, a.y, a.w, a.h);
      pango_cairo_show_layout(cr, _layout);
    };
  }

  auto fnCache = [=](DisplayContext &context) {
    // if the item is already cached, return.
    if (bRenderBufferCached)
      return;

    // create off screen buffer
    context.lock(true);
    set_layout_options(context.cr);
    ERROR_CHECK(context.cr);
    context.lock(false);

    _buf = context.allocateBuffer(_inkRectangle.width, _inkRectangle.height);

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

    auto drawfn = [=](DisplayContext &context) {
      cairo_set_matrix(context.cr, &mat._matrix);
      DrawingOutput::invoke(context.cr);
      cairo_set_source_surface(context.cr, _buf.rendered, _coordinates->x, _coordinates->y);
      double tw, th;
      tw = std::min(_inkRectangle.width, _coordinates->w);
      th = std::min(_inkRectangle.height, _coordinates->h);

      cairo_rectangle(context.cr, _inkRectangle.x, _inkRectangle.y, tw, th);
      cairo_fill(context.cr);
    };
    auto fnClipping = [=](DisplayContext &context) {
      cairo_set_matrix(context.cr, &mat._matrix);
      DrawingOutput::invoke(context.cr);
      cairo_set_source_surface(context.cr, _buf.rendered, _coordinates->x, _coordinates->y);
      cairo_rectangle(context.cr, _intersection.x, _intersection.y,
                      _intersection.width, _intersection.height);
      cairo_fill(context.cr);
    };
    functors_lock(true);
    fnDraw = std::bind(drawfn, _1);
    fnDrawClipped = std::bind(fnClipping, _1);
    functors_lock(false);
    bRenderBufferCached = true;
  };

  // two function provide mode switching for the rendering.
  // a cache surface is a new xcb surface that can be threaded in creation
  // base surface issues the drawing commands to the base window drawing cairo
  // context. base surface creation is not threaded.
  fnCacheSurface = fnCache;

  // the base option rendered contains two functions that rendering using the
  // cairo api to the base surface context. One is for clipping and one without.
  auto fnBase = [=](DisplayContext &context) {
    auto drawfn = [=](DisplayContext &context) {
      DrawingOutput::invoke(context.cr);
      fn(context.cr, *_coordinates);
      evaluate_cache(context);
    };
    auto fnClipping = [=](DisplayContext &context) {
      cairo_rectangle(context.cr, _intersection.x, _intersection.y,
                      _intersection.width, _intersection.height);
      cairo_clip(context.cr);
      DrawingOutput::invoke(context.cr);
      fn(context.cr, *_coordinates);
      cairo_reset_clip(context.cr);
      evaluate_cache(context);
    };
    functors_lock(true);
    fnDraw = std::bind(drawfn, _1);
    fnDrawClipped = std::bind(fnClipping, _1);
    functors_lock(false);
    if (bRenderBufferCached) {
      context.destroy_buffer(_buf);
      bRenderBufferCached = false;
    }
  };
  context.lock(true);
  set_layout_options(context.cr);
  context.lock(false);
  fnBaseSurface = fnBase;
  fnBaseSurface(context);

  bprocessed = true;
}

/**
\internal
\brief reads the image and creates a cairo surface image.
*/
void uxdevice::IMAGE::invoke(DisplayContext &context) {

  if (bLoaded)
    return;
  coordinates = context.current_units.coordinates;
  if (!coordinates) {
    const char *s = "An image requires an area size to be defined. ";
    ERROR_DRAW_PARAM(s);
    return;
  }

  auto fnthread = [=, &context]() {
    _image = readImage(_data, area->w, area->h);

    if (_image)
      bLoaded = true;
    else {
      const char *s = "The image could not be processed or loaded. ";
      ERROR_DRAW_PARAM(s);
      ERROR_DESC(_data);
    }
  };

  fnthread();

  using namespace std::placeholders;

  area = context.currentUnits.area;
  image = context.currentUnits.image;
  options = context.currentUnits.options;
  if (!(area && image && image->valid())) {
    const char *s = "A draw image object must include the following "
                    "attributes. A an area and an image.";
    ERROR_DRAW_PARAM(s);
    auto fn = [=](DisplayContext &context) {};

    fnBaseSurface = std::bind(fn, _1);
    fnCacheSurface = std::bind(fn, _1);
    fnDraw = std::bind(fn, _1);
    fnDrawClipped = std::bind(fn, _1);
    return;
  }
  // set the ink area.
  const coordinates &a = *coordinates;
  inkRectangle = {(int)a.x, (int)a.y, (int)a.w, (int)a.h};
  _inkRectangle = {(double)inkRectangle.x, (double)inkRectangle.y,
                   (double)inkRectangle.width, (double)inkRectangle.height
                  };
  hasInkExtents = true;
  auto fnCache = [=](DisplayContext &context) {
    // set directly callable rendering function.
    auto fn = [=](DisplayContext &context) {
      if (!image->valid())
        return;
      DrawingOutput::invoke(context.cr);
      cairo_set_source_surface(context.cr, image->_image, a.x, a.y);
      cairo_rectangle(context.cr, a.x, a.y, a.w, a.h);
      cairo_fill(context.cr);
    };
    auto fnClipping = [=](DisplayContext &context) {
      if (!image->valid())
        return;
      DrawingOutput::invoke(context.cr);
      cairo_set_source_surface(context.cr, image->_image, a.x, a.y);
      cairo_rectangle(context.cr, _intersection.x, _intersection.y,
                      _intersection.width, _intersection.height);
      cairo_fill(context.cr);
    };
    functors_lock(true);
    fnDraw = std::bind(fn, _1);
    fnDrawClipped = std::bind(fnClipping, _1);
    functors_lock(false);
    bRenderBufferCached = true;
  };

  // two function provide mode switching for the rendering.
  // a cache surface is a new xcb surface that can be threaded in creation
  // base surface issues the drawing commands to the base window drawing cairo
  // context
  fnCacheSurface = fnCache;
  fnBaseSurface = fnCache;
  fnBaseSurface(context);

  bprocessed = true;
}

/**
\internal
\brief
*/
void uxdevice::DRAW_FUNCTION::invoke(DisplayContext &context) {
  using namespace std::placeholders;

  options = context.currentUnits.options;

  // set the ink area.

  auto fnCache = [=](DisplayContext &context) {
    // set directly callable rendering function.
    auto fn = [=](DisplayContext &context) {
      DrawingOutput::invoke(context.cr);
      func(context.cr);
    };
    auto fnClipping = [=](DisplayContext &context) {
      DrawingOutput::invoke(context.cr);
      cairo_rectangle(context.cr, _intersection.x, _intersection.y,
                      _intersection.width, _intersection.height);
      cairo_clip(context.cr);
      func(context.cr);
      cairo_reset_clip(context.cr);
      evaluateCache(context);
    };
    functorsLock(true);
    fnDraw = std::bind(fn, _1);
    fnDrawClipped = std::bind(fnClipping, _1);
    functors_lock(false);
    bRenderBufferCached = true;
  };

  // two function provide mode switching for the rendering.
  // a cache surface is a new xcb surface that can be threaded in creation
  // base surface issues the drawing commands to the base window drawing cairo
  // context
  fnCacheSurface = fnCache;
  fnBaseSurface = fnCache;
  fnBaseSurface(context);

  bprocessed = true;
}
