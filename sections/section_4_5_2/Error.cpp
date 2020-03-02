#include "Error.h"
#include <iostream>

namespace cpplox {

void Error::error(int line, const std::string& message) {
    report(line, "", message);
}

void Error::report(int line, const std::string& where, const std::string& message) {
    std::cout << "[line " << line << "] Error" << where << ": " << message
              << "\n";
    hadError = true;
}

} // end namespace cpplox