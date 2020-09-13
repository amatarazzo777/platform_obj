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

/**
\internal
\def UX_DECLARE_TYPE_INDEX_MEMORY
\tparam typename T
\param const std::shared_ptr<T> ptr)

\brief sets a value at the specific type info. The storage is
std::any however can be correlated directly back into the type.

*/
typedef std::function<std::size_t(void)> hash_function_t;
typedef std::tuple<std::any, hash_function_t> unit_memory_storage_object_t;
typedef std::unordered_map<std::type_index, unit_memory_storage_object_t>
    unit_memory_storage_t;

#define UX_DECLARE_TYPE_INDEX_MEMORY(FUNCTION_NAME)                            \
  unit_memory_storage_t FUNCTION_NAME##_storage = {};                          \
                                                                               \
  template <typename T> void FUNCTION_NAME(const std::shared_ptr<T> ptr) {     \
    auto ti = std::type_index(typeid(T));                                      \
    FUNCTION_NAME##_storage[ti] =                                              \
        std::make_tuple(ptr, [&]() { return ptr->hash_code(); });              \
  }                                                                            \
                                                                               \
  template <typename T>                                                        \
  void FUNCTION_NAME(const std::shared_ptr<display_unit_t> ptr) {              \
    auto ti = std::type_index(typeid(T));                                      \
    FUNCTION_NAME##_storage[ti] =                                              \
        std::make_tuple(std::dynamic_pointer_cast<T>(ptr), [&]() {             \
          return std::dynamic_pointer_cast<T>(ptr)->hash_code();               \
        });                                                                    \
  }                                                                            \
                                                                               \
  template <typename T>                                                        \
  auto FUNCTION_NAME(void) const noexcept->const std::shared_ptr<T> {          \
    std::shared_ptr<T> ptr = {};                                               \
    auto ti = std::type_index(typeid(T));                                      \
    auto item = FUNCTION_NAME##_storage.find(ti);                              \
    if (item != FUNCTION_NAME##_storage.end()) {                               \
      auto obj_data = std::get<std::any>(item->second);                        \
      ptr = std::any_cast<std::shared_ptr<T>>(obj_data);                       \
    }                                                                          \
    return ptr;                                                                \
  }                                                                            \
                                                                               \
  std::size_t FUNCTION_NAME##_hash_code_all(void) const noexcept {             \
    std::size_t value = {};                                                    \
    for (auto &n : FUNCTION_NAME##_storage) {                                  \
      hash_combine(value, std::get<hash_function_t>(n.second)());              \
    }                                                                          \
    return value;                                                              \
  }                                                                            \
                                                                               \
  template <typename T> auto FUNCTION_NAME##_hash_code(void)->std::size_t {    \
    std::size_t value = {};                                                    \
    auto ti = std::type_index(typeid(T));                                      \
    auto item = FUNCTION_NAME##_storage.find(ti);                              \
    if (item != FUNCTION_NAME##_storage.end()) {                               \
      value = std::get<hash_function_t>(item->second)();                       \
    }                                                                          \
    return value;                                                              \
  }                                                                            \
  void FUNCTION_NAME##_clear(void) { FUNCTION_NAME##_storage.clear(); }
