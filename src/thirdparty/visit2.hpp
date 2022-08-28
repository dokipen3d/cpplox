#pragma once

#include <type_traits>
#include <utility>

#ifdef __GNUC__ // GCC 4.8+, Clang, Intel and other compilers compatible with GCC (-std=c++0x or above)
[[noreturn]] inline __attribute__((always_inline)) void unreachable() {__builtin_unreachable();}
#elif defined(_MSC_VER) // MSVC
[[noreturn]] __forceinline void unreachable() {__assume(false);}
#else // ???
inline void unreachable() {}
#endif

namespace std {

template< class T >
struct remove_cvref {
    typedef std::remove_cv_t<std::remove_reference_t<T>> type;
};

template <class T> 
using remove_cvref_t = typename std::remove_cvref<T>::type;
}

namespace cpplox {

template <bool IsValid, typename R> struct dispatcher;

template <typename R> struct dispatcher<false, R> {
    template <std::size_t I, typename F, typename V>
    static constexpr R case_(F&&, V&&) {
        unreachable();
    }

    template <std::size_t B, typename F, typename V>
    static constexpr R switch_(F&&, V&&) {
       unreachable();
    }
};

template <typename R> struct dispatcher<true, R> {
    template <std::size_t I, typename F, typename V>
    static constexpr R case_(F&& f, V&& v) {
        using Expected = R;
        using Actual = decltype(std::invoke(std::forward<F>(f),
                                            std::get<I>(std::forward<V>(v))));
        static_assert(
            std::is_same_v<Expected, Actual>,
            "`mpark::visit` requires the visitor to have a single return type");
        return std::invoke(std::forward<F>(f), std::get<I>(std::forward<V>(v)));
    }

    template <std::size_t B, typename F, typename V>
    static constexpr R switch_(F&& f, V&& v) {

        constexpr std::size_t size =
            std::variant_size_v<std::remove_cvref_t<V>>;
        switch (v.index()) {
        case B + 0: {
            return dispatcher<B + 0 < size, R>::template case_<B + 0>(
                std::forward<F>(f), std::forward<V>(v));
        }
        case B + 1: {
            return dispatcher<B + 1 < size, R>::template case_<B + 1>(
                std::forward<F>(f), std::forward<V>(v));
        }
        case B + 2: {
            return dispatcher<B + 2 < size, R>::template case_<B + 2>(
                std::forward<F>(f), std::forward<V>(v));
        }
        case B + 3: {
            return dispatcher<B + 3 < size, R>::template case_<B + 3>(
                std::forward<F>(f), std::forward<V>(v));
        }
        default: {
            return dispatcher<B + 4 < size, R>::template switch_<B + 4>(
                std::forward<F>(f), std::forward<V>(v));
        }
        }
    }
};

template <typename F, typename V> 
constexpr decltype(auto) visit(F&& f, V&& v) {
    using R = decltype(std::invoke(std::forward<F>(f),
                                   std::get<0>(std::forward<V>(v))));
    return dispatcher<true, R>::template switch_<0>(std::forward<F>(f),
                                                    std::forward<V>(v));
}

} // namespace cpplox