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
    shared_ptr<Node> parse_factor() {
        if (current_token.type == "INT") {
            int value = current_token.value;
            current_token = tokenizer.selectNext();
            return make_shared<IntVal>(value);
        } else if (current_token.type == "PLUS" || current_token.type == "MINUS") {
            Token op_token = current_token;
            current_token = tokenizer.selectNext();
            if (op_token.type == "PLUS") {
                return make_shared<UnOp>('+', parse_factor());
            } else {
                return make_shared<UnOp>('-', parse_factor());
            }
        } else if (current_token.type == "LPAREN") {
            current_token = tokenizer.selectNext();
            shared_ptr<Node> expression = parse_expression();
            if (current_token.type != "RPAREN") {
                throw invalid_argument("Expected ')'");
            }
            current_token = tokenizer.selectNext();
            return expression;
        } else {
            throw invalid_argument("Expected number, '+', '-' or '('");
        }
    }

    shared_ptr<Node> parse_term() {
        shared_ptr<Node> term = parse_factor();
        while (current_token.type == "MULT" || current_token.type == "DIV") {
            Token op_token = current_token;
            current_token = tokenizer.selectNext();
            shared_ptr<Node> next_factor = parse_factor();
            if (op_token.type == "MULT") {
                term = make_shared<BinOp>('*', term, next_factor);
            } else {
                term = make_shared<BinOp>('/', term, next_factor);
            }
        }
        return term;
    }

    shared_ptr<Node> parse_expression() {
        shared_ptr<Node> expression = parse_term();
        while (current_token.type == "PLUS" || current_token.type == "MINUS") {
            Token op_token = current_token;
            current_token = tokenizer.selectNext();
            shared_ptr<Node> next_term = parse_term();
            if (op_token.type == "PLUS") {
                expression = make_shared<BinOp>('+', expression, next_term);
            } else {
                expression = make_shared<BinOp>('-', expression, next_term);
            }
        }
        return expression;
    }

    shared_ptr<Node> run(const string& code) {
        tokenizer = Tokenizer(code);
        current_token = tokenizer.selectNext();
        shared_ptr<Node> root = parse_expression();
        if (current_token.type != "EOF") {
            throw invalid_argument("Expected EOF");
        }
        return root;
    }
};
