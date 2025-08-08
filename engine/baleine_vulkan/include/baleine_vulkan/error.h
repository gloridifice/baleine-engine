#pragma once
#include <exception>

#include "baleine_type/string.h"
#include "fmt/format.h"

class CreationException final: public std::exception {
  private:
    std::__cow_string error;
    std::__cow_string creation_object_name;
    std::string message;

  public:
    explicit CreationException(
        const String& creation_object_name,
        const String& error
    ) :
        error(error),
        creation_object_name(creation_object_name) {
        message = fmt::format(
            "Failed to create <{}>, caused by error: {}",
            creation_object_name,
            error
        );
    }

    [[nodiscard]] const char* what() const noexcept override {
        return message.c_str();
    }
};