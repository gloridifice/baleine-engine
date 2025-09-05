#pragma once

#include "absl/synchronization/mutex.h"

namespace baleine {

using Mutex = absl::Mutex;

template<typename T>
class LockGuard {
    T& value;
    absl::MutexLock mutex_lock;

  public:
    ~LockGuard() = default;

    explicit LockGuard(absl::Mutex& mutex, T& value) :
        mutex_lock(&mutex),
        value(value) {}

    T& operator*() const {
        return value;
    }
};

template<typename T>
class ReadGuard {
    const T& value;
    absl::ReaderMutexLock mutex_lock;

  public:
    ~ReadGuard() = default;

    explicit ReadGuard(absl::Mutex& mutex, T& value) :
        mutex_lock(&mutex),
        value(value) {}

    const T& operator*() const {
        return value;
    }
};

template<typename T>
class MutexVal {
  private:
    Mutex mutex {};
    T value;

  public:
    ~MutexVal() = default;

    explicit MutexVal(T&& value) : value(value) {}

    LockGuard<T> lock() {
        return LockGuard<T>(mutex, value);
    }

    ReadGuard<T> read_lock() {
        return ReadGuard<T>(mutex, value);
    }

    MutexVal(MutexVal&& val) noexcept {
        auto guard = val.lock();
        value = std::move(*guard);
    }

    MutexVal& operator=(MutexVal&& val) noexcept {
        auto guard = val.lock();
        value = std::move(*guard);
        return *this;
    }

    MutexVal(const MutexVal&) = delete;
    MutexVal& operator=(const MutexVal&) = delete;
};

} // namespace baleine
