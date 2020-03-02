# Scanning


## Fleshing out the apllication 4.1

So here we will be fleshing out main.cpp and creating a basic framework to start our interpreter from. This follows the https://craftinginterpreters.com/scanning.html page closely.

### Main


```c++
int main(int argumentCount, char* argumentValues[]) {

    Lox lox;

    if (argumentCount > 2) {
        std::cout << "Usage: cpplox [script] \n";
        return 0;
    } else if (argumentCount == 2) {
        lox.runFile(argumentValues[1]);
    } else {
        lox.runPrompt();
    }
}
```
First we create a `Lox` obejct. We will get to creating that soon. it is responsible to reading our code and processing it.

Here we are taking in arguments from the command line and we get passed how many arguments there were and also the arguments themselves as an array of strings (which in c/c++ are pointer to characters ie. char *).

Since the first argument is always the program itself, the second argumen is potentially the name of a scipt file that we want to run.
If there are more than two arguments we inform the user how to use the program correctly.

If there are exactly 2 arguments, it means that the user has given a filename and then we call the `lox.runFile()` function. If there is no argument, then we run the live prompt mode with `lox/runPrompt()`.

### Creating the lox class

There is a slight difference in the way that java and c++ define their classes an main function. In the Java code, main is part of the lox class, whereas in c++ I decided to have a lox class that contained the run(), runFile() and runPrompt() function as members, and then the main function creates a lox object and uses that. One of the reasons for this was to get around some c++ peculiarities (something to do with ODR and etc, I can't quite remember).

```c++
struct Lox {

    // all the following code from run(), runPrompt() and runFile() goes in here

}
```

### Reading a file

Required includes....

```c++
#include <fstream>
#include <string>
```
...

```c++
// in struct Lox { 
void runFile(const std::string& filePath) {
    std::ifstream inputFileStream(filePath);
    std::string code(std::istreambuf_iterator<char>{inputFileStream}, {});
    run(code);
};
```

Here we take in the filePath (c++ takes care of the conversion from char* to std::string) and construct a std::ifstream object which is an input stream. 
Then we construct a std::string directly using the string constructor that takes an iterator. In this case an input stream iterator that has the stream passed into that. This might nit be the most efficient way to read a file, but it is the simplest, and for this application, the speed was not an issue for me.

We then call the run() function which we will get to soon.

### Reading from the command line

Required includes....

```c++
#include <iostream>
```
...

```c++
// in struct Lox { 
void runPrompt() {
    std::cout << "Running prompt\n";
    std::string currentLine;
    for (;;) {
        std::getline(std::cin, currentLine);
        run(currentLine);
    }
};
```

This is run in the case that we want to run interactively and want to process one line at a time. We have a std::string, and then in an infinite loop, call std::getline (passing in the character input stream object std::cin and the string we want to read the code into). Then we pass the code to the run() function.

### Running the code.

Required includes....


```c++
#include <vector>
```
...

```c++
// in struct Lox { 
void run(const std::string& code) {
    auto tokens = cpplox::scanTokens(code);

    //print the tokens
    for (auto& token : tokens){
        std::cout << token << "\n"; 
    }
}
```
Here we take the code, and pass it into the scanToken function (which we will write soon). Then at this point, we just print out the tokens.

This devaties slightly from the java based version, where Scanner is a separate class. I originally had it working like that as well in c++, but I realised that I didn't need the scanner to keep any state around. It's job was to just process the tokens and give them back. So I simple made a scanTokens() free function (which we will move into a sepeate file soon, for now I keep it local in main.cpp to keep the project setup simple).

Also instead of a Java list of tokens, we get back a `std::vector<std::string>` of tokens.

### Stubbing out a temporary dummy scanTokens() function

Here in main.cpp, we will add a cpplox::scanToken() function so that our code will compile at this stage and actually print out the tokens to make sure that everything is working at this stage. This is also the first example of the cpplox namspace that we will be putting most of our classes and types in.  Again, this function will be factored out into a separate file soon but for now we can compile this.

```c++
// above struct Lox {

namespace cpplox {

std::vector<std::string> scanTokens(const std::string& code) {
    return {"token 1", "token2"};
}

} // end namespace cpplox
```
Now we can build our first main.cpp file and run it in the command line!

### Handling Errors 4.1.1

Now we can add some basic error handling into the Lox class. We will eventually factor out error handling into a different file, but again, for this part while we are getting our feet wet, we will do in directly in this file.

```c++
                                                                        /*
struct Lox {                                                            */

    bool hadError = false;
                                                                        /*
    void run(const std::string& code) {                                 */
```

We introduce a new variable within the Lox class to track if we had an Error or not.

Next we add into Lox the report() function.

```c++
                                                                        /*
bool hadError = false;                                                  */

void report(int line, const std::string& where,
            const std::string& message) {
    std::cout << "[line " << line << "] Error" << where << ": " << message
                << "\n";
    hadError = true;
}

void error(int line, const std::string& message) {
    report(line, "", message);
}
                                                                /*
void run(const std::string& code) {                             */
```

In runPrompt()....

```c++
                                                                /*
        run(currentLine);                                       */
        // set error back in case there was an error as we don't 
        // want to kill session
        hadError = false;  
    }                                                                                
                                          /*
};                                                          */
```
in the prompt, we might have had an error, but we dont want to close the interpreter, so we reset the error state.

In main()...

```c++
                                                            /*
} else if (argumentCount == 2) {
    runFile(argumentValues[1]);                             */
    if (hadError) {
        return 1;
    }                                                       /*
}                                                           */
```

Now we have the ability to report errors. We aren't actually calling that yet anywhere but we will soon! Return 1 returns to the operating system with an error code which is non zerp (ie something went wrong).

## Tokens and Lexemes 4.2
### 4.2.1


We will now define all of the token types. We will do that in a separate .h file.

```c++
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
```


### Token 4.2.3

This is a class that represents a token. 

We will add a contructor that takes some arguments to initialize its members (We will also add a default contructor later for other reasons).

> #### Java Objects to c++
> Java has a class of type 'Object' that can store any object. We want something like that in c++. One option could be c++'s std::any type, but that has a runtime cost. We will soon decide that c++17's std::variant is a good choice because we know at compile time the finite set of type that it will contain, so the compiler can optimize for that. Also, we will be using the visitor pattern which we get for free with std::variant.  
>
>For now we will just use a dummy class and not print the Object in the to_string method. 

> #### Printing enums
>We also want to be able to print the token type stored in the token. Java can do this automatically but c++ isn't quite there yet. So we will define a map to be able to convert from one to the other.

In TokenTypes.h...
Required includes....
```cpp
#include <map>
#include <sstream>
#include <string>
```

```cpp

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
        stream << "\n";
        return stream.str();
    }

    ETokenType eTokenType;
    Object literal;
    std::string lexeme;
    int line;
};

} // end namespace cpplox
```

### Using c++ variant to emulate Java's Object class 4.2.4

This is a custom section not in the original books. I will show you how to use c++17's std::variant to emulate the Object class.

What we will do is first define a type alias (with c++'s using keyword) to state that an ObjectVariant is a std::variant which can be anyone of a...
- void*
- double (lox's internal representation for all numbers)
- std::string
- bool

We then inherit from that type to define our 'Object'. The reason for this is that we will want to add on extra function to this class to be able to compare it to null and to also control the extraction of the type it holds later on.

Required includes....
```cpp
                                                                        /*
#include <map>
#include <sstream>
#include <string>                                                       */
#include <variant>
```

Replace `class Object{}` above Token in TokenTypes/h with this....
```c++ 
// equivalent to the use of the Java.Object in the crafting interpreters
// tutorial. void* means a not a literal. we check for it by checking the active
// index of the variant ie index() > 0
using ObjectVariant = std::variant<void*, double, std::string, bool>;

struct Object : ObjectVariant {

    using ObjectVariant::ObjectVariant;
    using ObjectVariant::operator=;

    inline bool operator==(std::nullptr_t)
        const { // needs to be inline because its a free function that
                // it included in multiple translation units. needs to
                // be marked inline so linker knows its the same one
        return std::holds_alternative<void*>(*this);
    }

    inline bool operator!=(std::nullptr_t ptr) const {
        return !(*this == ptr);
    }   


};
```
Now we can print the Object by calling the std::visit method on is inside Token's to_string() function...

```cpp
                                                                            /*
stream << " " << lexeme << " ";                                             */
        std::visit(
            [&](auto&& arg) {
                if (!(literal == nullptr)) { // not a void* so can print
                    stream << arg;
                }
            },
            static_cast<ObjectVariant>(literal));                           /*
        stream << "\n";                                                     
        return stream.str();                                                
    }                                                                       */

```

Now in main this can be demonstrated by creating a token and printing it.
```cpp
                                                                            /*
int main(int argumentCount, char* argumentValues[]) {

    Lox lox;                                                                */

    cpplox::Token token(cpplox::ETokenType::NUMBER, "lexeme", 3.0, 4);
    std::cout << token.toString();                                          /*

    if (argumentCount > 2) {                                                */

```
But remove those lines as we will now move on to the next section which is actually scanning lox code and turning it into tokens. 

## Scanning 4.4

Lets write our scanner in a new file Scanner.h

We are going to do a bit of experimenting here with c++. We don't need Scanner to be a class so we will just make it a function. The scanner function itself doesn't have a public interface. However the original book does define a few functions in it's scanner class. To keep scanner a function, we will just make a few lambda functions within the scanTokens() function itself. Lets see how it turns out!

>Note: you should now remove the scanTokens() function in main.cpp as there will be clashing versions of the function in the same namespace. Also Tokens aren't just strings now so we can't stream them to print them and we should remove this part of the code that prints of them in run() in main.cpp...
>```cpp
>    // print the tokens
>    for (auto& token : tokens) {
>        std::cout << token << "\n";
>    }
>```

```cpp
#pragma once

#include "TokenTypes.h"
#include <string>
#include <vector>

namespace cpplox {
std::vector<Token> scanTokens(const std::string& source);
}


```



Over in the cpp file we define the function. We will implement the scanToken() and other functions soon.

```cpp
#include "Scanner.h"

namespace cpplox {

std::vector<Token> scanTokens(const std::string& source) {

    std::vector<Token> tokens;

    while (!isAtEnd()) {

        start = current; // We are at the beginning of the next lexeme. reset
                         // start to be where we last finished
        scanToken(); // scan one token (which adds to the token vector) and then
                     // the loop continues until it meets the end condition
    }

    tokens.push_back(Token(ETokenType::END_OF_FILE, "", nullptr, line));
    return tokens;
}

} // end namespace cpplox
```

Now we need to introduce the ints which track which part of the source code we are.

```c++
                                                                            /*
    std::vector<Token> tokens;                                              */

    int start = 0;
    int current = 0;
    int line = 1; 
                                                                            /*
    while (!isAtEnd()) {                                                    */

```

Then we introduce the isAtEnd() function (notice the fact that it is a lambda and it is using the trailing return type syntax).

```cpp
                                                                            /*
    int line = 1;                                                           */

    // helper for detecting if we have gone past the end of the file
    auto isAtEnd = [&]() -> bool { return current >= source.length(); };
                                                                            /*
    while (!isAtEnd()) {                                                    */
```

Before we move onto the next section, I like to get things to compile and so far, we don't have a scanToken() function. Lets define an empty one.

```cpp
                                                                            /*
    auto isAtEnd = [&]() -> bool { return current >= source.length(); };    */

    auto scanToken = [&]() -> void {};
                                                                            /*
    while (!isAtEnd()) {                                                    */

```

## Recognizing Lexemes 4.5 

Now lets stub out the scanToken() function.

```cpp
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
        }
    };
```

This calls the addToken functions. There are actually two overloads for the function in the Java version. Because we can't have two lambda's named the same in c++, we will rename one of them....

Above scanToken()...
```cpp
    auto advance = [&]() -> const char& {
        current++;
        return source[current - 1];
    };

    auto addTokenLiteral = [&](ETokenType type, Object literal) {
        std::string text = source.substr(start, /*offset=*/current - start);
        tokens.push_back(Token(type, text, literal, line));
    };

    auto addToken = [&](ETokenType type) { addTokenLiteral(type, nullptr); };
```

#### Lexical Errors 4.5.1

Now we want to be able to report errors. Fo our scanner function to be able to report errors, we need to move the Lox.error() function to somewhere we can access it. We will move it to a separate header and make it a static function accessible from anywhere. We make hadError an inline variable (a  feature added in c++17 exactly for this purpose), so that all files that include this header see hadError as the same bool when the application is linked.

In Error.h...
```cpp
#pragma once 

#include "TokenTypes.h"
#include <string>

inline bool hadError = false;

namespace cpplox {
struct Error {
    static void report(int line, const std::string& where, const std::string& message);
    static void error(int line, const std::string& message);
};
} //end namespace lox

```

In Error.cpp
```cpp
#include "Error.h"
#include <iostream>

namespace cpplox {

void Error::report(int line, const std::string& where, const std::string& message) {
    std::cout << "[line " << line << "] Error" << where << ": " << message
              << "\n";
    hadError = true;
}

void Error::error(int line, const std::string& message) {
    report(line, "", message);
}


} // end namespace cpplox
```

Then remove the error() and report() functions from Lox struct in main.cpp and include this Error.h in main.cpp and Scanner.cpp

in main(), change 
```cpp
if (lox.hadError) {
    return 1;
}
```

to 

```cpp
if (hadError) {
    return 1;
}
```

### Operators 4.5.2

```cpp
                                                                                /*
        case ';': addToken(ETokenType::SEMICOLON); break;                       */
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
            break;                                                              /*
        default: {                                                              */
```


```cpp
    auto match = [&](const char& expected) -> bool {
        if (isAtEnd())
            return false;

        if (source[current] != expected)
            return false;

        current++;
        return true;
    };
```