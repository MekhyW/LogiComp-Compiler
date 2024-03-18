#include <iostream>
#include <string>
#include "Tokenizer.h"
#include "Node.h"

using namespace std;

class Parser {
private:
    static Tokenizer tokenizer;
    static Token current_token;

public:
    static Node* parse_factor() {
        current_token = tokenizer.selectNext();
        if (current_token.type == "INT") {
            return new IntVal(current_token.value);
        } else if (current_token.type == "PLUS") {
            return parse_factor();
        } else if (current_token.type == "MINUS") {
            Node* child = parse_factor();
            NodeValue value = current_token.value;
            current_token = tokenizer.selectNext();
            return new UnOp(value, *child);
        } else if (current_token.type == "LPAREN") {
            Node* expression = parse_expression();
            if (current_token.type != "RPAREN") {
                throw invalid_argument("Expected ')'");
            }
            current_token = tokenizer.selectNext();
            return expression;
        } else {
            throw invalid_argument("Expected number, '+', '-' or '('");
        }
    }

    static Node* parse_term() {
        Node* term = parse_factor();
        while (current_token.type == "MULT" || current_token.type == "DIV") {
            Token op_token = current_token;
            current_token = tokenizer.selectNext();
            Node* next_factor = parse_factor();
            term = new BinOp(op_token.value, *term, *next_factor);
        }
        return term;
    }

    static Node* parse_expression() {
        Node* expression = parse_term();
        while (current_token.type == "PLUS" || current_token.type == "MINUS") {
            Token op_token = current_token;
            current_token = tokenizer.selectNext();
            Node* next_term = parse_term();
            expression = new BinOp(op_token.value, *expression, *next_term);
        }
        return expression;
    }

    static Node* run(string code) {
        tokenizer = Tokenizer(code);
        current_token = tokenizer.selectNext();
        Node* root = parse_expression();
        if (current_token.type != "EOF") {
            throw invalid_argument("Unexpected token at end of input");
        }
        return root;
    }
};
