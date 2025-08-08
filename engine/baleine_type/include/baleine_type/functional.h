#pragma once

#include <functional>

namespace baleine {

template<typename T>
using Fn = std::function<T>;

template<typename T>
using Ref = std::reference_wrapper<T>;

}
