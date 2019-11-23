#pragma once
#include <string>
#include <vector>

namespace cpplox {

// recursive wrapper with single vector for each type
template <typename T> struct recursive_wrapper {
    // construct from an existing object
    recursive_wrapper(T t_) {
        t.push_back(std::move(t_));
        index = t.size() - 1;
    }
    // cast back to wrapped type
    // operator const T &()  { return t.front(); }
    operator const T&() const {
        return t[index];
    }
    operator T&() {
        return t[index];
    }
    //if we ever need a non const ref back
    // operator T&() {
    //     return t[index];
    // }

    // store the value
    static std::vector<T> t;
    size_t index;
    // std::basic_string<T> t;
};

template <typename T> inline std::vector<T> recursive_wrapper<T>::t;

inline void stripZerosFromString(std::string& text) {
    text.erase(text.find_last_not_of('0') + 1, std::string::npos);
    text.erase(text.find_last_not_of('.') + 1, std::string::npos);
}


// template interface. placeholder is just to make a paramter
// we can specialize on for the specialization
template <typename T, typename VariantPlaceholder>
struct has_type;

// this speecializes on std variant with no types. base/terminating case
template <typename T>
struct has_type<T, std::variant<>> : std::false_type {};

// this is the case where the first type is not T. in that case 
// inherit from the next specialization and forward the parameter pack.
template <typename T, typename U, typename... Ts>
struct has_type<T, std::variant<U, Ts...>> : has_type<T, std::variant<Ts...>> {};

// if the above template inherited from this one (ie this one matches and is specialized)
// then it will be the final one chosen and be true
template <typename T, typename... Ts>
struct has_type<T, std::variant<T, Ts...>> : std::true_type {};

template <typename T, typename VariantPlaceholder>
inline constexpr bool has_type_v = has_type<T, VariantPlaceholder>::value;


} // namespace cpplox
