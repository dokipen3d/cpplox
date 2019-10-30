#pragma once

#include "ExceptionError.h"
#include "TokenTypes.h"
#include <string>
#include <unordered_map>
#include <iostream>
#include <type_traits>
#include <memory>
#include <vector>
namespace cpplox {
struct Environment {
    //Environment() = default;
    // address of a ref is the same as taking the address of the object it refers to
    explicit Environment(std::vector<Environment>& stack, int parent) : stack(stack), parent(parent) {
        std::cout << "calling cnstr\n";
    }
    // Environment(const Environment&) = delete;
    // Environment(Environment&&) = delete;
    std::vector<Environment>& stack;
    int parent = -1;
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
        } else if (parent != -1) {
            //std::cout << "couldnt find so getting " << name.lexeme << "\n"; // if not in current scope, check parent
            return stack[parent].get(name);
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
        } else if (parent != -1) {
            stack[parent].assign(name, value);
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