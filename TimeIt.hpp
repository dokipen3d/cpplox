#pragma once

#include <chrono>
#include <iostream>

struct TimeIt {
    TimeIt() : m_StartTime(std::chrono::high_resolution_clock::now()) {
    }

    void time() const {
        std::cout << "scope took " << std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::high_resolution_clock::now() - m_StartTime).count() << " milliseconds\n";
    }
    std::chrono::high_resolution_clock::time_point m_StartTime;
};