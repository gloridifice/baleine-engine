#pragma once
#include <optional>

namespace baleine {

template<typename T>

using Option = std::optional<T>;

inline constexpr std::nullopt_t None {std::nullopt_t::_Construct::_Token};

}