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

#pragma once

#define PI (3.14159265358979323846264338327f)

#include <algorithm>
#include <any>
#include <array>
#include <cstdint>

#include <assert.h>
#include <atomic>
#include <cctype>
#include <chrono>
#include <climits>
#include <cmath>
#include <condition_variable>
#include <cstdarg>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <functional>
#include <mutex>
#include <thread>

#include <iomanip>
#include <iostream>
#include <iterator>
#include <list>
#include <map>
#include <memory>
#include <numeric>
#include <optional>
#include <random>
#include <regex>
#include <sstream>
#include <stdexcept>
#include <string>
#include <string_view>
#include <tuple>
#include <type_traits>
#include <typeindex>
#include <typeinfo>
#include <unordered_map>
#include <utility>
#include <variant>
#include <vector>

#include <sys/ipc.h>
#include <sys/shm.h>

#include <X11/Xlib-xcb.h>
#include <X11/Xutil.h>
#include <X11/keysym.h>
#include <X11/keysymdef.h>

#include <sys/types.h>
#include <xcb/xcb_keysyms.h>

#include <cairo-xcb.h>
#include <cairo.h>

#include <glib.h>
#include <librsvg/rsvg.h>
#include <pango/pangocairo.h>

#include "uxenums.hpp"
#include "uxmacros.hpp"

/**
\internal
\def hash_members_t
\param ... - variadic parameter expanding to hash combine each listed.
\brief Creates interface routines for the hashing system and change detection
logic. Hashes each of the listed values within the macro parameters.
*/

namespace uxdevice {
class hash_members_t {
public:
  hash_members_t() {}
  virtual ~hash_members_t() {}
  virtual std::size_t hash_code(void) const noexcept = 0;
  std::size_t __used_hash_code = {};
  void state_hash_code(void) { __used_hash_code = hash_code(); }
  bool is_different_hash() { return hash_code() != __used_hash_code; }
};

// from -
// https://stackoverflow.com/questions/2590677/how-do-i-combine-hash-values-in-c0x
template <typename T, typename... Rest>
void hash_combine(std::size_t &seed, const T &v, const Rest &... rest) {
  seed ^= std::hash<T>{}(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
  (hash_combine(seed, rest), ...);
}

} // namespace uxdevice

namespace uxdevice {
typedef std::function<std::size_t(void)> hash_function_t;

class unit_memory_object_t {
public:
  std::any object = {};
  hash_function_t hash_function = {};
};

typedef std::unordered_map<std::type_index, unit_memory_object_t>
    unit_memory_unordered_map_t;
class display_unit_t;
class unit_memory_storage_t {
public:
  unit_memory_storage_t() {}
  virtual ~unit_memory_storage_t() {}
  template <typename T> void unit_memory(const std::shared_ptr<T> ptr) {
    auto ti = std::type_index(typeid(T));
    storage[ti] = unit_memory_object_t{ptr, [&]() { return ptr->hash_code(); }};
  }

  template <typename T>
  void unit_memory(const std::shared_ptr<display_unit_t> ptr) {
    auto ti = std::type_index(typeid(T));
    storage[ti] = unit_memory_object_t{
        std::dynamic_pointer_cast<T>(ptr),
        [&]() { return std::dynamic_pointer_cast<T>(ptr)->hash_code(); }};
  }

  template <typename T>
  auto unit_memory(void) const noexcept -> const std::shared_ptr<T> {
    std::shared_ptr<T> ptr = {};
    auto ti = std::type_index(typeid(T));
    auto item = storage.find(ti);
    if (item != storage.end()) {
      auto obj_data = item->second.object;
      ptr = std::any_cast<std::shared_ptr<T>>(obj_data);
    }
    return ptr;
  }
  void copy_unit_memory(const unit_memory_storage_t &other) {
    storage = other.storage;
  }

  std::size_t unit_memory_hash_code(void) const noexcept {
    std::size_t value = {};
    for (auto &n : storage) {
      hash_combine(value, n.second.hash_function());
    }
    return value;
  }

  template <typename T> std::size_t unit_memory_hash_code(void) {
    std::size_t value = {};
    auto ti = std::type_index(typeid(T));
    auto item = storage.find(ti);
    if (item != storage.end()) {
      value = item->second.hash_function();
    }
    return value;
  }
  void unit_memory_clear(void) { storage.clear(); }

private:
  unit_memory_unordered_map_t storage = {};
};

} // namespace uxdevice
