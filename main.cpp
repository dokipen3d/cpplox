#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include "Scanner.h"
#include "TokenTypes.h"
#include "Expression.h"
#include "Error.h"


void run(const std::string& code) {
    
    std::cout << "scanning tokens\n";

    auto tokens = cpplox::scanTokens(code);
    for (auto& t : tokens){
            std::cout << t.lexeme << " ";
    }
    std::cout << "\n";

    std::cout << "parsing expressions\n";

    cpplox::Expr expression = cpplox::parse(tokens);
    if (hadError){
        std::cout << "error\n";
        return;
    }
    cpplox::print(expression);
}

// dummy functions to make main run at this stage
void runFile(const std::string& filePath) {
    std::cout << "running file " << filePath << "\n";
    std::ifstream inputFileStream(filePath);
    std::string code(std::istreambuf_iterator<char>{inputFileStream}, {});
    run(code);
};
void runPrompt() {
    std::cout << "Running prompt\n";
    std::string currentLine;
    for (;;) {
        std::getline(std::cin, currentLine);
        run(currentLine);
        // set error back in case there was an error as we don't want to kill session
        hadError = false;
    }
};

int main(int argumentCount, char* argumentValues[]) {

    //ddcpplox::visit2();
    
    if (argumentCount > 2) {
        std::cout << "Usage: cpplox [script] \n";
        return 0;
    } else if (argumentCount == 2) {
        runFile(argumentValues[1]);
        if (hadError) {
            return 1;
        }
    } else {
        runPrompt();
    }
}