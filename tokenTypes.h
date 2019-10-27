#pragma once

#include <map>
#include <sstream>
#include <string>
#include <variant>

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

const std::map<std::string, ETokenType> keywordMap{

    {"and", ETokenType::AND},       {"class", ETokenType::CLASS},
    {"else", ETokenType::ELSE},     {"false", ETokenType::FALSE},
    {"for", ETokenType::FOR},       {"fun", ETokenType::FUN},
    {"if", ETokenType::IF},         {"nil", ETokenType::NIL},
    {"or", ETokenType::OR},         {"print", ETokenType::PRINT},
    {"return", ETokenType::RETURN}, {"super", ETokenType::SUPER},
    {"this", ETokenType::THIS},     {"true", ETokenType::TRUE},
    {"var", ETokenType::VAR},       {"while", ETokenType::WHILE}};

// equivalent to the use of the Java.Object in the crafting interpreters
// tutorial. void* means a not a literal. we check for it by checking the active
// index of the variant ie index() > 0
using variantObject = std::variant<std::nullptr_t, double, std::string, bool>;
struct Object : variantObject {
    using variantObject::variantObject;

    const bool operator==(const std::nullptr_t& other) const {
        return std::holds_alternative<std::nullptr_t>(*this);
    }
    
    template <typename T>
    bool is() const{ // function needs to be const  to make it callable from a const ref
        return std::holds_alternative<T>(*this);
    }
};
class Token {
  public:
    Token() = default; // need this to make expression be able to hold Tokens as
                       // members and use aggregate initialization
    Token(ETokenType tokenType, std::string lexeme, Object literal, int line)
        : eTokenType(tokenType), literal(literal), lexeme(lexeme), line(line) {
    }

    std::string toString() {
        std::stringstream stream;
        auto search = tokenMap.find(eTokenType);
        if (search != tokenMap.end()) {
            stream << search->second;
        }
        stream << " " << lexeme << " ";
        std::visit(
            [&](auto&& arg) {
                if (literal.index() > 0) { // not a void* so can print
                    stream << arg;
                }
            },
            literal);

        return stream.str();
    }

    friend struct Error;

    ETokenType eTokenType;
    Object literal;
    std::string lexeme;
    int line;
};

} // namespace cpplox