#pragma once

#include <functional>
#include <map>
#include <sstream>
#include <string>
#include <variant>
#include <memory>

//#include "Object.h"
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
struct FunctionObject;
struct Environment;

// equivalent to the use of the Java.Object in the crafting interpreters
// tutorial. void* means a not a literal. we check for it by checking the active
// index of the variant ie index() > 0
using ObjectVariant = std::variant<void*, double, std::string, bool,
                                   recursive_wrapper<NativeFunction>,
                                   recursive_wrapper<FunctionObject>>;

struct Object : ObjectVariant {

    using ObjectVariant::ObjectVariant;
    using ObjectVariant::operator=;
    // helper functions to make variant comparable to nullptr
    //////////////////////////////////////////////////////////////////////////
    inline bool operator==(std::nullptr_t)
        const { // needs to be inline because its a free function that
                // it included in multiple translation units. needs to
                // be marked inline so linker knows its the same one
        return std::holds_alternative<void*>(*this);
    }

    inline bool operator==(const Object& other)
        const { // needs to be inline because its a free function that
                // it included in multiple translation units. needs to
                // be marked inline so linker knows its the same one
        if (std::holds_alternative<recursive_wrapper<NativeFunction>>(*this) ||
            std::holds_alternative<recursive_wrapper<NativeFunction>>(other)) {
            return false;
        } else {
            return *this == other;
        };
    }

    inline bool operator!=(std::nullptr_t ptr) const {
        return !(*this == ptr);
    }

    template <typename T>
    inline bool is() const { // function needs to be const  to make it
        // callable from a const ref
        using actualTypeToGet =
            std::conditional_t<has_type_v<recursive_wrapper<T>, ObjectVariant>,
                               recursive_wrapper<T>, T>;
        return std::holds_alternative<actualTypeToGet>(*this);
    }

    template <typename T>
    inline const T& getRecursiveObject() { // function needs to be const  to
                                           // make it callable from a const ref
        return std::get<recursive_wrapper<T>>(*this);
    }
    template <typename T>
    inline const T& get() { // function needs to be const  to
                            // make it callable from a const ref
        using actualTypeToGet =
            std::conditional_t<has_type_v<recursive_wrapper<T>, ObjectVariant>,
                               recursive_wrapper<T>, T>;
        return std::get<actualTypeToGet>(*this);
    }
};

struct Interpreter;

struct NativeFunction {
    NativeFunction(
        std::function<Object(const Interpreter&, const std::vector<Object>)>
            func,
        std::function<int()> arity);

    Object operator()(const Interpreter& interpreter,
                      const std::vector<Object>& objects) {
        return m_func(interpreter, objects);
    }

    friend std::ostream&
    operator<<(std::ostream& os, const recursive_wrapper<NativeFunction>& dt);

    // we store references to lambdas
    std::function<Object(const Interpreter&, const std::vector<Object>&)>
        m_func;
    std::function<int()> arity;
};

struct FunctionStatement;

struct FunctionObject {
    FunctionObject(const FunctionStatement& functionStatement,
                        const std::shared_ptr<Environment>& closure);

    Object operator()(Interpreter& interpreter,
                      const std::vector<Object>& arguments);
    int arity();

    friend std::ostream&
    operator<<(std::ostream& os, const recursive_wrapper<FunctionObject>& dt);

    const FunctionStatement& m_declaration;
    std::shared_ptr<Environment> closure;
};

class Token {
  public:
    Token() = default; // need th to make expression be able to hold Tokens
                       // as members
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
            static_cast<ObjectVariant>(literal));

        return stream.str();
    }

    friend struct Error;

    ETokenType eTokenType;
    Object literal;
    std::string lexeme;
    int line;
};

static_assert(std::is_move_constructible_v<Token>,
              "token  not move contructible");
static_assert(std::is_move_assignable_v<Token>, "token  not move contructible");

} // namespace cpplox

template <>
struct std::variant_size<cpplox::Object>
    : std::variant_size<cpplox::ObjectVariant> {};

template <std::size_t I>
struct std::variant_alternative<I, cpplox::Object>
    : std::variant_alternative<I, cpplox::ObjectVariant> {};