#pragma once

#include <exception>

namespace baleine {

using Exception = std::exception;
using ExceptionPtr = std::exception_ptr;

using LogicError = std::logic_error;
using RuntimeError = std::runtime_error;
using SystemError = std::system_error;
using OverflowError = std::overflow_error;
using RangeError = std::range_error;
using BadAlloc = std::bad_alloc;
using BadCast = std::bad_cast;
using BadException = std::bad_exception;
using BadWeakPtr = std::bad_weak_ptr;
using BadTypeId = std::bad_typeid;
using DomainError = std::domain_error;
using InvalidArgument = std::invalid_argument;
using LengthError = std::length_error;
using OutOfRange = std::out_of_range;

}
