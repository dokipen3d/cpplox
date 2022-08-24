#include "../src/TimeIt.hpp"
#include <iostream>

int fib(int n) {
    if (n < 2)
        return n;
    return fib(n - 1) + fib(n - 2);
}

int main() {
    TimeIt timer("fib");

    std::cout << fib(35);
}
