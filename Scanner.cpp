#include "Scanner.h"
#include "tokenTypes.h"
#include <iostream>

namespace cpplox {
std::vector<Token> scanTokens(const std::string& code) {
    int start = 0;
    int current = 0;
    int line = 1;

    std::vector<Token> tokens;

    auto advance = [&]() -> char {                               
        current++;
        return code[current - 1];                   
    };

    auto addTokenLiteral = [&](ETokenType type, std::any literal) {
        std::string text = code.substr(start, current-start);      
        tokens.push_back(Token(type, text, literal, line));    
    };

    auto addToken = [&](ETokenType type) {                
        addTokenLiteral(type, nullptr);                                
    };                                                      


    auto scanToken = [&](){                     
        char c = advance();                          
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

    auto isAtEnd = [&]() { return current >= code.length(); };

    while (!isAtEnd()) {
        // We are at the beginning of the next lexeme. reset start to be where
        // we last finished
        start = current;
        scanToken();
    }

    tokens.push_back(Token(ETokenType::END_OF_FILE, "", nullptr, line));

    return tokens;
}

void report(int line, const std::string& where, const std::string& message) {
    std::cout << "[line " << line << "] Error" << where << ": " << message
              << "\n";
    hadError = true;
}

void error(int line, const std::string& message) {
    report(line, "", message);
}
} // namespace cpplox
