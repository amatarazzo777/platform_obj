/**
\author Anthony Matarazzo
\file uxstdmacros.hpp
\date 5/12/20
\version 1.0
 \details  macros that minimize the maintained of member that are summarized
 in a has value.

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
