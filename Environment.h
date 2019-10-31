#pragma once

#include "ExceptionError.h"
#include "TokenTypes.h"
#include <string>
#include <unordered_map>
#include <iostream>
#include <type_traits>
#include <memory>

namespace cpplox {
struct Environment {
    //Environment() = default;
    // address of a ref is the same as taking the address of the object it refers to
    explicit Environment(const std::shared_ptr<Environment> environment = nullptr) : enclosing(environment) {
    }
    // Environment(const Environment&) = delete;
    // Environment(Environment&&) = delete;

    const std::shared_ptr<Environment> enclosing;
    // ~Environment() {
    //     if (enclosing != nullptr) {
    //         // set back env when this goes goes out of scope. should be execption safe. it might
    //         // have been changed bhy other blocks with their own environements
    //         *enclosing = *this;
    //     }
    // }
    //Environment(const Environment&)=default;

    const Object& get(const Token& name) const {
        if (auto search = values.find(name.lexeme);
            search != values.end()) { // if init version of contains()
            return search->second;
        } else if (enclosing != nullptr) {
            //std::cout << "couldnt find so getting " << name.lexeme << "\n"; // if not in current scope, check parent
            return enclosing->get(name);
        } else {
            throw RuntimeError(name, "Undefined variable '" + name.lexeme + "'.");
        }
    }

    void define(std::string name, Object value) {
        values.insert_or_assign(name, value);
    }

    void assign(Token name, Object value) {
        if (auto search = values.find(name.lexeme);
            search != values.end()) { // if init version of contains()
            values.insert_or_assign(name.lexeme, value);
            return;
        } else if (enclosing != nullptr) {
            enclosing->assign(name, value);
            return;
        } else {
            throw RuntimeError(name, "Undefined variable '" + name.lexeme + "'.");
        }
    }

    std::unordered_map<std::string, Object> values;
};

// static_assert(std::is_copy_constructible_v<Environment>, "");
// static_assert(std::is_copy_assignable_v<Environment>, "");
//static_assert(std::is_move_assignable_v<Environment>, "");





} // namespace cpplox