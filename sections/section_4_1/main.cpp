#include <fstream>
#include <string>
#include <iostream>
#include <vector>



namespace cpplox {

std::vector<std::string> scanTokens(const std::string& code) {
    return {"token 1", "token2"};
}

} // end namespace cpplox



struct Lox {

    void run(const std::string& code) {
        auto tokens = cpplox::scanTokens(code);

        //print the tokens
        for (auto& token : tokens){
            std::cout << token << "\n"; 
        }
    }

    void runFile(const std::string& filePath) {
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
        }
    };
}; // end of struct lox 

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