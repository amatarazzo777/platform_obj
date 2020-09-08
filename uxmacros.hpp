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
\details  macros that minimize the maintained source code. These macros provide
functionality inheirient within display units such as hashing and object
constructor, copy, assignment, and move.

*/

#pragma once

// from -
// https://stackoverflow.com/questions/2590677/how-do-i-combine-hash-values-in-c0x
namespace uxdevice {
template <typename T, typename... Rest>
void hash_combine(std::size_t &seed, const T &v, const Rest &... rest) {
  seed ^= std::hash<T>{}(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
  (hash_combine(seed, rest), ...);
}
} // namespace uxdevice

/**
\internal
\def STD_HASHABLE(CLASS_NAME)
\param CLASS_NAME to make std::hash aware.
\brief creates an operator() that is accessible from std::hash<>
These objects must expose a method named hash_code() which returns a
std::size_t.
*/
#define STD_HASHABLE(CLASS_NAME)                                               \
  template <> struct std::hash<CLASS_NAME> {                                   \
    std::size_t operator()(CLASS_NAME const &o) const noexcept {               \
      return o.hash_code();                                                    \
    }                                                                          \
  }

/**
\internal
\def HASH_TYPE_ID_THIS
\brief hashes the type id of the this pointer.
*/
#define HASH_TYPE_ID_THIS std::type_index(typeid(this)).hash_code()

/**
\internal
\def HASH_TYPE_ID_OBJECT
\param OBJECT_NAME - the variable name
\brief hashes the type id of the this pointer.
*/
#define HASH_TYPE_ID_OBJECT(OBJECT_NAME)                                       \
  std::type_index(typeid(OBJECT_NAME)).hash_code()

/**
\internal
\def HASH_OBJECT_MEMBERS
\param ... - variadic parameter expanding to hash combine each listed.
\brief Creates interface routines for the hashing system and change detection
logic. Hashes each of the listed values within the macro parameters.
*/
#define HASH_OBJECT_MEMBERS(...)                                               \
  std::size_t hash_code(void) const noexcept {                                 \
    std::size_t value = {};                                                    \
    hash_combine(value, __VA_ARGS__);                                          \
    return value;                                                              \
  }                                                                            \
  std::size_t __used_hash_code = {};                                           \
  void state_hash_code(void) { __used_hash_code = hash_code(); }               \
  bool is_different_hash() { return hash_code() != __used_hash_code; }

#define HASH_OBJECT_MEMBER_SHARED_PTR(PTR_VAR)                                 \
  PTR_VAR ? PTR_VAR->hash_code() : std::type_index(typeid(PTR_VAR)).hash_code()

#define HASH_OBJECT_MEMBERS_CONTAINING_VECTOR(VECTOR_NAME, CLASS_NAME, ...)    \
  std::size_t hash_code(void) const noexcept {                                 \
    std::size_t value = {};                                                    \
    std::for_each(VECTOR_NAME.begin(), VECTOR_NAME.end(),                      \
                  [&value](const CLASS_NAME &n) { hash_combine(value, n); });  \
    hash_combine(value, __VA_ARGS__);                                          \
    return value;                                                              \
  }

/* the macro creates the stream interface for both constant references
and shared pointers as well as establishes the prototype for the insertion
function. The implementation is not standard and will need definition.
This is the route for formatting objects that accept numerical data and process
to human readable values. Modern implementations include the processing of size
information. Yet within the c++ implementation, the data structures that report
and hold information is elaborate.
*/

#define DECLARE_STREAM_INTERFACE(CLASS_NAME)                                   \
public:                                                                        \
  surface_area_t &operator<<(const CLASS_NAME &data) {                         \
    stream_input(data);                                                        \
    return *this;                                                              \
  }                                                                            \
  surface_area_t &operator<<(const std::shared_ptr<CLASS_NAME> data) {         \
    stream_input(data);                                                        \
    return *this;                                                              \
  }                                                                            \
                                                                               \
private:                                                                       \
  surface_area_t &stream_input(const CLASS_NAME &_val);                        \
  surface_area_t &stream_input(const std::shared_ptr<CLASS_NAME> _val);

/*
The macro provides a creation of necessary input stream routines that
maintains the display lists. These routines are private within the class
and are activated by the << operator. These are the underlying operations.

*/
#define DECLARE_STREAM_IMPLEMENTATION(CLASS_NAME)                              \
public:                                                                        \
  surface_area_t &operator<<(const CLASS_NAME &data) {                         \
    stream_input(data);                                                        \
    return *this;                                                              \
  }                                                                            \
  surface_area_t &operator<<(const std::shared_ptr<CLASS_NAME> data) {         \
    stream_input(data);                                                        \
    return *this;                                                              \
  }                                                                            \
                                                                               \
private:                                                                       \
  surface_area_t &stream_input(const CLASS_NAME &_val) {                       \
    stream_input(make_shared<CLASS_NAME>(_val));                               \
    return *this;                                                              \
  }                                                                            \
  surface_area_t &stream_input(const shared_ptr<CLASS_NAME> _val) {            \
    DL_SPIN;                                                                   \
    auto item = DL.emplace_back(_val);                                         \
    item->invoke(context);                                                     \
    DL_CLEAR;                                                                  \
    maintain_index(item);                                                      \
    return *this;                                                              \
  }
