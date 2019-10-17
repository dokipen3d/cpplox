#include <fstream>
#include <iostream>
#include <string>
#include <vector>

class Scanner {
public:
    Scanner(std::string sourceIn) : source(std::move(sourceIn)) {}
    std::vector<std::string> scanTokens() { return {"token1", "token2"}; }
private:
    std::string source;
};

void run(const std::string& code) {
    Scanner scanner(code);
    auto tokens = scanner.scanTokens();

    //print the tokens
    for (auto& token : tokens){
        std::cout << token << "\n"; 
    }
}

// dummy functions to make main run at this stage
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

int main(int argumentCount, char* argumentValues[]) {

    if (argumentCount > 2) {
        std::cout << "Usage: cpplox [script] \n";
        return 0;
    } else if (argumentCount == 2) {
        runFile(argumentValues[1]);
    } else {
        runPrompt();
    }
}
