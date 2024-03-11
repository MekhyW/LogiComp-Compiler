#include <iostream>
#include <string>
#include "Parser.h"
using namespace std;

Tokenizer Parser::tokenizer = Tokenizer("");
Token Parser::current_token;
Token Parser::prev_token_expression;
Token Parser::prev_token_term;

int main(int argc, char *argv[]) {
    if (argc != 2) {
        cout << "Usage: " << argv[0] << " \"<input>\"" << endl;
        return 1;
    }
    string code;
    code += argv[1];
    Parser::run(code);
    return 0;
}