#include <iostream>
#include <string>
#include "Tokenizer.h"
using namespace std;

class Parser {
private:
    static int result;
    static Tokenizer tokenizer;
    static Token current_token;
    static Token prev_token_expression;
    static Token prev_token_term;

public:
    static int parse_term() {
        int term = 1;
        prev_token_term.type = "MULT";
        prev_token_term.value = 0;
        while (current_token.type != "EOF" && current_token.type != "PLUS" && current_token.type != "MINUS") {
            if (prev_token_term.type == "INT" && current_token.type == "INT") { throw invalid_argument("Numbers cannot be followed by another number"); }
            if (prev_token_term.type != "INT" && current_token.type != "INT") { throw invalid_argument("Signs cannot be followed by another sign"); }
            if (prev_token_term.type != "INT" && current_token.type == "EOF") { throw invalid_argument("Expected number"); }
            if (prev_token_term.type == "MULT") { term *= current_token.value; }
            else if (prev_token_term.type == "DIV") { term /= current_token.value; }
            prev_token_term = current_token;
            current_token = tokenizer.selectNext();
        }
        return term;
    }

    static int parse_expression() {
        result = 0;
        prev_token_expression.type = "PLUS";
        prev_token_expression.value = 0;
        while (current_token.type != "EOF") {
            current_token = tokenizer.selectNext();
            if (current_token.type != "INT") { throw invalid_argument("Expected number"); }
            int term = parse_term();
            if (prev_token_expression.type == "PLUS") { result += term; }
            else if (prev_token_expression.type == "MINUS") { result -= term; }
            prev_token_expression = current_token;
        }
        return result;
    }

    static void run(string code) {
        tokenizer = Tokenizer(code);
        cout << parse_expression() << endl;
    }
};
