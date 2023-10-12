#pragma once

#include <chrono>
#include <iostream>
#include <string>

struct TimeIt {
    TimeIt(const char* scopeName = "scope", bool scope = true)
        : m_StartTime(std::chrono::high_resolution_clock::now()), scope(scope),
          scopeName(scopeName) {
    }
    ~TimeIt() {
        if (scope) { // if default scope is set then this will get called on
                     // destruction
            time();
        } // if scope was set to false, then user has to call time manually
    }

    void time() const {
        // std::cout << scopeName << " took "
        //           << std::chrono::duration_cast<std::chrono::milliseconds>(
        //                  std::chrono::high_resolution_clock::now() -
        //                  m_StartTime)
        //                  .count()
        //           << " milliseconds\n";
        auto end = std::chrono::duration<double>(
                         std::chrono::high_resolution_clock::now() -
                         m_StartTime)
                         .count();
        std::cout << scopeName << " took "
                  <<   end
                  << " seconds\n";

    }
    bool scope;
    const char* scopeName;
    std::chrono::high_resolution_clock::time_point m_StartTime;
};