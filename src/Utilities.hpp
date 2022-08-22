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

inline std::vector<std::string> split(std::string stringIn, char c = ' ') {
    std::vector<std::string> result;
    const char* str = stringIn.c_str();

    do {
        const char* begin = str;

        while (*str != c && *str)
            str++;

        result.push_back(std::string(begin, str));
    } while (0 != *str++);

    return result;
}

// trim from end (in place)
static inline void rtrim(std::string &s) {
    s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char ch) {
        return !std::isspace(ch);
    }).base(), s.end());
}

inline bool safeToReuse = false;

// recursive wrapper with single vector for each type
template <typename T> struct recursive_wrapper {
    // construct from an existing object

    recursive_wrapper(T&& t_) {
        // t.push_back(std::move(t_));
        index = t.push(std::forward<T>(t_));
        // std::cout << "increasing\n";
        //  t.push_back(std::forward<T>(t_));
        //  index = t.size() - 1;
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
    int index = -1;
    // std::basic_string<T> t;
};

// template <typename T> inline std::vector<T> recursive_wrapper<T>::t;
template <typename T> inline sparestack<T> recursive_wrapper<T>::t;

// recursive wrapper with single vector for each type
template <typename T> struct recursive_wrapper2 {
    // construct from an existing object

    recursive_wrapper2(T&& t_) {
        // t.push_back(std::move(t_));
        index = t.push(std::forward<T>(t_));
        // std::cout << "increasing\n";
        // t.push_back(std::forward<T>(t_));
        // index = t.size() - 1;
    }

    // ~recursive_wrapper2() {

    //     if (safeToReuse) [[likely]] {
    //         //std::cout << "erasing\n";

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
    int index = -1;
    // std::basic_string<T> t;
};

// template <typename T> inline std::vector<T> recursive_wrapper<T>::t;
template <typename T> inline sparestack<T> recursive_wrapper2<T>::t;

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



/*
char key, key2, key3;

            system("stty raw");

            key = getchar();

            system("stty cooked");

            if (key == 27 && !backspace) {
                system("stty raw");

                key2 = getchar();
                 std::cout << key2;
                std::cout << key3;

                key3 = getchar();
                if (key3 == 65) {
                    std::cout << "\r" << lastLine;
                }
                if(key3 == 8){
                    backspace = true;
                    std::cout << "BS\n";
                }
                system("stty cooked");
                runLastLine = true;
            } else if (key == 3) {
                raise(SIGINT);
            }

            else {
                // system("stty cooked");
                if (runLastLine) {
                    currentLine = lastLine;
                    std::cout << "\r" << lastLine;
                    std::cin >> lastLine;
                    std::cin.ignore();

                } else {

                    std::cout << key;
                }
                */