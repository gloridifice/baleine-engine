#pragma once

#include <type_traits>

// 重载位运算符的宏
#define ENABLE_BITMASK_OPERATORS(EnumType) \
inline EnumType operator|(EnumType a, EnumType b) { \
using T = std::underlying_type_t<EnumType>; \
return static_cast<EnumType>(static_cast<T>(a) | static_cast<T>(b)); \
} \
inline EnumType operator&(EnumType a, EnumType b) { \
using T = std::underlying_type_t<EnumType>; \
return static_cast<EnumType>(static_cast<T>(a) & static_cast<T>(b)); \
} \
inline EnumType operator^(EnumType a, EnumType b) { \
using T = std::underlying_type_t<EnumType>; \
return static_cast<EnumType>(static_cast<T>(a) ^ static_cast<T>(b)); \
} \
inline EnumType operator~(EnumType a) { \
using T = std::underlying_type_t<EnumType>; \
return static_cast<EnumType>(~static_cast<T>(a)); \
} \
inline EnumType& operator|=(EnumType& a, EnumType b) { \
a = a | b; \
return a; \
} \
inline EnumType& operator&=(EnumType& a, EnumType b) { \
a = a & b; \
return a; \
} \
inline EnumType& operator^=(EnumType& a, EnumType b) { \
a = a ^ b; \
return a; \
}
