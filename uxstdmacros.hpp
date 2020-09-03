/**
\author Anthony Matarazzo
\file uxevent.hpp
\date 5/12/20
\version 1.0
 \details  paint class

*/
#pragma once
#define STD_HASHABLE(CLASS_NAME)                                               \
  template <> struct std::hash<CLASS_NAME> {                                   \
    std::size_t operator()(CLASS_NAME const &o) const noexcept {               \
      return o.hash_code();                                                    \
    }                                                                          \
  }

#define HASH_OBJECT_MEMBERS(...)                                               \
  std::size_t hash_code(void) const noexcept {                                 \
    std::size_t value = {};                                                    \
    hash_combine(value, __VA_ARGS__);                                          \
    return value;                                                              \
  }

#define HASH_OBJECT_MEMBERS_CONTAINING_VECTOR(VECTOR_NAME, CLASS_NAME, ...)    \
  std::size_t hash_code(void) const noexcept {                                 \
    std::size_t value = {};                                                    \
    std::for_each(VECTOR_NAME.begin(), VECTOR_NAME.end(),                      \
                  [&value](const CLASS_NAME &n) {                              \
                    hash_combine(value, n.hash_code());                        \
                  });                                                          \
    hash_combine(value, __VA_ARGS__);                                          \
    return value;                                                              \
  }
