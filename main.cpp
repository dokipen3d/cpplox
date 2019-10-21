#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include "Scanner.h"
#include "tokenTypes.h"
#include "AstTypes.h"


void run(const std::string& code) {
    
    auto tokens = cpplox::scanTokens(code);

    // print the tokens
    for (auto& token : tokens) {
        std::cout << token.toString() << "\n";
    }
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