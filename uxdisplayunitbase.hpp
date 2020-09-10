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
/**
\author Anthony Matarazzo
\file uxdisplayunitbase.hpp
\date 5/12/20
\version 1.0
\details

*/
/**
\internal
\typedef indirect_index_display_unit_t as a base member, the key data member
uses this type. All classes can provide an indirect numerical or string key.
The [] operator exposes searching for objects.
 */
namespace uxdevice {
typedef std::variant<std::string, std::size_t> indirect_index_display_unit_t;
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

/**
\internal

\class index_by_t

\brief A class which is inherited which manages the index key data.
The operators, copy, move constructors are invoked by the class deriving:
display_unit_t.

 */
namespace uxdevice {
class index_by_t {
public:
  /// @brief default constructor
  index_by_t() {}

  /// @brief constructor storing a string as key
  index_by_t(std::string _k) : key(_k) {}

  /// @brief constructor storing a size_t as key
  index_by_t(std::size_t _k) : key(_k) {}

  /// @brief copy assignment operator
  index_by_t &operator=(const index_by_t &other) {
    key = other.key;
    return *this;
  }
  /// @brief move assignment
  index_by_t &operator=(index_by_t &&other) noexcept {
    key = std::move(other.key);
    return *this;
  }
  /// @brief move constructor
  index_by_t(index_by_t &&other) noexcept { key = std::move(other.key); }

  /// @brief copy constructor
  index_by_t(const index_by_t &other) { *this = other; }

  // @brief virtual destructor, all objects must have a destructor
  virtual ~index_by_t() {}

  HASH_OBJECT_MEMBERS(HASH_TYPE_ID_THIS, key)
  indirect_index_display_unit_t key = {};
};
} // namespace uxdevice

/**
\internal
\template specializes the std::hash<uxdevice::index_by_t>
 * std structure for () operator hashing. The object's
 * type id as well as its value is summarized. The object
 * exposes a public function hash_code which is invoked.
 */
template <> struct std::hash<uxdevice::index_by_t> {
  std::size_t operator()(uxdevice::index_by_t const &o) const noexcept {
    size_t value = std::type_index(typeid(o)).hash_code();
    uxdevice::hash_combine(value, o.hash_code());
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
class display_unit_t : public index_by_t {
public:
  display_unit_t() {}

  /// @brief copy assignment operator
  display_unit_t &operator=(const display_unit_t &other) {
    index_by_t::operator=(other);
    is_processed = other.is_processed;
    viewport_inked = other.viewport_inked;
    error_description = other.error_description;
    return *this;
  }
  /// @brief move assignment
  display_unit_t &operator=(display_unit_t &&other) noexcept {
    index_by_t::operator=(other);
    is_processed = std::move(other.is_processed);
    viewport_inked = std::move(other.viewport_inked);
    error_description = std::move(other.error_description);
    return *this;
  }

  /// @brief move constructor
  display_unit_t(display_unit_t &&other) noexcept
      : index_by_t(other), is_processed(std::move(other.is_processed)),
        viewport_inked(std::move(other.viewport_inked)),
        error_description(std::move(other.error_description)) {}

  /// @brief copy constructor
  display_unit_t(const display_unit_t &other)
      : index_by_t(other), is_processed(other.is_processed),
        viewport_inked(other.viewport_inked),
        error_description(other.error_description) {}

  virtual ~display_unit_t() {}

  virtual void invoke(display_context_t &context) { state_hash_code(); }
  virtual bool is_output(void) { return false; }
  void error(const char *s) { error_description = s; }
  bool valid(void) { return error_description == nullptr; }

  HASH_OBJECT_MEMBERS(HASH_TYPE_ID_THIS, is_processed, viewport_inked,
                      error_description)

  bool is_processed = false;
  bool viewport_inked = false;
  const char *error_description = nullptr;
};
} // namespace uxdevice
STD_HASHABLE(uxdevice::display_unit_t);

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

    std::copy(other.options.begin(), other.options.end(),
              std::back_inserter(options));

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

  ~drawing_output_t() { display_context_t::destroy_buffer(internal_buffer); }

  void intersect(cairo_rectangle_t &r);
  void intersect(context_cairo_region_t &r);

  void invoke(cairo_t *cr);
  void invoke(display_context_t &context) {}
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

  HASH_OBJECT_MEMBERS(HASH_TYPE_ID_THIS, has_ink_extents,
                      last_render_time.time_since_epoch().count(),
                      fn_cache_surface.target_type().name(),
                      fn_base_surface.target_type().name(),
                      fn_draw.target_type().name(),
                      fn_draw_clipped.target_type().name())

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
  cairo_rectangle_t ink_rectangle = cairo_rectangle_t();
  cairo_rectangle_int_t intersection_int = cairo_rectangle_int_t();
  cairo_rectangle_t intersection_double = cairo_rectangle_t();
};
} // namespace uxdevice
STD_HASHABLE(uxdevice::drawing_output_t);

/**
\internal
\def TYPED_INDEX_INTERFACE(CLASS_NAME)
\brief adds an index() method while returning
the reference to *this in continuation syntax. This provides
the capability to index in the same expression.
*/
#define TYPED_INDEX_IMPLEMENTATION(CLASS_NAME)                                 \
  CLASS_NAME &index(const std::string &_k) {                                   \
    key = _k;                                                                  \
    return *this;                                                              \
  }                                                                            \
  CLASS_NAME &index(const std::size_t &_k) {                                   \
    key = _k;                                                                  \
    return *this;                                                              \
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
        public painter_brush_t, std::enable_shared_from_this<CLASS_NAME> {     \
  public:                                                                      \
    using painter_brush_t::painter_brush_t;                                    \
    CLASS_NAME &operator=(const CLASS_NAME &other) {                           \
      painter_brush_t::operator=(other);                                       \
      display_unit_t::operator=(other);                                        \
      return *this;                                                            \
    }                                                                          \
    CLASS_NAME(const CLASS_NAME &other)                                        \
        : display_unit_t(other), painter_brush_t(other) {}                     \
    CLASS_NAME(CLASS_NAME &&other)                                             \
        : display_unit_t(other), painter_brush_t(other) {}                     \
    void invoke(display_context_t &context);                                   \
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
    TYPED_INDEX_IMPLEMENTATION(CLASS_NAME)                                     \
  };                                                                           \
  }                                                                            \
  STD_HASHABLE(uxdevice::CLASS_NAME);

/**
\internal
\def DECLARE_MARKER_DISPLAY_UNIT(CLASS_NAME)
\brief declares a class that marks a unit but does not store a value.
This is useful for switch and state logic. When the item is present, the
invoke method is called. class inherits publicly display_unit_t
*/
#define DECLARE_MARKER_DISPLAY_UNIT(CLASS_NAME)                                \
  namespace uxdevice {                                                         \
  using CLASS_NAME = class CLASS_NAME : public display_unit_t,                 \
        std::enable_shared_from_this<CLASS_NAME> {                             \
  public:                                                                      \
    CLASS_NAME() {}                                                            \
                                                                               \
    CLASS_NAME &operator=(CLASS_NAME &&other) noexcept {                       \
      display_unit_t::operator=(other);                                        \
      return *this;                                                            \
    }                                                                          \
                                                                               \
    CLASS_NAME &operator=(const CLASS_NAME &other) {                           \
      display_unit_t::operator=(other);                                        \
      return *this;                                                            \
    }                                                                          \
                                                                               \
    CLASS_NAME(CLASS_NAME &&other) noexcept : display_unit_t(other) {}         \
    CLASS_NAME(const CLASS_NAME &other) : display_unit_t(other) {}             \
                                                                               \
    virtual ~CLASS_NAME() {}                                                   \
    virtual void invoke(display_context_t &);                                  \
                                                                               \
    TYPED_INDEX_IMPLEMENTATION(CLASS_NAME)                                     \
    HASH_OBJECT_MEMBERS(display_unit_t::hash_code(), HASH_TYPE_ID_THIS)        \
  };                                                                           \
  }                                                                            \
  STD_HASHABLE(uxdevice::CLASS_NAME);

/**
\internal
\def DECLARE_STORAGE_EMITTER_DISPLAY_UNIT(CLASS_NAME, STORAGE_TYPE)
\param CLASS_NAME - the name the display unit should assume.
\param STORAGE_TYPE - the storage class or trivial type.

\brief provides the flexibility to store associated data. the invoke
public method is called. The class has a public member named "value" of the
given type within the second macro parameter.
*/
#define DECLARE_STORAGE_EMITTER_DISPLAY_UNIT(CLASS_NAME, STORAGE_TYPE)         \
  namespace uxdevice {                                                         \
  using CLASS_NAME = class CLASS_NAME : public display_unit_t,                 \
        std::enable_shared_from_this<CLASS_NAME> {                             \
  public:                                                                      \
    CLASS_NAME() : value(STORAGE_TYPE{}) {}                                    \
    CLASS_NAME(const STORAGE_TYPE &o) : value(o) {}                            \
    CLASS_NAME &operator=(const CLASS_NAME &other) {                           \
      display_unit_t::operator=(other);                                        \
      return *this;                                                            \
    }                                                                          \
    CLASS_NAME &operator=(CLASS_NAME &&other) noexcept {                       \
      index_by_t::operator=(other);                                            \
      return *this;                                                            \
    }                                                                          \
    CLASS_NAME(const CLASS_NAME &other)                                        \
        : display_unit_t(other), value(other.value) {}                         \
    CLASS_NAME(CLASS_NAME &&other)                                             \
    noexcept : display_unit_t(other), value(other.value) {}                    \
                                                                               \
    virtual ~CLASS_NAME() {}                                                   \
                                                                               \
    void invoke(display_context_t &context);                                   \
                                                                               \
    TYPED_INDEX_IMPLEMENTATION(CLASS_NAME)                                     \
    HASH_OBJECT_MEMBERS(display_unit_t::hash_code(), HASH_TYPE_ID_THIS, value) \
                                                                               \
    STORAGE_TYPE value;                                                        \
  };                                                                           \
  }                                                                            \
  STD_HASHABLE(uxdevice::CLASS_NAME);

/**
\internal
\def DECLARE_STORAGE_EMITTER_DISPLAY_UNIT(CLASS_NAME, STORAGE_TYPE)
\param CLASS_NAME - the name the display unit should assume.
\param STORAGE_TYPE - the storage class or trivial type.

\brief provides the flexibility to store associated data. the invoke
public method is called. The class has a public member named "value" of the
given type within the second macro parameter.
*/
#define DECLARE_STORAGE_EMITTER_DRAWING_FUNCTION(CLASS_NAME, STORAGE_TYPE)     \
  namespace uxdevice {                                                         \
  using CLASS_NAME = class CLASS_NAME : public drawing_output_t,               \
        std::enable_shared_from_this<CLASS_NAME> {                             \
  public:                                                                      \
    CLASS_NAME &operator=(const CLASS_NAME &other) {                           \
      drawing_output_t::operator=(other);                                      \
      return *this;                                                            \
    }                                                                          \
    CLASS_NAME &operator=(CLASS_NAME &&other) noexcept {                       \
      index_by_t::operator=(other);                                            \
      return *this;                                                            \
    }                                                                          \
    CLASS_NAME(const CLASS_NAME &other)                                        \
        : drawing_output_t(other), value(other.value) {}                       \
    CLASS_NAME(CLASS_NAME &&other)                                             \
    noexcept : drawing_output_t(other), value(other.value) {}                  \
                                                                               \
    virtual ~CLASS_NAME() {}                                                   \
                                                                               \
    void invoke(display_context_t &context);                                   \
                                                                               \
    TYPED_INDEX_IMPLEMENTATION(CLASS_NAME)                                     \
    HASH_OBJECT_MEMBERS(drawing_output_t::hash_code(), HASH_TYPE_ID_THIS,      \
                        value)                                                 \
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
  using CLASS_NAME = class CLASS_NAME : public listener_t,                     \
        std::enable_shared_from_this<CLASS_NAME> {                             \
  public:                                                                      \
    CLASS_NAME(const event_handler_t &_dispatch_event)                         \
        : listener_t(listener_storage_t{std::type_index(typeid(this)),         \
                                        _dispatch_event}) {}                   \
    CLASS_NAME &operator=(const CLASS_NAME &other) {                           \
      listener_t::operator=(other);                                            \
      return *this;                                                            \
    }                                                                          \
    CLASS_NAME &operator=(CLASS_NAME &&other) noexcept {                       \
      listener_t::operator=(other);                                            \
      return *this;                                                            \
    }                                                                          \
    CLASS_NAME(const CLASS_NAME &other) : listener_t(other) {}                 \
    CLASS_NAME(CLASS_NAME &&other) noexcept : listener_t(other) {}             \
                                                                               \
    virtual ~CLASS_NAME() {}                                                   \
                                                                               \
    HASH_OBJECT_MEMBERS(listener_t::hash_code(), HASH_TYPE_ID_THIS)            \
    TYPED_INDEX_IMPLEMENTATION(CLASS_NAME)                                     \
  };                                                                           \
  }                                                                            \
  STD_HASHABLE(uxdevice::CLASS_NAME);
