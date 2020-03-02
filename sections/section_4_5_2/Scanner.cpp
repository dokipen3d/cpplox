#include "Scanner.h"
#include "Error.h"

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

    auto match = [&](const char& expected) -> bool {
        if (isAtEnd())
            return false;
            
        if (source[current] != expected)
            return false;

        current++;
        return true;
    };

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
                case '!':
            addToken(match('=') ? ETokenType::BANG_EQUAL : ETokenType::BANG);
            break;
        case '=':
            addToken(match('=') ? ETokenType::EQUAL_EQUAL : ETokenType::EQUAL);
            break;
        case '<':
            addToken(match('=') ? ETokenType::LESS_EQUAL : ETokenType::LESS);
            break;
        case '>':
            addToken(match('=') ? ETokenType::GREATER_EQUAL
                                : ETokenType::GREATER);
            break;
        default: {
            Error::error(line, "Unexpected character.");
            break;
        }

        } //end switch (c)
    };

    while (!isAtEnd()) {
        start = current; 
        scanToken();
    }

    tokens.push_back(Token(ETokenType::END_OF_FILE, "", nullptr, line));
    return tokens;
}

} // end namespace cpplox