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
\author Anthony Matarazzo
\file uxenums.hpp
\date 5/12/20
\version 1.0
 \details  options for parameters

*/
#pragma once
namespace uxdevice {
enum class antialias_options_t {
  def = CAIRO_ANTIALIAS_DEFAULT,

  /* method */
  off = CAIRO_ANTIALIAS_NONE,
  gray = CAIRO_ANTIALIAS_GRAY,
  subPixel = CAIRO_ANTIALIAS_SUBPIXEL,

  /* hints */
  fast = CAIRO_ANTIALIAS_FAST,
  good = CAIRO_ANTIALIAS_GOOD,
  best = CAIRO_ANTIALIAS_BEST
};

enum class filter_options_t {
  fast = CAIRO_FILTER_FAST,
  good = CAIRO_FILTER_GOOD,
  best = CAIRO_FILTER_BEST,
  nearest = CAIRO_FILTER_NEAREST,
  bilinear = CAIRO_FILTER_BILINEAR,
  gaussian = CAIRO_FILTER_GAUSSIAN
};

enum class extend_options_t {
  off = CAIRO_EXTEND_NONE,
  repeat = CAIRO_EXTEND_REPEAT,
  reflect = CAIRO_EXTEND_REFLECT,
  pad = CAIRO_EXTEND_PAD
};

enum class line_cap_options_t {
  butt = CAIRO_LINE_CAP_BUTT,
  round = CAIRO_LINE_CAP_ROUND,
  square = CAIRO_LINE_CAP_SQUARE
};

enum class line_join_options_t {
  miter = CAIRO_LINE_JOIN_MITER,
  round = CAIRO_LINE_JOIN_ROUND,
  bevel = CAIRO_LINE_JOIN_BEVEL
};

enum graphic_operator_options_t {
  opClear = CAIRO_OPERATOR_CLEAR,
  opSource = CAIRO_OPERATOR_SOURCE,
  opOver = CAIRO_OPERATOR_OVER,
  opIn = CAIRO_OPERATOR_IN,
  opOut = CAIRO_OPERATOR_OUT,
  opAtop = CAIRO_OPERATOR_ATOP,
  opDest = CAIRO_OPERATOR_DEST,
  opDestOver = CAIRO_OPERATOR_DEST_OVER,
  opDestIn = CAIRO_OPERATOR_DEST_IN,
  opDestOut = CAIRO_OPERATOR_DEST_OUT,
  opDestAtop = CAIRO_OPERATOR_DEST_ATOP,
  opXor = CAIRO_OPERATOR_XOR,
  opAdd = CAIRO_OPERATOR_ADD,
  opSaturate = CAIRO_OPERATOR_SATURATE,
  opMultiply = CAIRO_OPERATOR_MULTIPLY,
  opScreen = CAIRO_OPERATOR_SCREEN,
  opOverlay = CAIRO_OPERATOR_OVERLAY,
  opDarken = CAIRO_OPERATOR_DARKEN,
  opLighten = CAIRO_OPERATOR_LIGHTEN,
  opColorDodge = CAIRO_OPERATOR_COLOR_DODGE,
  opColorBurn = CAIRO_OPERATOR_COLOR_BURN,
  opHardLight = CAIRO_OPERATOR_HARD_LIGHT,
  opSoftLight = CAIRO_OPERATOR_SOFT_LIGHT,
  opDifference = CAIRO_OPERATOR_DIFFERENCE,
  opExclusion = CAIRO_OPERATOR_EXCLUSION,
  opHSLHUE = CAIRO_OPERATOR_HSL_HUE,
  opHSLSaturation = CAIRO_OPERATOR_HSL_SATURATION,
  opHSLColor = CAIRO_OPERATOR_HSL_COLOR,
  opHSLLuminosity = CAIRO_OPERATOR_HSL_LUMINOSITY
};
enum class text_alignment_options_t {
  left = PangoAlignment::PANGO_ALIGN_LEFT,
  center = PangoAlignment::PANGO_ALIGN_CENTER,
  right = PangoAlignment::PANGO_ALIGN_RIGHT,
  justified = 4
};
enum class text_ellipsize_options_t {
  off = PANGO_ELLIPSIZE_NONE,
  start = PANGO_ELLIPSIZE_START,
  middle = PANGO_ELLIPSIZE_MIDDLE,
  end = PANGO_ELLIPSIZE_END
};
enum class content_options_t {
  color = CAIRO_CONTENT_COLOR,
  alpha = CAIRO_CONTENT_ALPHA,
  all = CAIRO_CONTENT_COLOR_ALPHA
};
} // namespace uxdevice
