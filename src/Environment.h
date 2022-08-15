#pragma once

#include "TokenTypes.h"
#include <iostream>
#include <memory>
#include <string>
#include <type_traits>
#include <unordered_map>

#include "thirdparty/bytell_hash_map.hpp"
#include "thirdparty/flat_hash_map.hpp"
#include "thirdparty/robin_hood.h"
#include "thirdparty/robin_hood_map.h"
#include "thirdparty/unordered_dense.h"

#include "thirdparty/tsl/robin_map.h"

#include "ExceptionError.h"
#include "Utilities.hpp"
#include "thirdparty/plf_colony.h"

template <typename S> struct hasher {
    size_t operator()(const S& x) const {
        /* your code here, e.g. "return hash<int>()(x.value);" */
        return FNVHash(x);
    }
};

namespace cpplox {
struct Environment { //}: std::enable_shared_from_this<Environment> {
    // Environment() = default;
    // address of a ref is the same as taking the address of the object it
    // refers to
    // explicit Environment(
    //     const std::shared_ptr<Environment>& environment = nullptr)
    //     : enclosing(environment) {
    // }

    explicit Environment(Environment* environment = nullptr)
        : enclosing(environment) {
    }

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
        return {};
    }

    void define(const std::string& name, const Object& value) {
        values.insert_or_assign(name, value);
    }

    void assignAt(int distance, const Token& name, const Object& value) {
        ancestor(distance)->values.insert_or_assign(name.lexeme, value);
    }
    void assign(const Token& name, const Object& value) {
        if (const auto search = values.find(name.lexeme);
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
    // std::unordered_map<std::string, Object> values;
    // robin_hood::unordered_map<std::string, Object> values;
    tsl::robin_map<std::string, Object> values;

    //ankerl::unordered_dense::map<std::string, Object> values;

    int handle = -1;
    // plf::colony<Environment>::iterator it;
    //ska::bytell_hash_map<std::string, Object, hasher<std::string>> values;
    // ska::flat_hash_map<std::string, Object> values;
};

// static_assert(std::is_copy_constructible_v<Environment>, "");
// static_assert(std::is_copy_assignable_v<Environment>, "");
// static_assert(std::is_move_assignable_v<Environment>, "");

} // namespace cpplox