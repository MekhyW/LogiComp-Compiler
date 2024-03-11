#include <iostream>
#include <string>
#include "Tokenizer.h"
using namespace std;

class Parser {
private:
    static Tokenizer tokenizer;
    static Token current_token;
    static Token prev_token_expression;
    static Token prev_token_term;

public:
    static int parse_factor() {
        current_token = tokenizer.selectNext();
        if (current_token.type == "INT") { return current_token.value; }
        else if (current_token.type == "PLUS") { return parse_factor(); }
        else if (current_token.type == "MINUS") { return -parse_factor(); }
        else if (current_token.type == "LPAREN") {
            int expression = parse_expression();
            if (current_token.type != "RPAREN") { throw invalid_argument("Expected ')'"); }
            return expression;
        }
        else { throw invalid_argument("Expected number, '+', '-' or '('"); }
    }

    static int parse_term() {
        int term = 1;
        prev_token_term = {"MULT", 0};
        while (1) {
            int factor = parse_factor();
            if (prev_token_term.type == "MULT") { term *= current_token.value; }
            else if (prev_token_term.type == "DIV") { term /= current_token.value; }
            current_token = tokenizer.selectNext();
            if (current_token.type == "MULT" || current_token.type == "DIV") { prev_token_term = current_token; }
            else { return term; }
        }
    }

    static int parse_expression() {
        int result = 0;
        while (1) {
            int term = parse_term();
            if (prev_token_expression.type == "PLUS") { result += term; }
            else if (prev_token_expression.type == "MINUS") { result -= term; }
            if(current_token.type == "PLUS" || current_token.type == "MINUS") { prev_token_expression = current_token; }
            else { return result; }
        }
    }

    static void run(string code) {
        tokenizer = Tokenizer(code);
        prev_token_expression = {"PLUS", 0};
        int final_result = parse_expression();
        cout << final_result << endl;
    }
};
