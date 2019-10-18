#pragma once

#include <any>
#include <map>
#include <sstream>
#include <string>

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

const std::map<ETokenType, std::string> tokenMap{
    // Single-character tokens
    {ETokenType::LEFT_PARENTHESIS, "left parenthesis"},
    {ETokenType::RIGHT_PARENTHESIS, "right parenthesis"},
};

class Token {
  public:
    Token(ETokenType tokenType, std::string lexeme, std::any literal, int line)
        : eTokenType(tokenType), lexeme(lexeme), literal(literal), line(line) {
    }

    std::string toString() {
        std::stringstream stream;
        auto search = tokenMap.find(eTokenType);
        if(search != tokenMap.end()){
            stream << search->second;
        }
        stream << " " << lexeme << " ";
        if (literal.type() == typeid(std::string)) {
            stream << std::any_cast<std::string>(literal) << " ";
        } else if (literal.type() == typeid(double)) {
            stream << std::any_cast<double>(literal) << " ";
        }
        return stream.str();
    }

  private:
    ETokenType eTokenType;
    std::string lexeme;
    std::any literal;
    int line;
};

} // namespace cpplox
