#include "Scanner.h"
#include "Error.h"
#include "TokenTypes.h"
#include <iostream>

namespace cpplox {
////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////
std::vector<Token> scanTokens(const std::string& source) {

    std::vector<Token> tokens;

    int start = 0;
    int current = 0;
    int line = 1;

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
    auto isAtEnd = [&]() -> bool { return current >= source.length(); };

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
            Error::error(line, "Unterminated string.");
            return;
        }

        // The closing ". we know its a " because the previous while loop
        // terminated with that condition
        advance();

        // Trim the surrounding quotes.
        // c++ way of doing substr pos+offset
        std::string value = source.substr(
            start + 1, current - start - 2); // dont know why we need to do -2
        // need to use different named lambda as we cant override the name
        addTokenLiteral(ETokenType::STRING,
                        source.substr(start + 1, current - start - 2));
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
        auto search = keywordMap.find(text);
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
        case '-': {
            if (peek() == '-') {
                advance();
                addToken(ETokenType::MINUS_MINUS);
            } else {
                addToken(ETokenType::MINUS);
            }
            break;
        }
        case '+': {
            if (peek() == '+') {
                advance();
                addToken(ETokenType::PLUS_PLUS);
            } else {
                addToken(ETokenType::PLUS);
            }
            break;
        }
        case ';': addToken(ETokenType::SEMICOLON); break;
        case '*': addToken(ETokenType::STAR); break;
        case '%': addToken(ETokenType::MOD); break;

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
                std::string str;
                str = (char)c;
                Error::error(line, std::string("Unexpected character: \'") +
                                       str + std::string("\'"));
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