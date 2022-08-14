#pragma once
#include "thirdparty/sparestack.hpp"
#include <string>
#include <utility>
#include <vector>

static unsigned int FNVHash(std::string str) {
    const unsigned int fnv_prime = 0x811C9DC5;
    unsigned int hash = 0;
    unsigned int i = 0;
    unsigned int len = str.length();

    for (i = 0; i < len; i++) {
        hash *= fnv_prime;
        hash ^= (str[i]);
    }

    return hash;
}

namespace cpplox {

inline bool safeToReuse = false;

// recursive wrapper with single vector for each type
template <typename T> struct recursive_wrapper {
    // construct from an existing object

    recursive_wrapper(T&& t_) {
        // t.push_back(std::move(t_));
        index = t.push(std::forward<T>(t_));
        std::cout << "increasing\n";
        // t.push_back(std::forward<T>(t_));
        // index = t.size() - 1;
    }

    // ~recursive_wrapper() {

    //     if (safeToReuse) [[likely]] {
    //         t.eraseAt(index);
    //     }
    // }
    // recursive_wrapper(const recursive_wrapper& operand) = default;

    // recursive_wrapper(recursive_wrapper&& operand) = default;


    // recursive_wrapper& operator=(const recursive_wrapper& rhs)  = default;
    // recursive_wrapper& operator=(recursive_wrapper&& rhs) = default;

    // cast back to wrapped type
    // operator const T &()  { return t.front(); }
    operator const T&() const {
        return t[index];
    }
    operator T&() {
        return t[index];
    }

    // bool operator==(const recursive_wrapper<T>& other) const {
    //     return (t[index] == static_cast<T>(other)) ;
    // }

    // if we ever need a non const ref back
    // operator T&() {
    //     return t[index];
    // }

    // store the value
    // static std::vector<T> t;
    static sparestack<T> t;
    size_t index = -1;
    // std::basic_string<T> t;
};

// template <typename T> inline std::vector<T> recursive_wrapper<T>::t;
template <typename T> inline sparestack<T> recursive_wrapper<T>::t;

inline void stripZerosFromString(std::string& text) {
    text.erase(text.find_last_not_of('0') + 1, std::string::npos);
    text.erase(text.find_last_not_of('.') + 1, std::string::npos);
}

// template interface. placeholder is just to make a paramter
// we can specialize on for the specialization
template <typename T, typename VariantPlaceholder> struct has_type;

// this speecializes on std variant with no types. base/terminating case
template <typename T> struct has_type<T, std::variant<>> : std::false_type {};

// this is the case where the first type is not T. in that case
// inherit from the next specialization and forward the parameter pack.
template <typename T, typename U, typename... Ts>
struct has_type<T, std::variant<U, Ts...>> : has_type<T, std::variant<Ts...>> {
};

// if the above template inherited from this one (ie this one matches and is
// specialized) then it will be the final one chosen and be true
template <typename T, typename... Ts>
struct has_type<T, std::variant<T, Ts...>> : std::true_type {};

template <typename T, typename VariantPlaceholder>
inline constexpr bool has_type_v = has_type<T, VariantPlaceholder>::value;

// should be in c++20 but isn't
//  allows std::visit(overloaded([](typeB& A){},
//                               [](typeB& B){}))

template <class... Ts> struct overloaded : Ts... { using Ts::operator()...; };
template <class... Ts> overloaded(Ts...) -> overloaded<Ts...>;

template <class F> class finally {
  public:
    explicit finally(F f) noexcept : f_(std::move(f)), invoke_(true) {
    }

    finally(finally&& other) noexcept
        : f_(std::move(other.f_)), invoke_(other.invoke_) {
        other.invoke_ = false;
    }

    finally(const finally&) = delete;
    finally& operator=(const finally&) = delete;

    ~finally() noexcept {
        if (invoke_)
            f_();
    }

  private:
    F f_;
    bool invoke_;
};

} // namespace cpplox
