#pragma once

#include <exception>

namespace baleine {

using Exception = std::exception;
using ExceptionPtr = std::exception_ptr;

using LogicError = std::logic_error;
using RangeError = std::range_error;

}
