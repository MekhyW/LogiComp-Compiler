#include <iostream>
#include <string>
#include "Tokenizer.h"
using namespace std;

class Parser {
private:
    static Tokenizer tokenizer;

public:
    static int parse_expression() {
        int result = 0;
        string op = "PLUS";
        Token current_token = tokenizer.selectNext();
        if (current_token.type == "EOF") { throw invalid_argument("Input cannot be empty"); }
        while (current_token.type != "EOF") {
            if (current_token.type == "INT") {
                if (op == "PLUS") { result += current_token.value; } 
                else if (op == "MINUS") { result -= current_token.value; }
            } else { throw invalid_argument("Input must start with a number and signs cannot be followed by another sign"); }
            Token next_token = tokenizer.selectNext();
            op = next_token.type;
            current_token = tokenizer.selectNext();
            if (op == "INT") { throw invalid_argument("Numbers cannot be followed by another number"); }
            if (op != "EOF" && current_token.type == "EOF") { throw invalid_argument("Expected number"); }
        }
        return result;
    }

    static void run(string code) {
        tokenizer = Tokenizer(code);
        cout << parse_expression() << endl;
    }
};
