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
#pragma once
#ifndef UXDISPLAYUNITBASE_HPP
#define UXDISPLAYUNITBASE_HPP

/**
\author Anthony Matarazzo
\file uxdisplayunitbase.hpp
\date 5/12/20
\version 1.0
\details The file implements all base classes and template class factories
that inform and operate within the rendering system. THe following templates
are for use by the displayunits.hpp.

painter_brush_emitter_t
marker_emitter_t
storage_emitter_t
class_storage_emitter_t
storage_drawing_function_t
class_storage_drawing_function_t

*/
/**
\internal
\typedef indirect_index_display_unit_t as a base member, the key data member
uses this type. All classes can provide an indirect numerical or string key.
The [] operator exposes searching for objects.
 */
namespace uxdevice {
typedef std::variant<std::monostate, std::string, std::size_t>
    indirect_index_display_unit_t;
}

/**
\internal
\template specializes the std::hash<uxdevice::indirect_index_display_unit_t>
 * std structure for () operator hashing.
 */
template <> struct std::hash<uxdevice::indirect_index_display_unit_t> {
  std::size_t
  operator()(uxdevice::indirect_index_display_unit_t const &o) const noexcept {
    size_t value = o.index();
    uxdevice::hash_combine(value, std::type_index(typeid(o)).hash_code());
    if (auto pval = std::get_if<std::string>(&o))
      uxdevice::hash_combine(value, *pval);
    else if (auto pval = std::get_if<std::size_t>(&o))
      uxdevice::hash_combine(value, *pval);
    return value;
  }
};

/// \typedef cairo_function_t
/// @brief holds a call to a cairo function with parameters bound.
/// The cairo context - cr is provided later.
namespace uxdevice {
typedef std::function<void(cairo_t *cr)> cairo_function_t;
using cairo_function_t = cairo_function_t;

} // namespace uxdevice
template <> struct std::hash<uxdevice::cairo_function_t> {
  std::size_t operator()(uxdevice::cairo_function_t const &o) const noexcept {
    return reinterpret_cast<std::size_t>(std::addressof(o));
  }
};

namespace uxdevice {
using cairo_option_function_t = class cairo_option_function_t {
public:
  cairo_option_function_t() {}
  cairo_option_function_t(const cairo_function_t &val) { value.push_back(val); }

  cairo_option_function_t &operator=(const cairo_option_function_t &other) {
    value = other.value;
    return *this;
  }
  /// @brief move assignment
  cairo_option_function_t &operator=(cairo_option_function_t &&other) noexcept {
    value = std::move(other.value);
    return *this;
  }

  cairo_option_function_t(cairo_option_function_t &other) noexcept
      : value(other.value) {}

  cairo_option_function_t(cairo_option_function_t &&other) noexcept
      : value(std::move(other.value)) {}
  virtual ~cairo_option_function_t() {}
  virtual void invoke(display_context_t &context);
  virtual void emit(cairo_t *cr);

  std::size_t hash_code(void) const noexcept;

  std::list<cairo_function_t> value = {};
};
} // namespace uxdevice
  // namespace uxdevice

/**

\internal
\brief
  Abstract interfaces that should be defined within the class definition
template parameters of the object factories. When these are placed there, the
public members of the pure abstract class are inherited within the resulting
class generated  from the template. This allows one to implement the function
for callback or usage where appropriate. The uxdisplayunits.cpp file contains
the implementation of these objects. Objects are of a trivial storage or class
type.  These methods provide usage for the data or marking the position within
the display list.

*/
namespace uxdevice {
class emit_display_context_abstract_t {
public:
  virtual ~emit_display_context_abstract_t(){};
  virtual void emit(display_context_t &context) = 0;
};

} // namespace uxdevice

namespace uxdevice {
class emit_cairo_abstract_t {
public:
  virtual ~emit_cairo_abstract_t(){};
  virtual void emit(cairo_t *cr) = 0;
};

} // namespace uxdevice

namespace uxdevice {
class emit_cairo_relative_coordinate_abstract_t {
public:
  virtual ~emit_cairo_relative_coordinate_abstract_t(){};
  virtual void emit_relative(cairo_t *cr) = 0;
  virtual void emit_absolute(cairo_t *cr) = 0;
};

} // namespace uxdevice

namespace uxdevice {
class emit_cairo_coordinate_abstract_t {
public:
  virtual ~emit_cairo_coordinate_abstract_t(){};
  virtual void emit(cairo_t *cr) = 0;
  virtual void emit(cairo_t *cr, coordinate_t &a) = 0;
};

} // namespace uxdevice

namespace uxdevice {
class emit_pango_abstract_t {
public:
  virtual ~emit_pango_abstract_t(){};
  virtual void emit(PangoLayout *layout) = 0;
};
} // namespace uxdevice

namespace uxdevice {
class attribute_display_context_memory_t {
public:
  virtual ~attribute_display_context_memory_t() {}
};

} // namespace uxdevice

/**
\internal
\class display_unit_t
\brief

\brief base class for all display units. defaulted
is the is_output function. Drawing object should override
this and return true. This enables the checking of the surface
for errors after invocation.

*/
namespace uxdevice {
class display_unit_t : virtual public hash_members_t {
public:
  display_unit_t() {}

  /// @brief copy assignment operator
  display_unit_t &operator=(const display_unit_t &other) {
    is_processed = other.is_processed;
    viewport_inked = other.viewport_inked;
    error_description = other.error_description;
    return *this;
  }
  /// @brief move assignment
  display_unit_t &operator=(display_unit_t &&other) noexcept {
    is_processed = std::move(other.is_processed);
    viewport_inked = std::move(other.viewport_inked);
    error_description = std::move(other.error_description);
    return *this;
  }

  /// @brief move constructor
  display_unit_t(display_unit_t &&other) noexcept
      : is_processed(std::move(other.is_processed)),
        viewport_inked(std::move(other.viewport_inked)),
        error_description(std::move(other.error_description)) {}

  /// @brief copy constructor
  display_unit_t(const display_unit_t &other)
      : is_processed(other.is_processed), viewport_inked(other.viewport_inked),
        error_description(other.error_description) {}

  virtual ~display_unit_t() {}

  virtual bool is_output(void) { return false; }
  void error(const char *s) { error_description = s; }
  bool valid(void) { return error_description == nullptr; }
  void changed(void) { bchanged = true; }
  bool has_changed(void) { return bchanged; }

  std::size_t hash_code(void) const noexcept {
    std::size_t __value = {};
    hash_combine(__value, std::type_index(typeid(display_unit_t)), is_processed,
                 viewport_inked, bchanged, error_description);
    return __value;
  }

  bool is_processed = false;
  bool viewport_inked = false;
  bool bchanged = false;
  const char *error_description = nullptr;
};
} // namespace uxdevice

/**
\internal
\class drawing_output_t
\brief base class for objects that produce image_block_t drawing commands
The is_output is overridden to return true. As well the object uses
the render work list to determine if a particular image_block_t is on screen.

\details


 */
namespace uxdevice {
class drawing_output_t : public display_unit_t {
public:
  typedef display_context_t::context_cairo_region_t context_cairo_region_t;

  /// @brief default constructor
  drawing_output_t() : display_unit_t() {}

  /// @brief move assignment
  drawing_output_t &operator=(drawing_output_t &&other) noexcept {
    display_unit_t::operator=(other);

    internal_buffer = std::move(other.internal_buffer);

    fn_draw = std::move(other.fn_draw);
    fn_draw_clipped = std::move(other.fn_draw_clipped);
    fn_cache_surface = std::move(other.fn_cache_surface);
    fn_base_surface = std::move(other.fn_base_surface);

    options = std::move(other.options);
    ink_rectangle = std::move(other.ink_rectangle);
    intersection_int = std::move(other.intersection_int);
    intersection_double = std::move(other.intersection_double);
    return *this;
  }

  /// @brief copy assignment operator
  drawing_output_t &operator=(const drawing_output_t &other) {
    display_unit_t::operator=(other);

    if (other.internal_buffer.rendered)
      internal_buffer.rendered =
          cairo_surface_reference(other.internal_buffer.rendered);

    if (other.internal_buffer.cr)
      internal_buffer.cr = cairo_reference(other.internal_buffer.cr);

    fn_draw = other.fn_draw;
    fn_draw_clipped = other.fn_draw_clipped;
    fn_cache_surface = other.fn_cache_surface;
    fn_base_surface = other.fn_base_surface;

    std::copy(other.options.value.begin(), other.options.value.end(),
              std::back_inserter(options.value));

    ink_rectangle = other.ink_rectangle;
    intersection_int = other.intersection_int;
    intersection_double = other.intersection_double;

    return *this;
  }

  /// @brief move constructor
  drawing_output_t(drawing_output_t &&other) noexcept
      : display_unit_t(other),
        internal_buffer(std::move(other.internal_buffer)),

        fn_cache_surface(std::move(other.fn_cache_surface)),
        fn_base_surface(std::move(other.fn_base_surface)),
        fn_draw(std::move(other.fn_draw)),
        fn_draw_clipped(std::move(other.fn_draw_clipped)),

        options(std::move(other.options)),
        ink_rectangle(std::move(other.ink_rectangle)),
        intersection_int(std::move(other.intersection_int)),
        intersection_double(std::move(other.intersection_double)) {}

  /// @brief copy constructor
  drawing_output_t(const drawing_output_t &other) {
    // invoke copy operator
    *this = other;
  }

  virtual ~drawing_output_t() {
    display_context_t::destroy_buffer(internal_buffer);
  }
  void emit(display_context_t &context);
  void intersect(cairo_rectangle_t &r);
  void intersect(context_cairo_region_t &r);

  bool is_output(void) { return true; }

  // These functions switch the rendering apparatus from off
  // screen threaded to on screen. all rendering is serialize to the main
  // surface
  //
  std::atomic_flag lockFunctors = ATOMIC_FLAG_INIT;
#define LOCK_FUNCTORS_SPIN                                                     \
  while (lockFunctors.test_and_set(std::memory_order_acquire))

#define LOCK_FUNCTORS_CLEAR lockFunctors.clear(std::memory_order_release)

  void functors_lock(bool b) {
    if (b)
      LOCK_FUNCTORS_SPIN;
    else
      LOCK_FUNCTORS_CLEAR;
  }
  std::size_t hash_code(void) const noexcept {
    std::size_t __value = {};
    hash_combine(__value, std::type_index(typeid(drawing_output_t)),
                 has_ink_extents, overlap, error_description);
    return __value;
  }

  bool has_ink_extents = false;
  cairo_rectangle_int_t c = cairo_rectangle_int_t();
  cairo_region_overlap_t overlap = CAIRO_REGION_OVERLAP_OUT;

  std::atomic<bool> bRenderBufferCached = false;
  draw_buffer_t internal_buffer = {};

  draw_logic_t fn_cache_surface = draw_logic_t();
  draw_logic_t fn_base_surface = draw_logic_t();
  draw_logic_t fn_draw = draw_logic_t();
  draw_logic_t fn_draw_clipped = draw_logic_t();

  // measure processing time
  std::chrono::system_clock::time_point last_render_time = {};
  void evaluate_cache(display_context_t &context);
  bool first_time_rendered = true;
  cairo_option_function_t options = {};
  cairo_rectangle_int_t ink_rectangle = cairo_rectangle_int_t();
  cairo_rectangle_t ink_rectangle_double = cairo_rectangle_t();
  cairo_rectangle_int_t intersection_int = cairo_rectangle_int_t();
  cairo_rectangle_t intersection_double = cairo_rectangle_t();
};
} // namespace uxdevice

/**
\internal

\class typed_index_t

\tparam T the type of interface reference to return from the index public member
function.

\brief A class which is inherited which manages the index key
    data.The operators,
    copy,
    move constructors are invoked by the class deriving : display_unit_t.
*/

namespace uxdevice {
class key_storage_t {
public:
  key_storage_t() {}
  virtual ~key_storage_t() {}
  key_storage_t(const indirect_index_display_unit_t &_key) : key(_key) {}
  /// @brief copy assignment operator
  key_storage_t &operator=(const key_storage_t &other) {
    key = other.key;
    return *this;
  }
  /// @brief move assignment
  key_storage_t &operator=(key_storage_t &&other) noexcept {
    key = std::move(other.key);
    return *this;
  }
  /// @brief move constructor
  key_storage_t(key_storage_t &&other) noexcept { key = std::move(other.key); }

  /// @brief copy constructor
  key_storage_t(const key_storage_t &other) { *this = other; }

  indirect_index_display_unit_t key = {};
};

template <typename T>
class typed_index_t : public key_storage_t, virtual public hash_members_t {
public:
  /// @brief default constructor
  typed_index_t() : key_storage_t{} {}

  /// @brief copy assignment operator
  typed_index_t &operator=(const typed_index_t &other) {
    key_storage_t::operator=(other);
    return *this;
  }
  /// @brief move assignment
  typed_index_t &operator=(typed_index_t &&other) noexcept {
    key_storage_t::operator=(other);
    return *this;
  }
  /// @brief move constructor
  typed_index_t(typed_index_t &&other) noexcept : key_storage_t(other) {}

  /// @brief copy constructor
  typed_index_t(const typed_index_t &other) : key_storage_t(other) {}

  T &index(const std::string &_k) {
    key_storage_t::operator=(indirect_index_display_unit_t(_k));
    return *static_cast<T *>(this);
  }
  T &index(const std::size_t &_k) {
    key_storage_t::operator=(indirect_index_display_unit_t(_k));
    return *static_cast<T *>(this);
  }
  std::size_t hash_code(void) const noexcept {
    std::size_t __value = {};
    hash_combine(__value, std::type_index(typeid(drawing_output_t)), key);
    return __value;
  }
  virtual ~typed_index_t() {}
};
} // namespace uxdevice

/**
\internal
\class listener_t

\tparam T - the name the display unit should assume.

\brief


*/
namespace uxdevice {
template <typename T>
class listener_t : public typed_index_t<T>,
                   virtual public hash_members_t,
                   std::enable_shared_from_this<T> {
public:
  listener_t() = delete;
  listener_t(event_handler_t _dispatch)
      : ti(std::type_index(typeid(T))), dispatch_event(_dispatch) {}

  std::size_t hash_code(void) const noexcept {
    std::size_t __value = {};
    hash_combine(__value, typed_index_t<T>::hash_code(), ti);
    return __value;
  }

  std::type_index ti;
  event_handler_t dispatch_event;
};

} // namespace uxdevice

/**
\internal
\class painter_brush_emitter_t
\tparam T used to note and enable the typed index interface which allows
continuation syntax when objects are created and an indirect index is applied.
Such as: text_font_t{"arial 20px"}.index("idxfont")
Here the .index property returns a strong type of the original.


\tparam Args... provides the ability to catch specific functions and their
prototypes. emit. Attributes may also be listed that apply to specific
functionality of the system such as labeling items as textual rendering
related. This is useful when applying attributes that comprise a rendering
function.


\brief creates a painter brush object that is also a display unit.
class inherits publicly display_unit_t and painter_brush_t

*/

namespace uxdevice {
template <typename T, typename... Args>
class painter_brush_emitter_t : public display_unit_t,
                                public painter_brush_t,
                                virtual public hash_members_t,
                                public typed_index_t<T>,
                                std::enable_shared_from_this<T>,
                                public Args... {

public:
  using painter_brush_t::painter_brush_t;
  // copy constructor
  painter_brush_emitter_t(const painter_brush_emitter_t &other)
      : display_unit_t(other),
        painter_brush_t(other), typed_index_t<T>::typed_index_t(other) {}
  // move constructor
  painter_brush_emitter_t(painter_brush_emitter_t &&other) noexcept
      : display_unit_t(other),
        painter_brush_t(other), typed_index_t<T>::typed_index_t(other) {}

  // copy assignment operator
  painter_brush_emitter_t &operator=(const painter_brush_emitter_t &other) {
    display_unit_t::operator=(other);
    typed_index_t<T>::operator=(other);
    hash_members_t::operator=(other);
    painter_brush_t::operator=(other);
    return *this;
  }
  // move assignment operator
  painter_brush_emitter_t &operator=(painter_brush_emitter_t &&other) noexcept {
    display_unit_t::operator=(other);
    typed_index_t<T>::operator=(other);
    hash_members_t::operator=(other);
    painter_brush_t::operator=(other);
    return *this;
  }

  virtual ~painter_brush_emitter_t() {}

  void emit(display_context_t &context) { painter_brush_t::emit(context.cr); }
  void emit(cairo_t *cr) { painter_brush_t::emit(cr); }
  void emit(cairo_t *cr, coordinate_t &a) { painter_brush_t::emit(cr, a); }

  std::size_t hash_code(void) const noexcept {
    std::size_t __value = {};
    hash_combine(__value, std::type_index(typeid(T)),
                 painter_brush_t::hash_code(), display_unit_t::hash_code(),
                 lineWidth, radius, x, y);

    return __value;
  }

  double lineWidth = 1;
  unsigned short radius = 3;
  double x = 1, y = 1;
};
} // namespace uxdevice

/**
\internal
\class marker_emitter_t

\tparam T - the name the display unit should assume.

\tparam Args... - lists applicable invoke and emit functions.
Depending on the particular display unit type, context, pango or cairo
emit functions may be included from the abstract base functions. When
these are listed, the interface callback must be defined.

\brief declares a class that marks a unit as an emitter but does not store a
value. This is useful for switch and state logic. When the item is present, the
emit method is called. class inherits publicly display_unit_t : public
polymorphic_overloads_t
*/
namespace uxdevice {
template <typename T, typename... Args>
class marker_emitter_t : public display_unit_t,
                         virtual public hash_members_t,
                         public typed_index_t<T>,
                         public Args... {
public:
  marker_emitter_t() {}

  marker_emitter_t &operator=(marker_emitter_t &&other) noexcept {
    display_unit_t::operator=(other);
    return *this;
  }

  marker_emitter_t &operator=(const marker_emitter_t &other) {
    display_unit_t::operator=(other);
    return *this;
  }
  std::size_t hash_code(void) const noexcept {
    std::size_t __value = {};
    hash_combine(__value, typed_index_t<T>::hash_code(),
                 std::type_index(typeid(T)).hash_code(),
                 display_unit_t::hash_code());

    return __value;
  }

  marker_emitter_t(marker_emitter_t &&other) noexcept : display_unit_t(other) {}
  marker_emitter_t(const marker_emitter_t &other) : display_unit_t(other) {}

  virtual ~marker_emitter_t() {}
};
} // namespace uxdevice

/**
\internal

\class storage_emitter_t

\tparam T - the name the display unit should assume.

\tparam TS - the storage class or trivial type.

\tparam Args... - lists applicable invoke and emit functions.
Depending on the particular display unit type, context, pango or cairo
emit functions may be included from the abstract base functions. When
these are listed, the interface callback must be defined.

\brief provides the flexibility to store associated trivial data such as enum,
double, etc. The template names this storage area within the object 'value'.

*/
namespace uxdevice {
template <typename T, typename TS, typename... Args>
class storage_emitter_t : virtual public hash_members_t,
                          public display_unit_t,
                          public typed_index_t<T>,
                          std::enable_shared_from_this<T>,
                          public Args... {
public:
  storage_emitter_t() : value(TS{}) {}
  storage_emitter_t(const TS &o) : value(o) {}

  // copy constructor
  storage_emitter_t(const storage_emitter_t &other)
      : hash_members_t(other), display_unit_t(other), typed_index_t<T>(other),
        value(other.value) {}

  // move constructor
  storage_emitter_t(storage_emitter_t &&other) noexcept
      : hash_members_t(other), display_unit_t(other), typed_index_t<T>(other),
        value(other.value) {}

  // copy assignment operator
  storage_emitter_t &operator=(const storage_emitter_t &other) {
    value = other.value;
    display_unit_t::operator=(other);
    typed_index_t<T>::operator=(other);
    hash_members_t::operator=(other);
    return *this;
  }

  // move assignment operator
  storage_emitter_t &operator=(storage_emitter_t &&other) noexcept {
    value = other.other;
    display_unit_t::operator=(other);
    typed_index_t<T>::operator=(other);
    hash_members_t::operator=(other);
    return *this;
  }

  virtual ~storage_emitter_t() {}
  std::size_t hash_code(void) const noexcept {
    std::size_t __value = {};
    hash_combine(__value, typed_index_t<T>::hash_code(),
                 std::type_index(typeid(T)).hash_code(),
                 display_unit_t::hash_code(), value);

    return __value;
  }
  TS value;
};
} // namespace uxdevice

/**
\internal
\class class_storage_emitter_t

\tparam T - the name the display unit should assume.

\tparam TC - the storage class type. The storage type is publicly
inherited and all constructors are also inherited. The class must include
the hash_members_t interface. That is, implement the hash_code()
 member function.

\tparam Args... - inherits the data class interface as public.
  There constructors are inherited as well as the public methods of the class.

\brief provides the flexibility to create classes that store associated data.


*/
namespace uxdevice {
template <typename T, typename TC, typename... Args>
class class_storage_emitter_t : public TC,
                                public typed_index_t<T>,
                                public display_unit_t,
                                virtual public hash_members_t,
                                std::enable_shared_from_this<T>,
                                public Args... {
public:
  using TC::TC;
  class_storage_emitter_t() {}

  // copy constructor
  class_storage_emitter_t(const class_storage_emitter_t &other)
      : hash_members_t(other), TC(other), typed_index_t<T>(other),
        display_unit_t(other) {}

  // move constructor
  class_storage_emitter_t(class_storage_emitter_t &&other) noexcept
      : hash_members_t(other), TC(other), typed_index_t<T>(other),
        display_unit_t(other) {}

  // copy assignment operator
  class_storage_emitter_t &operator=(const class_storage_emitter_t &other) {
    display_unit_t::operator=(other);
    hash_members_t::operator=(other);
    typed_index_t<T>::operator=(other);
    TC::operator=(other);
    return *this;
  }

  // move assignment operator
  class_storage_emitter_t &operator=(class_storage_emitter_t &&other) noexcept {
    display_unit_t::operator=(other);
    hash_members_t::operator=(other);
    typed_index_t<T>::operator=(other);
    TC::opterator = (other);
    return *this;
  }

  virtual ~class_storage_emitter_t() {}

  std::size_t hash_code(void) const noexcept {
    std::size_t __value = {};
    hash_combine(__value, typed_index_t<T>::hash_code(),
                 std::type_index(typeid(T)).hash_code(),
                 display_unit_t::hash_code(), TC::hash_code());

    return __value;
  }
};
} // namespace uxdevice

/**
\internal
\class storage_drawing_function_t

\tparam T - the name the display unit should assume.
\tparam TS - the storage trivial type.
\tparam Args.. - this names the applicable invoke and emit functions.
 Depending on the particular display unit type, pango or cairo
emit functions may be useful. The parameter is intended for abstract call
classes to be listed. However user defined classes may also be listed.

\brief provides the flexibility to store associated trivial data in the class.
The class names the the item 'value' which can be accessed from class member
functions such as emit and invoke.

*/

namespace uxdevice {
template <typename T, typename TS, typename... Args>
class storage_drawing_function_t : virtual public drawing_output_t,
                                   virtual public hash_members_t,
                                   public typed_index_t<T>,
                                   std::enable_shared_from_this<T>,
                                   public Args... {
public:
  storage_drawing_function_t() : value(TS{}) {}

  // copy constructor
  storage_drawing_function_t(const storage_drawing_function_t &other)
      : value(other), drawing_output_t(other), typed_index_t<T>(other),
        hash_members_t(other) {}

  // move constructor
  storage_drawing_function_t(storage_drawing_function_t &&other) noexcept
      : value(other), drawing_output_t(other), typed_index_t<T>(other),
        hash_members_t(other) {}

  // copy assignment operator
  storage_drawing_function_t &
  operator=(const storage_drawing_function_t &other) {
    value = other.value;
    drawing_output_t::operator=(other);
    typed_index_t<T>::operator=(other);
    hash_members_t::operator=(other);
    return *this;
  }
  // move assignment operator
  storage_drawing_function_t &
  operator=(storage_drawing_function_t &&other) noexcept {
    value = other.value;
    drawing_output_t::operator=(other);
    typed_index_t<T>::operator=(other);
    hash_members_t::operator=(other);
    return *this;
  }

  virtual ~storage_drawing_function_t() {}

  std::size_t hash_code(void) const noexcept {
    std::size_t __value = {};
    hash_combine(__value, typed_index_t<T>::hash_code(),
                 std::type_index(typeid(T)).hash_code(),
                 drawing_output_t::hash_code(), value);

    return __value;
  }
  TS value;
};
} // namespace uxdevice

/**
\internal
\class class_storage_drawing_function_t

\tparam T - the name the display unit should assume.
\tparam TC - the storage class. The class must implement the hash_members_t
object and hash_code function. \tparam Args... - a list of interfaces to
inheirit. The system uses the spot for abstract classes. However, other user
defined classes can be placed here as well.

\brief Provides the ability to store an object accepting a specific parameter
api. These objects are called to render functions and also have
a dimension. They occupy a bounds as defined by inkRectangle. They can be
established as on or off view_port by intersection test with the
view_port rectangle. The class must include
the hash_members_t interface. That is, implement the hash_code()
 member function.


*/
namespace uxdevice {
template <typename T, typename TC, typename... Args>
class class_storage_drawing_function_t : public TC,
                                         public typed_index_t<T>,
                                         virtual public drawing_output_t,
                                         virtual public hash_members_t,
                                         std::enable_shared_from_this<T>,
                                         public Args... {
public:
  using TC::TC;
  class_storage_drawing_function_t() {}

  // copy constructor
  class_storage_drawing_function_t(
      const class_storage_drawing_function_t &other)
      : hash_members_t(other), drawing_output_t(other),
        TC(other), typed_index_t<T>(other) {}

  // move constructor
  class_storage_drawing_function_t(
      class_storage_drawing_function_t &&other) noexcept
      : hash_members_t(other), drawing_output_t(other),
        TC(other), typed_index_t<T>(other) {}

  // copy assignment operator
  class_storage_drawing_function_t &
  operator=(const class_storage_drawing_function_t &other) {
    drawing_output_t::operator=(other);
    typed_index_t<T>::operator=(other);
    hash_members_t::operator=(other);
    TC::operator=(other);
    return *this;
  }
  // move assignment operator
  class_storage_drawing_function_t &
  operator=(class_storage_drawing_function_t &&other) noexcept {
    drawing_output_t::operator=(other);
    typed_index_t<T>::operator=(other);
    hash_members_t::operator=(other);
    TC::operator=(other);
    return *this;
  }

  std::size_t hash_code(void) const noexcept {
    std::size_t __value = {};
    hash_combine(__value, typed_index_t<T>::hash_code(),
                 std::type_index(typeid(T)).hash_code(),
                 drawing_output_t::hash_code(), TC::hash_code());

    return __value;
  }
  virtual ~class_storage_drawing_function_t() {}
};
} // namespace uxdevice
#endif
