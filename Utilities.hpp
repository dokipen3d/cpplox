#pragma once
#include <vector>
#include <string>

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
    operator const T&() {
        return t[index];
    }

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
} // namespace cpplox
