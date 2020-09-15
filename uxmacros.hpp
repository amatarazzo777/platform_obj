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
\file uxmacros.hpp
\date 5/12/20
\version 1.0
\details  macros that minimize the maintained source code.

*/

#pragma once

/**
\internal
\def UX_REGISTER_STD_HASH_SPECIALIZATION(CLASS_NAME)
\param CLASS_NAME to make std::hash aware.
\brief creates an operator() that is accessible from std::hash<>
These objects must expose a method named hash_code() which returns a
std::size_t.
*/
#define UX_REGISTER_STD_HASH_SPECIALIZATION(CLASS_NAME)                        \
  template <> struct std::hash<CLASS_NAME> {                                   \
    std::size_t operator()(CLASS_NAME const &o) const noexcept {               \
      return o.hash_code();                                                    \
    }                                                                          \
  }
