#pragma once

#include <atomic>

namespace baleine {

template<class T>
using Atomic = std::atomic<T>;

}
