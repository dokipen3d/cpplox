#pragma once




#include "TokenTypes.h"
#include <iostream>
#include <memory>
#include <string>
#include <type_traits>
#include <unordered_map>

#include "thirdparty/bytell_hash_map.hpp"
#include "thirdparty/plf_colony.h"
#include "ExceptionError.h"

namespace cpplox {
struct Environment { //}: std::enable_shared_from_this<Environment> {
    // Environment() = default;
    // address of a ref is the same as taking the address of the object it
    // refers to
    // explicit Environment(
    //     const std::shared_ptr<Environment>& environment = nullptr)
    //     : enclosing(environment) {
    // }

    explicit Environment(
        Environment* environment = nullptr)
        : enclosing(environment) {
        //std::cout << "env\n";
    }
    /*Environment() = delete;
    Environment(const Environment& other) {
        std::cout << "copy\n";
        enclosing = other.enclosing;
        values = other.values;
    };*/

    //Environment(Environment&& other) {
    //    //std::cout << "move\n";

    //   enclosing = other.enclosing;
    //   values = other.values;
    //   handle = other.handle;

    //};
    
    //Environment& operator=(const Environment& other) {
    //    //std::cout << "copy assignment of env\n";
    //    enclosing = other.enclosing;
    //    values = other.values;
    //    handle = other.handle;
    //    return *this;
    //}
    //Environment(Environment&& other) = default;
    //Environment(Environment&& other) = default;

    //~Environment(){
    //    std::cout << "AAAAA\n";
    //}




    Environment* ancestor(int distance) {
        Environment* environmentLocal = this;
        for (int i = 0; i < distance; i++) {

            environmentLocal = environmentLocal->enclosing;
        }

        return environmentLocal;
    }

    const Object& get(const Token& name) const {
        if (auto search = values.find(name.lexeme);
            search != values.end()) { // if init version of contains()
            return search->second;
        } else if (enclosing != nullptr) {
            // if not in current scope, check parent
            return enclosing->get(name);
        } else {
            throw RuntimeError(name,
                               "Undefined variable '" + name.lexeme + "'.");
        }
    }

    Object getAt(int distance, const std::string& name) {
        Environment* anc = ancestor(distance);
        const auto& varmap = anc->values;
        auto env = varmap.find(name);
        if (env != varmap.end()) {
            return env->second;
        }
    }

    void define(std::string name, const Object& value) {
        values.insert_or_assign(name, value);
    }

    void assignAt(int distance, const Token& name, const Object& value) {
        ancestor(distance)->values.insert_or_assign(name.lexeme, value);
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
            throw RuntimeError(name,
                               "Undefined variable '" + name.lexeme + "'.");
        }
    }

    // std::shared_ptr<Environment> enclosing;
    Environment* enclosing = nullptr;
    std::unordered_map<std::string, Object> values;
    int handle = -1;
    //plf::colony<Environment>::iterator it;
    //ska::bytell_hash_map<std::string, Object> values;

};

// static_assert(std::is_copy_constructible_v<Environment>, "");
// static_assert(std::is_copy_assignable_v<Environment>, "");
// static_assert(std::is_move_assignable_v<Environment>, "");

} // namespace cpplox