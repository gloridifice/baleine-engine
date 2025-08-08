#pragma once
#include <optional>

namespace baleine {

template<typename T>

using Option = std::optional<T>;

template<typename T>
static Option<T> Some(T&& value) {
    return std::make_optional(value);
}

inline constexpr std::nullopt_t None {std::nullopt_t::_Construct::_Token};

}