
#pragma once

#include "exception.h"
#include "functional.h"
#include "memory.h"
#include "optional.h"


namespace baleine {
/**
 * A rust-like result. It can be a 'value' or an 'error'. It follows the RAII paradigm.
 * 
 * - A 'value' contains a T type variable.
 *  - use static function @c Ok(value) to create a 'value'.
 * - An 'error' contains an exception.
 *  - use static function @c Err(exception) to create a 'error'.
 *
 * Use @c unwrap() to get the value inside or throw the exception.
 *
 * @code
 *  
 * @endcode
 */
template<typename T>
class Result {
    Option<T> value;
    Option<Unique<Exception>> error;

  private:
    void try_throw_error() {
        if (error.has_value())
            throw std::move(error.value());
        error.reset();
    }
    
    T& get() {
        return value.value();
    }

    T take() {
        auto v = std::move(value.value());
        value.reset();
        return v;
    }

  public:
    explicit Result(Option<T>&& value, Option<Unique<Exception>>&& error) :
        value(std::move(value)),
        error(std::move(error)) {}

    static Result new_ok(T value) {
        return Result(std::move(value), None);
    }

    static Result new_err(Unique<Exception>&& exception_ptr) {
        return Result(None, std::move(exception_ptr));
    }

    bool is_ok() const {
        return value.has_value();
    }

    bool is_err() const {
        return error.has_value();
    }

    void inspect(Fn<void(T&)> fun) {
        if (is_ok())
            fun(get());
    }

    T& peek() {
        try_throw_error();
        return get();
    }
    
    Exception& peek_err() const {
        return *error.value();
    }

    Unique<Exception> unwrap_err() {
        auto err = std::move(error.value());
        error.reset();
        return err;
    }

    Option<T> ok() {
        if (is_ok()) return std::move(value);
        return None;
    }

    T unwrap() {
        try_throw_error();
        return take();
    }

    T unwrap_or(T&& or_value) {
        if (is_err())
            return or_value;
        return take();
    }

    T unwrap_or_else(Fn<T()> fun) {
        if (is_err())
            return fun();
        return take();
    }

    bool is_valid() const {
        return is_ok() || is_err();
    }
};

/**
 * @return Return a @c Result with given @c T type value.
 */
template<typename T>
static Result<T> Ok(T value) {
    return Result<T>::new_ok(value);
}

/**
 * @return Return a @c Result with given @c Exception.
 */
template<typename T>
static Result<T> Err(Unique<Exception>&& exception) {
    return Result<T>::new_err(std::move(exception));
}
}