#pragma once
#include "thirdparty/sparestack.hpp"
#include <string>
#include <utility>
#include <vector>
#include <variant>

template <class T> constexpr std::string_view type_name() {
    using namespace std;
#ifdef __clang__
    string_view p = __PRETTY_FUNCTION__;
    return string_view(p.data() + 34, p.size() - 34 - 1);
#elif defined(__GNUC__)
    string_view p = __PRETTY_FUNCTION__;
#if __cplusplus < 201402
    return string_view(p.data() + 36, p.size() - 36 - 1);
#else
    return string_view(p.data() + 49, p.find(';', 49) - 49);
#endif
#elif defined(_MSC_VER)
    string_view p = __FUNCSIG__;
    return string_view(p.data() + 84, p.size() - 84 - 7);
#endif
}

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

struct Assign;
struct Binary;
struct Grouping;
struct Variable;
struct Unary;
struct Logical;
struct Call;
struct Increment;
struct Decrement;
struct Get;
struct Set;

struct BlockStatement;
struct IfStatement;
struct WhileStatement;
struct FunctionStatement;
struct ClassStatement;

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
static inline void rtrim(std::string& s) {
    s.erase(std::find_if(s.rbegin(), s.rend(),
                         [](unsigned char ch) { return !std::isspace(ch); })
                .base(),
            s.end());
}

inline bool safeToReuse = false;

// recursive wrapper with single vector for each type
template <typename T> struct recursive_wrapper {
    // construct from an existing object

    recursive_wrapper(T&& t_, std::vector<T>* storageIn = nullptr);
    recursive_wrapper(recursive_wrapper const&) = default;
    recursive_wrapper(recursive_wrapper&&) = default;

    recursive_wrapper& operator=(const recursive_wrapper& other) = default;
    recursive_wrapper& operator=(recursive_wrapper&& other) = default;

    // ~recursive_wrapper() {
    //     std::cout << "oos " << type_name<T>() << " " << index << " \n";

    //     // if (safeToReuse) [[likely]] {
    //     //     t.eraseAt(index);
    //     // }
    // }
    // recursive_wrapper(const recursive_wrapper& operand) = default;

    // recursive_wrapper(recursive_wrapper&& operand) = default;

    // recursive_wrapper& operator=(const recursive_wrapper& rhs)  = default;
    // recursive_wrapper& operator=(recursive_wrapper&& rhs) = default;

    // cast back to wrapped type
    // operator const T &()  { return t.front(); }
    operator const T&() const {
        //return (*test)[index];
        return test[0];
    }
    operator T&() {
        return test[0];
    }

    // bool operator==(const recursive_wrapper<T>& other) const {
    //     return (t[index] == static_cast<T>(other)) ;
    // }

    // if we ever need a non const ref back
    // operator T&() {
    //     return t[index];
    // }

    // store the value
    // static sparestack<T> t;
    //uint32_t storageIndex = 0;
    // std::basic_string<T> t;
    uint32_t index = -1;
    std::vector<T> test;

    static std::vector<std::vector<T>> t;
};

// template <typename T> inline std::vector<T> recursive_wrapper<T>::t;
// we can now use an index in each recursive wrapper to index into a uniquw
// storage per script. perf wasn't majorly affected (tested fib 35) but it was
// when we added the storage index. (3.3s to 3.75s 12% slower).
template <typename T>
inline std::vector<std::vector<T>> recursive_wrapper<T>::t;

// we define the constructor outside of the class for now so that we can test using the static vectors for now
// once we have the parser and interpreter passing in the unique storage vectors, we can stop using the static ones.
// using the stored vector is a minimal perf hit and goes from ~3.3 to ~3.55 seconds on fibonacci. next step would 
// be to try a tagged pointer to store the wrapper index in the pointer bits
template <typename T> 
recursive_wrapper<T>::recursive_wrapper(T&& t_, std::vector<T>* storageIn){//} : test{&recursive_wrapper<T>::t[0]} {
        // t.push_back(std::move(t_));
        // std::cout << "got here: " << type_name<T>() << "\n";
        test.emplace_back(std::forward<T>(t_));
        // t[storageIndex].push_back(std::forward<T>(t_));
        // test = &t[0];
        //  std::cout << "increasing\n";
        //   t.push_back(std::forward<T>(t_));
        //index = test->size() - 1;
};
//
//template<typename T>
//struct wrapper {
//    wrapper(const T& t) {
//        p = std::make_unique<T>(t);
//    }
//
//    wrapper(const wrapper& other) {
//        p = std::make_unique<T>(*other.p.get());
//    }
//
//    wrapper& operator= (const wrapper& other) {
//        p = std::make_unique<T>(*other.p.get());
//        return *this;
//    }
//    ~wrapper() = default;
//
//    operator const T&() const {
//        return *p;
//    }
//    operator T&() {
//        return *p;
//    }
//
//    std::unique_ptr<T> p;
//    // auto operator <=> (const wrapper& other) const {
//    //     return p <=> other.p;
//    // }
//};



template <typename T> struct wrapper {
    wrapper(const T& t) {
        index = storage.push(t);
        useCount++;


    }

    wrapper(const wrapper& other) {
        index = other.index;
        useCount++;

    }

    wrapper& operator=(const wrapper& other) {
        index = other.index;
        useCount++;

        return *this;
    }
    ~wrapper() {
        useCount--;
        //std::cout << "Use" << useCount << "\n"; 
        if (useCount == 1) {
            storage.eraseAt(index);
        }
    }

    operator const T&() const {
        return storage[index];
    }
    operator T&() {
        return storage[index];
    }

    static sparestack<T> storage;
    int index = -1;
    int useCount = 0;

// auto operator <=> (const wrapper& other) const {
    //     return p <=> other.p;
    // }
};

template <typename T> inline sparestack<T> wrapper<T>::storage;


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

template <class... Ts> struct overloaded : Ts... {
    using Ts::operator()...;
};
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