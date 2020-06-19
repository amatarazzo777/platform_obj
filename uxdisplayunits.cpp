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
    cairo_status_t stat = context.error_check(obj);                             \
    if (stat)                                                                  \
      context.error_state(__func__, __LINE__, __FILE__, stat);                  \
  }
#define ERROR_DRAW_PARAM(s)                                                    \
  context.error_state(__func__, __LINE__, __FILE__, std::string_view(s));       \
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

  context.currentUnits.options.remove_if([=](auto &n) {
    return n->fnOption.target_type().hash_code() == optType;
  });

  context.currentUnits.options.emplace_back(this);
}

/**
\internal
\brief emits the alignment setting within the pango layout object.
*/
void uxdevice::ALIGN::emit(PangoLayout *layout) {
  // only change setting if changed, this saves on unnecessary
  // layout context rendering internal to pango
  if (setting == alignment::justified && !pango_layout_get_justify(layout)) {
    pango_layout_set_justify(layout, true);
  } else if (static_cast<alignment>(pango_layout_get_alignment(layout)) !=
             setting) {
    pango_layout_set_justify(layout, false);
    pango_layout_set_alignment(layout, static_cast<PangoAlignment>(setting));
  }
}

/**
\internal
\brief creates if need be and sets options that differ.
*/
bool uxdevice::TEXT_RENDER::set_layout_options(cairo_t *cr) {
  bool ret = false;

  // create layout
  if (!layout)
    layout = pango_cairo_create_layout(cr);

  guint layoutSerial = pango_layout_get_serial(layout);

  const PangoFontDescription *originalDescription =
    pango_layout_get_font_description(layout);
  if (!originalDescription ||
      !pango_font_description_equal(originalDescription, font->fontDescription))
    pango_layout_set_font_description(layout, font->fontDescription);

  if (align) {
    align->emit(layout);
  }

  // set the width and height of the layout.
  if (pango_layout_get_width(layout) != a.w * PANGO_SCALE)
    pango_layout_set_width(layout, a.w * PANGO_SCALE);

  if (pango_layout_get_height(layout) != a.h * PANGO_SCALE)
    pango_layout_set_height(layout, a.h * PANGO_SCALE);

  std::string_view sinternal = std::string_view(pango_layout_get_text(layout));
  if (text->data.compare(sinternal) != 0)
    pango_layout_set_text(layout, text->data.data(), -1);

  // any changes
  if (layoutSerial != pango_layout_get_serial(layout)) {
    pango_layout_get_pixel_extents(layout, &ink_rect, &logical_rect);
    int tw = std::min((double)logical_rect.width, a.w);
    int th = std::min((double)logical_rect.height, a.h);
    inkRectangle = {(int)a.x, (int)a.y, tw, th};
    _inkRectangle = {(double)inkRectangle.x, (double)inkRectangle.y,
                     (double)inkRectangle.width, (double)inkRectangle.height
                    };

    hasInkExtents = true;
    ret = true;
  }

  return ret;
}

void uxdevice::TEXT_RENDER::create_shadow(void) {
  if (!shadowImage) {
    shadowImage = cairo_image_surface_create(
                    CAIRO_FORMAT_ARGB32, _inkRectangle.width + textshadow->x,
                    _inkRectangle.height + textshadow->y);
    shadowCr = cairo_create(shadowImage);
    // offset text by the parameter amounts
    cairo_move_to(shadowCr, textshadow->x, textshadow->y);
    if (set_layout_options(shadowCr))
      pango_cairo_update_layout(shadowCr, layout);
    textshadow->emit(shadowCr);

    pango_cairo_show_layout(shadowCr, layout);

#if defined(USE_STACKBLUR)
    blurImage(shadowImage, textshadow->radius);

#elif defined(USE_SVGREN)
    cairo_surface_t *blurred = blurImage(shadowImage, textshadow->radius);
    cairo_surface_destroy(shadowImage);
    shadowImage = blurred;
#endif
  }
}

/**
\internal
\brief


*/
void uxdevice::TEXT_RENDER::setup_draw(DisplayContext &context) {
  using namespace std::placeholders;

  source = context.currentUnits.source;
  textoutline = context.currentUnits.textoutline;
  textfill = context.currentUnits.textfill;
  textshadow = context.currentUnits.textshadow;
  textarea = context.currentUnits.textarea;
  text = context.currentUnits.text;
  font = context.currentUnits.font;
  align = context.currentUnits.align;
  options = context.currentUnits.options;

  // check the context parameters before operating
  if (!((pen || textoutline || textfill) && area && text && font)) {
    const char *s = "A draw text object must include the following "
                    "attributes. A pen or a textoutline or "
                    " textfill. As well, an area, text and font";
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
  if (textoutline) {
    bUsePathLayout = true;
    bOutline = true;
  }
  cairo_get_matrix(context.cr, &mat._matrix);

  // if the text is filled with a texture or gradient
  if (textfill) {
    bFilled = true;
    bUsePathLayout = true;
  }

  std::function<void(cairo_t * cr, AREA & a)> fnShadow;
  std::function<void(cairo_t * cr, AREA & a)> fn;

  if (textshadow) {
    fnShadow = [=](cairo_t *cr, AREA &a) {
      createShadow();
      cairo_set_source_surface(cr, shadowImage, a.x, a.y);
      cairo_rectangle(cr, a.x, a.y, a.w, a.h);
      cairo_fill(cr);
    };
  } else {

    fnShadow = [=](cairo_t *cr, AREA &a) {};
  }

  // set the drawing function
  if (bUsePathLayout) {
    // set the drawing function to the one that will be used by the rendering
    // options for text. These functions accept five parameters.
    // These are the clipping versions that have coordinates.
    if (bFilled && bOutline) {
      fn = [=](cairo_t *cr, AREA a) {
        cairo_set_matrix(context.cr, &mat._matrix);

        DrawingOutput::invoke(cr);
        if (set_layout_options(cr))
          pango_cairo_update_layout(cr, layout);
        fnShadow(cr, a);
        cairo_move_to(cr, a.x, a.y);
        pango_cairo_layout_path(cr, layout);
        textfill->emit(cr, a.x, a.y, a.w, a.h);
        cairo_fill_preserve(cr);
        textoutline->emit(cr, a.x, a.y, a.w, a.h);
        cairo_stroke(cr);
      };

      // text is only filled.
    } else if (bFilled) {
      fn = [=](cairo_t *cr, AREA a) {
        cairo_set_matrix(context.cr, &mat._matrix);
        DrawingOutput::invoke(cr);
        if (set_layout_options(cr))
          pango_cairo_update_layout(cr, layout);
        fnShadow(cr, a);
        cairo_move_to(cr, a.x, a.y);
        pango_cairo_layout_path(cr, layout);
        textfill->emit(cr, a.x, a.y, a.w, a.h);
        cairo_fill(cr);
      };

      // text is only outlined.
    } else if (bOutline) {
      fn = [=](cairo_t *cr, AREA a) {
        cairo_set_matrix(context.cr, &mat._matrix);
        DrawingOutput::invoke(cr);
        if (set_layout_options(cr))
          pango_cairo_update_layout(cr, layout);
        fnShadow(cr, a);
        cairo_move_to(cr, a.x, a.y);
        pango_cairo_layout_path(cr, layout);
        textoutline->emit(cr, a.x, a.y, a.w, a.h);
        cairo_stroke(cr);
      };
    }

  } else {

    // no outline or fill defined, therefore the pen is used.
    // fastest text display uses the function
    //  which uses the raster of the font system
    //        --- see pango_cairo_show_layout
    fn = [=](cairo_t *cr, AREA a) {
      cairo_set_matrix(context.cr, &mat._matrix);
      DrawingOutput::invoke(cr);
      if (set_layout_options(cr))
        pango_cairo_update_layout(cr, layout);
      fnShadow(cr, a);
      cairo_move_to(cr, a.x, a.y);
      pen->emit(cr, a.x, a.y, a.w, a.h);
      pango_cairo_show_layout(cr, layout);
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

    AREA a = *area;
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
      cairo_set_source_surface(context.cr, _buf.rendered, area->x, area->y);
      double tw, th;
      tw = std::min(_inkRectangle.width, area->w);
      th = std::min(_inkRectangle.height, area->h);

      cairo_rectangle(context.cr, _inkRectangle.x, _inkRectangle.y, tw, th);
      cairo_fill(context.cr);
    };
    auto fnClipping = [=](DisplayContext &context) {
      cairo_set_matrix(context.cr, &mat._matrix);
      DrawingOutput::invoke(context.cr);
      cairo_set_source_surface(context.cr, _buf.rendered, area->x, area->y);
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
      fn(context.cr, *area);
      evaluate_cache(context);
    };
    auto fnClipping = [=](DisplayContext &context) {
      cairo_rectangle(context.cr, _intersection.x, _intersection.y,
                      _intersection.width, _intersection.height);
      cairo_clip(context.cr);
      DrawingOutput::invoke(context.cr);
      fn(context.cr, *area);
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
  area = context.currentUnits.area;
  if (!area) {
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
  const AREA &a = *area;
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
