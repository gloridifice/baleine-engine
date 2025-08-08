#pragma once

#include <memory>

namespace baleine {

template<class T>
using Unique = std::unique_ptr<T>;

template<class T>
using Shared = std::shared_ptr<T>;

template<class T>
using Weak = std::weak_ptr<T>;

template<class T>
using EnableSharedFromThis = std::enable_shared_from_this<T>;

using std::move;

}
