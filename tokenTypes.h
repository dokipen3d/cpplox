#pragma once

#include <sstream>

namespace cpplox {
enum class ETokenType {
    // Single-character tokens
    LEFT_PARENTHESIS,
    RIGHT_PARENTHESIS,
    LEFT_BRACE,
    RIGHT_BRACE,
    COMMA,
    DOT,
    MINUS,
    PLUS,
    SEMICOLON,
    SLASH,
    STAR,

    // One or two character tokens.
    BANG,
    BANG_EQUAL,
    EQUAL,
    EQUAL_EQUAL,
    GREATER,
    GREATER_EQUAL,
    LESS,
    LESS_EQUAL,

    // Literals.
    IDENTIFIER,
    STRING,
    NUMBER,

    // Keywords.
    AND,
    CLASS,
    ELSE,
    FALSE,
    FUN,
    FOR,
    IF,
    NIL,
    OR,
    PRINT,
    RETURN,
    SUPER,
    THIS,
    TRUE,
    VAR,
    WHILE,
    END_OF_FILE
};

class Token {

    Token(ETokenType tokenType, std::string lexeme, std::string literal,
          int line)
        : eTokenType(tokenType), lexeme(lexeme), literal(literal), line(line) {
    }

    std::string toString() {
        std::stringstream stream;
        stream << lexeme << " " << literal << "\n";
        return stream.str();
    }
  private:
    ETokenType eTokenType;
    std::string lexeme;
    std::string literal;
    int line;
    


};

} // namespace cpplox
