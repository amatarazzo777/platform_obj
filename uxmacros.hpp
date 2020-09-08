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
These objects must expose a method named hash_code() which returns a std::size_t.
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
\brief Creates interface routines for the hashing system and change detection logic.
  Hashes each of the listed values within the macro parameters.
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

/**
\internal
\def DECLARE_PAINTER_BRUSH_DISPLAY_UNIT(CLASS_NAME)
\brief creates a painter brush object that is also a display unit.
class inherits publically display_unit_t and painter_brush_t
*/
#define DECLARE_PAINTER_BRUSH_DISPLAY_UNIT(CLASS_NAME)                         \
  namespace uxdevice {                                                         \
  using CLASS_NAME = class CLASS_NAME : public display_unit_t,                 \
        public painter_brush_t {                                               \
  public:                                                                      \
    using painter_brush_t::painter_brush_t;                                    \
    CLASS_NAME &operator=(const CLASS_NAME &other) {                           \
      painter_brush_t::operator=(other);                                       \
      display_unit_t::operator=(other);                                        \
      return *this;                                                            \
    }                                                                          \
    CLASS_NAME(const CLASS_NAME &other)                                        \
        : painter_brush_t(other), display_unit_t(other) {}                     \
    CLASS_NAME(CLASS_NAME &&other)                                             \
        : painter_brush_t(other), display_unit_t(other) {}                     \
    void emit(cairo_t *cr) {                                                   \
      painter_brush_t::emit(cr);                                               \
      cairo_set_line_width(cr, lineWidth);                                     \
    }                                                                          \
    void emit(cairo_t *cr, double x, double y, double w, double h) {           \
      painter_brush_t::emit(cr);                                               \
      cairo_set_line_width(cr, lineWidth);                                     \
    }                                                                          \
    HASH_OBJECT_MEMBERS(display_unit_t::hash_code(), HASH_TYPE_ID_THIS,        \
                        painter_brush_t::hash_code(), lineWidth, radius, x, y) \
    double lineWidth = 1;                                                      \
    unsigned short radius = 3;                                                 \
    double x = 1, y = 1;                                                       \
    TYPED_INDEX_INTERFACE(CLASS_NAME)                                          \
  };                                                                           \
  }                                                                            \
  STD_HASHABLE(uxdevice::CLASS_NAME)

/**
\internal
\def DECLARE_MARKER_DISPLAY_UNIT(CLASS_NAME)
\brief declares a class that marks a unit but does not store a value.
This is useful for switch and state logic. When the ite is present, the
invoke method is called. class inherits publically display_unit_t
*/
#define DECLARE_MARKER_DISPLAY_UNIT(CLASS_NAME)                                \
  namespace uxdevice {                                                         \
  using CLASS_NAME = class CLASS_NAME : public display_unit_t {                \
  public:                                                                      \
    CLASS_NAME() {}                                                            \
                                                                               \
    CLASS_NAME &operator=(CLASS_NAME &&other) noexcept {                       \
      display_unit_t::operator=(other);                                        \
    }                                                                          \
                                                                               \
    CLASS_NAME &operator=(const CLASS_NAME &other) {                           \
      display_unit_t::operator=(other);                                        \
      return *this;                                                            \
    }                                                                          \
                                                                               \
    CLASS_NAME(CLASS_NAME &&other) noexcept {                                  \
      display_unit_t(other);                                                   \
      return *this;                                                            \
    }                                                                          \
    CLASS_NAME(const CLASS_NAME &other) { display_unit_t(other); }             \
                                                                               \
    virtual ~CLASS_NAME() {}                                                   \
    void invoke(display_unit_t &); \
                                                                               \
    TYPED_INDEX_INTERFACE(CLASS_NAME)                                          \
    HASH_OBJECT_MEMBERS(display_unit_t::hash_code(), HASH_TYPE_ID_THIS)        \
  };                                                                           \
  }                                                                            \
  STD_HASHABLE(uxdevice::CLASS_NAME);

/**
\internal
\def DECLARE_STORING_EMITTER_DISPLAY_UNIT(CLASS_NAME, STORAGE_TYPE)
\param CLASS_NAME - the name the display unit should assume.
\param STORAGE_TYPE - the storage class or trivial type.

\brief provides the flexibility to store associated data. the invoke
public method is called. The class has a public member named "value" of the
given type within the second macro parameter.
*/
#define DECLARE_STORING_EMITTER_DISPLAY_UNIT(CLASS_NAME, STORAGE_TYPE)         \
  namespace uxdevice {                                                         \
  using CLASS_NAME = class CLASS_NAME : public display_unit_t {                \
  public:                                                                      \
    using STORAGE_TYPE::STORAGE_TYPE;                                          \
    CLASS_NAME &operator=(const CLASS_NAME &other) {                           \
      display_unit_t::operator=(other);                                        \
      return *this;                                                            \
    }                                                                          \
    CLASS_NAME &operator=(CLASS_NAME &&other) noexcept {                       \
      index_by_t::operator=(other);                                            \
    }                                                                          \
    CLASS_NAME(const CLASS_NAME &other)                                        \
        : display_unit_t(other), value(other.value) {}                         \
    CLASS_NAME(CLASS_NAME &&other)                                             \
    noexcept                                                                   \
        : display_unit_t(other), value(std::move(other.value)),                \
          display_unit_t(other) {}                                             \
                                                                               \
    virtual ~CLASS_NAME() {}                                                   \
                                                                               \
    void invoke(display_context_t &context);                                   \
                                                                               \
    TYPED_INDEX_INTERFACE(CLASS_NAME)                                          \
    HASH_OBJECT_MEMBERS(display_unit_t::hash_code(), HASH_TYPE_ID_THIS, value) \
                                                                               \
    STORAGE_TYPE &value;                                                       \
  };                                                                           \
  }                                                                            \
  STD_HASHABLE(uxdevice::CLASS_NAME);

/**
\internal
\def DECLARE_NAMED_LISTENER_DISPLAY_UNIT(CLASS_NAME)
\brief the macro creates a named listener which inherits
the listener_t interface publicly. The type_info is stored within
the based class.
*/
#define DECLARE_NAMED_LISTENER_DISPLAY_UNIT(CLASS_NAME)                        \
  namespace uxdevice {                                                         \
  using CLASS_NAME = class CLASS_NAME : public listener_t {                    \
  public:                                                                      \
    CLASS_NAME(const event_handler_t &_evtDispatcher)                          \
        : listener_t(std::make_tuple<std::type_info, event_handler>(           \
              std::type_info(this), _evtDispatcher)) {}                        \
    CLASS_NAME &operator=(const CLASS_NAME &other) {                           \
      listener_t::operator=(other);                                            \
      return *this;                                                            \
    }                                                                          \
    CLASS_NAME &operator=(CLASS_NAME &&other) noexcept {                       \
      listener_t::operator=(other);                                            \
    }                                                                          \
    CLASS_NAME(const CLASS_NAME &other) : listener_t(other) {}                 \
    CLASS_NAME(CLASS_NAME &&other)                                             \
    noexcept                                                                   \
        : display_unit_t(other), value(std::move(other.value)),                \
          display_unit_t(other) {}                                             \
                                                                               \
    virtual ~CLASS_NAME() {}                                                   \
                                                                               \
    HASH_OBJECT_MEMBERS(listener_t::hash_code(), HASH_TYPE_ID_THIS)            \
    TYPED_INDEX_INTERFACE(CLASS_NAME)                                          \
  };                                                                           \
  }                                                                            \
  STD_HASHABLE(uxdevice::CLASS_NAME);
