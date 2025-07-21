#pragma once

#include <functional>

template<typename T>
using Fn = std::function<T>;

template<typename T>
using Ref = std::reference_wrapper<T>;