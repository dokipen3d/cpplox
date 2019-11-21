#pragma once

#include <map>
#include <sstream>
#include <string>
#include <variant>

#include "Utilities.hpp"

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

inline const std::map<std::string, ETokenType> keywordMap{

    {"and", ETokenType::AND},       {"class", ETokenType::CLASS},
    {"else", ETokenType::ELSE},     {"false", ETokenType::FALSE},
    {"for", ETokenType::FOR},       {"fun", ETokenType::FUN},
    {"if", ETokenType::IF},         {"nil", ETokenType::NIL},
    {"or", ETokenType::OR},         {"print", ETokenType::PRINT},
    {"return", ETokenType::RETURN}, {"super", ETokenType::SUPER},
    {"this", ETokenType::THIS},     {"true", ETokenType::TRUE},
    {"var", ETokenType::VAR},       {"while", ETokenType::WHILE}};

// forward declares
struct NativeFunction;

// equivalent to the use of the Java.Object in the crafting interpreters
// tutorial. void* means a not a literal. we check for it by checking the active
// index of the variant ie index() > 0
using Object = std::variant<void*, double, std::string, bool,
                            recursive_wrapper<NativeFunction>>;

// helper functions to make variant comparable to nullptr
//////////////////////////////////////////////////////////////////////////
inline bool operator==(
    const Object& other,
    std::nullptr_t) { // needs to be inline because its a free function that
                      // it included in multiple translation units. needs to
                      // be marked inline so linker knows its the same one
    return std::holds_alternative<void*>(other);
}

inline bool operator==(
    const Object& a,
    const Object& b) { // needs to be inline because its a free function that
                      // it included in multiple translation units. needs to
                      // be marked inline so linker knows its the same one
    if(std::holds_alternative<recursive_wrapper<NativeFunction>>(a) || 
        std::holds_alternative<recursive_wrapper<NativeFunction>>(b)) {
            return false;
        }
    else {
        return a == b;
    };
}

inline bool operator!=(const Object& other, std::nullptr_t ptr) {
    return !(other == ptr);
}

template <typename T>
bool is(const Object& object) { // function needs to be const  to make it
                                // callable from a const ref
    return std::holds_alternative<T>(object);
}

template <typename T>
inline const T&
getRecursiveObject(const Object& object) { // function needs to be const  to make it
                                 // callable from a const ref
    return std::get<recursive_wrapper<T>>(object);
}




struct Interpreter;

struct NativeFunction {
    NativeFunction() = default;
    Object call(const Interpreter& interpreter,
                const std::vector<Object> arguments) const;

    friend std::ostream& operator<<(std::ostream& os, const recursive_wrapper<NativeFunction>& dt);
};

struct FunctionObject {
    FunctionObject() = default;
    Object call(const Interpreter& interpreter,
                const std::vector<Object> arguments);
    friend std::ostream& operator<<(std::ostream& os, const recursive_wrapper<FunctionObject>& dt);
};



class Token {
  public:
    Token() = default; // need this to make expression be able to hold Tokens as
                       // members
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
        stream << " " << lexeme << " ";
        std::visit(
            [&](auto&& arg) {
                if (!(literal == nullptr)) { // not a void* so can print
                    stream << arg;
                }
            },
            static_cast<Object>(literal));

        return stream.str();
    }

    friend struct Error;

    ETokenType eTokenType;
    Object literal;
    std::string lexeme;
    int line;
};

static_assert(std::is_move_constructible_v<Token>,
              "token is not move contructible");
static_assert(std::is_move_assignable_v<Token>,
              "token is not move contructible");


} // namespace cpplox