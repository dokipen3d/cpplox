#include "Scanner.h"

namespace cpplox {

std::vector<Token> scanTokens(const std::string& source) {

    std::vector<Token> tokens;

    int start = 0;
    int current = 0;
    int line = 1;

    // helper for detecting if we have gone past the end of the file
    auto isAtEnd = [&]() -> bool { return current >= source.length(); };

    auto scanToken = [&]() -> void {};

    while (!isAtEnd()) {

        start = current; // We are at the beginning of the next lexeme. reset
                         // start to be where we last finished
        scanToken(); // scan one token (which adds to the token vector) and then
                     // the loop continues until it meets the end condition
    }

    tokens.push_back(Token(ETokenType::END_OF_FILE, "", nullptr, line));
    return tokens;
}

} // end namespace cpplox