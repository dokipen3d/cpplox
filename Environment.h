#pragma once

#include "ExceptionError.h"
#include "TokenTypes.h"
#include <string>
#include <unordered_map>

namespace cpplox {
struct Environment {

    Object& get(const Token& name) {
        if (auto search = values.find(name.lexeme);
            search != values.end()) { // if init version of contains()
            return search->second;
        } else {

            throw RuntimeError(name,
                               "Undefined variable '" + name.lexeme + "'.");
        }
    }

    void define(std::string name, Object value) {
        values.insert_or_assign(std::move(name), std::move(value));
    }

    void assign(Token name, Object value) {
        if (auto search = values.find(name.lexeme);
            search != values.end()) { // if init version of contains()
            values.insert_or_assign(name.lexeme, std::move(value));
            return;
        }

        throw RuntimeError(name, "Undefined variable '" + name.lexeme + "'.");
    }

    std::unordered_map<std::string, Object> values;
};
} // namespace cpplox