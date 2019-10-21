#include "Scanner.h"
#include "tokenTypes.h"
#include <iostream>

namespace cpplox {
////////////////////////////////////////////////////////////////////////
void report(int line, const std::string& where, const std::string& message) {
    std::cout << "[line " << line << "] Error" << where << ": " << message
              << "\n";
    hadError = true;
}

void error(int line, const std::string& message) {
    report(line, "", message);
}
////////////////////////////////////////////////////////////////////////
std::vector<Token> scanTokens(const std::string& source) {
    int start = 0;
    int current = 0;
    int line = 1;

    std::vector<Token> tokens;

    auto addTokenLiteral = [&](ETokenType type, Object literal) {
        std::string text = source.substr(start, /*offset=*/current - start);
        tokens.push_back(Token(type, text, literal, line));
    };

    auto addToken = [&](ETokenType type) { addTokenLiteral(type, nullptr); };

    // this moves the 'current' character one forward and then returns the
    // previous one we just moved past
    auto advance = [&]() -> const char& {
        current++;
        return source[current - 1];
    };

    // helper for detecting if we have gone past the end of the file
    auto isAtEnd = [&]() { return current >= source.length(); };

    // match checks the second character. like a conditional advance(). It only
    // consumes the current character if it’s what we’re looking for.
    // Technically, match() is doing lookahead too. advance() and peek() are the
    // fundamental operators and match() combines them.
    auto match = [&](const char& expected) -> bool {
        if (isAtEnd())
            return false;
        // since advance moves one forward, we want to check 'current' for the
        // second character
        if (source[current] != expected)
            return false;

        current++;
        return true;
    };
    // this just returns the current (or rather 'next') charater with safety
    // bounds checking
    auto peek = [&]() -> const char {
        if (isAtEnd())
            return '\0'; // return null termination
        return source[current];
    };

    auto peekNext = [&]() -> const char {
        if (current + 1 >= source.length())
            return '\0';
        return source[current + 1];
    };
    auto String = [&]() {
        // this while keeps advancing until it hits another '"' and also keeps
        // track of when the line increases
        while (peek() != '"' && !isAtEnd()) {
            if (peek() == '\n')
                line++;
            advance();
        }

        // Unterminated string.
        if (isAtEnd()) {
            error(line, "Unterminated string.");
            return;
        }

        // The closing ". we know its a " because the previous while loop
        // terminated with that condition
        advance();

        // Trim the surrounding quotes.
        // c++ way of doing substr pos+offset
        std::string value = source.substr(start + 1, current - start - 1);
        // need to use different named lambda as we cant override the name
        addTokenLiteral(ETokenType::STRING, value);
    };

    auto isDigit = [&](const char& c) -> bool { return c >= '0' && c <= '9'; };

    auto number = [&]() {
        while (isDigit(peek())) {
            advance();
        }
        // Look for a fractional part.
        if (peek() == '.' && isDigit(peekNext())) {
            // Consume the "."
            advance();

            while (isDigit(peek()))
                advance();
        }

        addTokenLiteral(ETokenType::NUMBER,
                        std::stod(source.substr(start, current - start)));
    };

    auto isAlpha = [&](const char& c) -> bool {
        return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_';
    };

    auto isAlphaNumeric = [&](const char& c) -> bool {
        return isAlpha(c) || isDigit(c);
    };

    auto identifier = [&]() {
        while (isAlphaNumeric(peek()))
            advance();

        // default
        auto type = ETokenType::IDENTIFIER;

        // See if the identifier is a reserved word.
        const std::string text = source.substr(start, current - start);
        auto search  = keywordMap.find(text);
        if (search != keywordMap.end()) {
            type = search->second;
        }

        addToken(type);
    };

    // our single function that scans individual tokens.
    // This is called in the while loop below
    auto scanToken = [&]() {
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
        case '/': {
            if (match('/')) {
                // when we find a second /, we don’t end the token yet.
                // Instead, we keep consuming characters until we reach the end
                // of the line.  A comment goes until the end of the
                // line.
                while (peek() != '\n' && !isAtEnd()) {
                    advance();
                }
            } else { // if it wansn't a comment (second character not another
                     // '/'), then it was a slash token
                addToken(ETokenType::SLASH);
            }
            break;
        }
        case ' ':
        case '\r':
        case '\t': // Ignore whitespace.
            break;

        case '\n': line++; break;
        case '"': String(); break;

        default:
            if (isDigit(c)) {
                number();
            } else if (isAlpha(c)) {
                identifier();
            } else {
                error(line, "Unexpected character.");
                break;
            }
        }
    };

    while (!isAtEnd()) {

        start = current; // We are at the beginning of the next lexeme. reset
                         // start to be where we last finished
        scanToken(); // scan one token (which adds to the token vector) and then
                     // the loop continues until it meets the end condition
    }

    tokens.push_back(Token(ETokenType::END_OF_FILE, "", nullptr, line));

    return tokens;
}

} // namespace cpplox