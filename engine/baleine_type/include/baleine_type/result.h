
#pragma once

#include "exception.h"
#include "functional.h"
#include "memory.h"
#include "optional.h"

namespace baleine {
template<typename T>
class Result {
    Option<T> value;
    Option<ExceptionPtr> error;

  private:
    T& get() {
        return value.value();
    }

    T take() {
        T val = std::move(value.value());
        value.reset();
        return val;
    }

  public:
    explicit Result(Option<T>&& value, Option<ExceptionPtr>&& error) :
        value(std::move(value)),
        error(std::move(error)) {}

    Result ok(T value) {
        return Result(std::move(value), None);
    }

    Result err(ExceptionPtr exception_ptr) {
        return Result(None, std::move(exception_ptr));
    }

    bool is_ok() const {
        return value.has_value();
    }

    bool is_err() const {
        return error.has_value();
    }

    ExceptionPtr& err() {
        return error.value();
    }

    void inspect(Fn<void(T&)> fun) {
        if (is_ok())
            fun(get());
    }

    T unwrap() {
        if (is_err())
            throw error.value();
        return take();
    }

    T unwrap_or(T&& or_value) {
        if (is_err())
            return or_value;
        return take();
    }

    bool is_valid() const {
        return is_ok() || is_err();
    }
};
} // namespace baleine