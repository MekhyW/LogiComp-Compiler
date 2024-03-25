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
    shared_ptr<Node> parse_block() {
        shared_ptr<Node> block_node = make_shared<BlockNode>();
        while (current_token.type != "EOF") { block_node->add_statement(parse_statement()); }
        return block_node;
    }

    shared_ptr<Node> parse_statement() {
        if (current_token.type == "PRINT") {
            current_token = tokenizer.selectNext();
            if (current_token.type == "LPAREN") {
                current_token = tokenizer.selectNext();
                shared_ptr<Node> expression_node = parse_expression();
                if (current_token.type != "RPAREN") { throw invalid_argument("Expected ')' after print expression"); }
                current_token = tokenizer.selectNext();
                return make_shared<PrintNode>(expression_node);
            } 
            else { throw invalid_argument("Expected '(' after print keyword"); }
        }
        else if (current_token.type == "EOF") { return make_shared<NoOpNode>(); }
        else { 
            string identifier = current_token.type;
            current_token = tokenizer.selectNext();
            if (current_token.type == "ASSIGN") {
                current_token = tokenizer.selectNext();
                return make_shared<AssignmentNode>(identifier, parse_expression());
            } else { throw invalid_argument("Expected '=' after identifier"); }
        }
    }

    shared_ptr<Node> parse_expression() {
        shared_ptr<Node> expression_node = parse_term();
        while (current_token.type == "PLUS" || current_token.type == "MINUS") {
            Token op_token = current_token;
            current_token = tokenizer.selectNext();
            shared_ptr<Node> next_term_node = parse_term();
            if (op_token.type == "PLUS") { expression_node = make_shared<BinOpNode>("+", expression_node, next_term_node); }
            else { expression_node = make_shared<BinOpNode>("-", expression_node, next_term_node); }
        }
        return expression_node;
    }

    shared_ptr<Node> parse_term() {
        shared_ptr<Node> term_node = parse_factor();
        while (current_token.type == "MULT" || current_token.type == "DIV") {
            Token op_token = current_token;
            current_token = tokenizer.selectNext();
            shared_ptr<Node> next_factor_node = parse_factor();
            if (op_token.type == "MULT") { term_node = make_shared<BinOpNode>("*", term_node, next_factor_node); }
            else { term_node = make_shared<BinOpNode>("/", term_node, next_factor_node); }
        }
        return term_node;
    }

    shared_ptr<Node> parse_factor() {
        if (current_token.type == "NUMBER") {
            int value = current_token.value;
            current_token = tokenizer.selectNext();
            return make_shared<IntValNode>(value);
        } else if (current_token.type == "LPAREN") {
            current_token = tokenizer.selectNext();
            shared_ptr<Node> expression_node = parse_expression();
            if (current_token.type != "RPAREN") { throw invalid_argument("Expected ')' after expression"); }
            current_token = tokenizer.selectNext();
            return expression_node;
        } else {
            string identifier = current_token.type;
            current_token = tokenizer.selectNext();
            return make_shared<VarNode>(identifier);
        }
    }

    shared_ptr<Node> run(const string& code) {
        tokenizer = Tokenizer(code);
        current_token = tokenizer.selectNext();
        shared_ptr<Node> root = parse_block();
        if (current_token.type != "EOF") { throw invalid_argument("Expected EOF"); }
        return root;
    }
};
