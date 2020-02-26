# Scanning

## This is where we create the basic skeleton of our program and follows the https://craftinginterpreters.com/scanning.html page closely

### 4.1

So here we will be fleshing out main.cpp and creating a basic framework to start our interpreter from.

```cpp
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
```
