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
\file uxdisplayunitbase.hpp
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
    context.unit_memory.drawing_options_fn.remove_if([=](auto &n) {
      auto &funcTarget = std::get<cairo_function_t>(n->_data);
      return funcTarget.target_type().hash_code() == optType;
    });
    context.unit_memory.drawing_options_fn.emplace_back(this);
  }
  state_hash_code();
}
