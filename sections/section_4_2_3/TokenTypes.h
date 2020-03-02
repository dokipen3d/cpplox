#pragma once

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

inline const std::map<ETokenType, std::string> tokenMap{
    // Single-character tokens
    {ETokenType::LEFT_PARENTHESIS, "Left Parenthesis"},
    {ETokenType::RIGHT_PARENTHESIS, "Right Parenthesis"},
    {ETokenType::LEFT_BRACE, "Left_brace"},
    {ETokenType::RIGHT_BRACE, "Right_brace"},
    {ETokenType::COMMA, "Comma"},
    {ETokenType::DOT, "Dot"},
    {ETokenType::MINUS, "Minus"},
    {ETokenType::PLUS, "Plus"},
    {ETokenType::SEMICOLON, "Semicolon"},
    {ETokenType::SLASH, "Slash"},
    {ETokenType::STAR, "Star"},
    {ETokenType::BANG, "Bang"},
    {ETokenType::BANG_EQUAL, "Bang_equal"},
    {ETokenType::EQUAL, "Equal"},
    {ETokenType::EQUAL_EQUAL, "Equal_equal"},
    {ETokenType::GREATER, "Greater"},
    {ETokenType::GREATER_EQUAL, "Greater_equal"},
    {ETokenType::LESS, "Less"},
    {ETokenType::LESS_EQUAL, "Less_equal"},
    {ETokenType::IDENTIFIER, "Identifier"},
    {ETokenType::STRING, "String"},
    {ETokenType::NUMBER, "Number"},
    {ETokenType::AND, "And"},
    {ETokenType::CLASS, "Class"},
    {ETokenType::ELSE, "Else"},
    {ETokenType::FALSE, "False"},
    {ETokenType::FUN, "Fun"},
    {ETokenType::FOR, "For"},
    {ETokenType::IF, "If"},
    {ETokenType::NIL, "Nil"},
    {ETokenType::OR, "Or"},
    {ETokenType::PRINT, "Print"},
    {ETokenType::RETURN, "Return"},
    {ETokenType::SUPER, "Super"},
    {ETokenType::THIS, "This"},
    {ETokenType::TRUE, "True"},
    {ETokenType::VAR, "Var"},
    {ETokenType::WHILE, "While"},
    {ETokenType::END_OF_FILE, "End_of_file"}};

class Object{};

class Token {
  public:
    
    Token(ETokenType tokenType, std::string lexeme, Object literal, int line)
        : eTokenType(tokenType), literal(std::move(literal)),
          lexeme(std::move(lexeme)), line(line) {
    }

    std::string toString() {
        std::stringstream stream;
        auto search = tokenMap.find(eTokenType);
        if (search != tokenMap.end()) {
            stream << search->second;
        }
        stream << " " << lexeme << " Object";
        return stream.str();
    }

    ETokenType eTokenType;
    Object literal;
    std::string lexeme;
    int line;
};

} // end namespace cpplox