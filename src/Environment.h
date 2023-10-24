#pragma once

#include "TokenTypes.h"
#include <iostream>
#include <memory>
#include <string>
#include <type_traits>
#include <unordered_map>

//#include "thirdparty/bytell_hash_map.hpp"
#include "thirdparty/flat_hash_map.hpp"
#include "thirdparty/robin_hood.h"
#include "thirdparty/robin_hood_map.h"
//#include "thirdparty/unordered_dense.h"

//#include "absl/container/flat_hash_map.h"
//#include "absl/container/node_hash_map.h"

#include "thirdparty/tsl/robin_map.h"

#include "ExceptionError.h"
#include "Utilities.hpp"
#include "boost/smart_ptr/local_shared_ptr.hpp"

//#include "thirdparty/shared_ptr.hpp"
template <typename S> struct hasher {
    size_t operator()(const S& x) const {
        /* your code here, e.g. "return hash<int>()(x.value);" */
        return FNVHash(x);
    }
};

namespace cpplox {
struct Environment {//}: std::enable_shared_from_this<Environment> {
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

            environmentLocal = environmentLocal->enclosing.get();
        }

        return environmentLocal;
    }

    const Object& get(const Token& name) const {
        if (auto search = values.find(name.hash);
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

    Object getAt(int distance, const Token& name) {
        Environment* anc = ancestor(distance);
        const auto& varmap = anc->values;
        if (auto env = varmap.find(name.hash); env != varmap.end()) {
            return env->second;
        }
        return {};
    }

    void define(const int32_t& name, const Object& value) {
        values.insert_or_assign(name, value);
    }


    template <typename T>
    void defineVal(const int32_t& name, T&& value) {
        values.insert_or_assign(name, std::forward<T>(value));
    }

    void assignAt(int distance, const Token& name, const Object& value) {
        ancestor(distance)->values.insert_or_assign(name.hash, value);
    }

    void assign(const Token& name, const Object& value) {
        if (const auto search = values.find(name.hash);
            search != values.end()) { // if init version of contains()
            values.insert_or_assign(name.hash, value);
            return;
        } else if (enclosing != nullptr) {
            enclosing->assign(name, value);
            return;
        } else {
            throw RuntimeError(name,
                               "Undefined variable '" + name.lexeme + "'.");
        }
    }

    //std::shared_ptr<Environment> enclosing;
    boost::local_shared_ptr<Environment> enclosing;

    // Environment* enclosing = nullptr;
    //std::unordered_map<std::string, Object> values;
    //robin_hood::unordered_map<std::string, Object> values;
    tsl::robin_map<int32_t, Object> values;

    // tsl::robin_map<std::string, Object, std::hash<std::string>,
    //                std::equal_to<std::string>,
    //                std::allocator<std::pair<std::string, Object>>,
    //                true> values;

    // ankerl::unordered_dense::map<std::string, Object> values;
    // absl::flat_hash_map<std::string, Object> values;
    //  absl::node_hash_map<std::string, Object> values;

    int handle = -1;

    // int refCount = 0;
    //  plf::colony<Environment>::iterator it;
    // ska::bytell_hash_map<std::string, Object, hasher<std::string>> values;
    //ska::flat_hash_map<std::string, Object> values;
};

// static_assert(std::is_copy_constructible_v<Environment>, "");
// static_assert(std::is_copy_assignable_v<Environment>, "");
// static_assert(std::is_move_assignable_v<Environment>, "");

} // namespace cpplox