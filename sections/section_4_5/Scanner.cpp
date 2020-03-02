#include "Scanner.h"

namespace cpplox {

std::vector<Token> scanTokens(const std::string& source) {

    std::vector<Token> tokens;

    int start = 0;
    int current = 0;
    int line = 1;

    auto isAtEnd = [&]() -> bool { return current >= source.length(); };

    auto advance = [&]() -> const char& {
        current++;
        return source[current - 1];
    };

    auto addTokenLiteral = [&](ETokenType type, Object literal) {
        std::string text = source.substr(start, /*offset=*/current - start);
        tokens.push_back(Token(type, text, literal, line));
    };

    auto addToken = [&](ETokenType type) { addTokenLiteral(type, nullptr); };

    auto scanToken = [&]() -> void {
        const char& c = advance();
        switch (c) {
        case '(': addToken(ETokenType::LEFT_PARENTHESIS); break;
        case ')': addToken(ETokenType::RIGHT_PARENTHESIS); break;
        case '{': addToken(ETokenType::LEFT_BRACE); break;
        case '}': addToken(ETokenType::RIGHT_BRACE); break;
        case ',': addToken(ETokenType::COMMA); break;
        case '.': addToken(ETokenType::DOT); break;
        case '-': addToken(ETokenType::MINUS); break;
        case '+': addToken(ETokenType::PLUS); break;
        case ';': addToken(ETokenType::SEMICOLON); break;
        case '*': addToken(ETokenType::STAR); break;
        }
    };

    while (!isAtEnd()) {
        start = current; 
        scanToken();
    }

    tokens.push_back(Token(ETokenType::END_OF_FILE, "", nullptr, line));
    return tokens;
}

} // end namespace cpplox