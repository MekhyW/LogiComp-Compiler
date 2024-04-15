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
        while (current_token.type != "EOF" && current_token.type != "END") { block_node->add_statement(parse_statement()); }
        return block_node;
    }

    shared_ptr<Node> parse_statement() {
        if (current_token.type == "EOF") { return make_shared<NoOpNode>(); }
        else if (current_token.type == "NEWLINE" || current_token.type == "END" || current_token.type == "ELSE") {
            current_token = tokenizer.selectNext(); 
            return make_shared<NoOpNode>(); 
        }
        else if (current_token.type == "PRINT") {
            current_token = tokenizer.selectNext();
            if (current_token.type != "LPAREN") { throw invalid_argument("Expected '(' after print keyword"); }
            current_token = tokenizer.selectNext();
            shared_ptr<Node> expression_node = parse_boolexpression();
            if (current_token.type != "RPAREN") { throw invalid_argument("Expected ')' after print expression"); }
            current_token = tokenizer.selectNext();
            if (current_token.type != "NEWLINE") { throw invalid_argument("Expected newline after print statement"); }
            current_token = tokenizer.selectNext();
            return make_shared<PrintNode>(expression_node);
        }
        else if (current_token.type == "WHILE") {
            current_token = tokenizer.selectNext();
            shared_ptr<Node> condition = parse_boolexpression();
            if (current_token.type != "DO") { throw invalid_argument("Expected 'do' after while expression"); }
            current_token = tokenizer.selectNext();
            if (current_token.type != "NEWLINE") { throw invalid_argument("Expected newline after while expression"); }
            shared_ptr<Node> block_node = parse_block();
            if (current_token.type != "END") { throw invalid_argument("Expected 'end' after while block"); }
            current_token = tokenizer.selectNext();
            if (current_token.type != "NEWLINE") { throw invalid_argument("Expected newline after while block"); }
            current_token = tokenizer.selectNext();
            return make_shared<WhileNode>(condition, block_node);
        }
        else if (current_token.type == "IF") {
            current_token = tokenizer.selectNext();
            shared_ptr<Node> condition = parse_boolexpression();
            if (current_token.type != "THEN") { throw invalid_argument("Expected 'then' after if expression"); }
            current_token = tokenizer.selectNext();
            if (current_token.type != "NEWLINE") { throw invalid_argument("Expected newline after if expression"); }
            shared_ptr<Node> block_node = parse_block();
            shared_ptr<Node> else_block_node = make_shared<NoOpNode>();
            if (current_token.type == "ELSE") {
                current_token = tokenizer.selectNext();
                else_block_node = parse_block();
            }
            if (current_token.type != "END") { throw invalid_argument("Expected 'end' after if block"); }
            current_token = tokenizer.selectNext();
            if (current_token.type != "NEWLINE") { throw invalid_argument("Expected newline after if block"); }
            current_token = tokenizer.selectNext();
            return make_shared<IfNode>(condition, block_node, else_block_node);
        }
        else { 
            string identifier = current_token.type;
            if (identifier == "NUMBER") { throw invalid_argument("Cannot assign to number"); }
            current_token = tokenizer.selectNext();
            if (current_token.type != "ASSIGN") { throw invalid_argument("Expected '=' after identifier"); }
            current_token = tokenizer.selectNext();
            shared_ptr<Node> bolexp_node = parse_boolexpression();
            if (current_token.type != "NEWLINE") { throw invalid_argument("Expected newline after assignment"); }
            current_token = tokenizer.selectNext();
            return make_shared<AssignmentNode>(identifier, bolexp_node);
        }
    }

    shared_ptr<Node> parse_boolexpression() {
        shared_ptr<Node> bool_expression_node = parse_boolterm();
        while (current_token.type == "OR") {
            Token op_token = current_token;
            current_token = tokenizer.selectNext();
            shared_ptr<Node> next_bool_term_node = parse_boolterm();
            bool_expression_node = make_shared<BinOpNode>("or", bool_expression_node, next_bool_term_node);
        }
        return bool_expression_node;
    }

    shared_ptr<Node> parse_boolterm() {
        shared_ptr<Node> bool_term_node = parse_relexpression();
        while (current_token.type == "AND") {
            Token op_token = current_token;
            current_token = tokenizer.selectNext();
            shared_ptr<Node> next_rel_expression_node = parse_relexpression();
            bool_term_node = make_shared<BinOpNode>("and", bool_term_node, next_rel_expression_node);
        }
        return bool_term_node;
    }

    shared_ptr<Node> parse_relexpression() {
        shared_ptr<Node> relexpression_node = parse_expression();
        while (current_token.type == "EQ" || current_token.type == "NEQ" 
        || current_token.type == "GT" || current_token.type == "LT" || current_token.type == "GE" || current_token.type == "LE") {
            Token op_token = current_token;
            current_token = tokenizer.selectNext();
            shared_ptr<Node> next_expression_node = parse_expression();
            if (op_token.type == "GT") { relexpression_node = make_shared<BinOpNode>(">", relexpression_node, next_expression_node); }
            else if (op_token.type == "LT") { relexpression_node = make_shared<BinOpNode>("<", relexpression_node, next_expression_node); }
            else if (op_token.type == "GE") { relexpression_node = make_shared<BinOpNode>(">=", relexpression_node, next_expression_node); }
            else if (op_token.type == "LE") { relexpression_node = make_shared<BinOpNode>("<=", relexpression_node, next_expression_node); }
            else if (op_token.type == "EQ") { relexpression_node = make_shared<BinOpNode>("==", relexpression_node, next_expression_node); }
            else { relexpression_node = make_shared<BinOpNode>("!=", relexpression_node, next_expression_node); }
        }
        return relexpression_node;
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
        while (current_token.type == "MULT" || current_token.type == "DIV" || current_token.type == "MOD") {
            Token op_token = current_token;
            current_token = tokenizer.selectNext();
            shared_ptr<Node> next_factor_node = parse_factor();
            if (op_token.type == "MULT") { term_node = make_shared<BinOpNode>("*", term_node, next_factor_node); }
            else if (op_token.type == "DIV") { term_node = make_shared<BinOpNode>("/", term_node, next_factor_node); }
            else { term_node = make_shared<BinOpNode>("%", term_node, next_factor_node); }
        }
        return term_node;
    }

    shared_ptr<Node> parse_factor() {
        if (current_token.type == "NUMBER") {
            int value = current_token.value;
            current_token = tokenizer.selectNext();
            return make_shared<IntValNode>(value);
        }
        else if (current_token.type == "LPAREN") {
            current_token = tokenizer.selectNext();
            shared_ptr<Node> bool_expression_node = parse_boolexpression();
            if (current_token.type != "RPAREN") { throw invalid_argument("Expected ')' after expression"); }
            current_token = tokenizer.selectNext();
            return bool_expression_node;
        }
        else if (current_token.type == "MINUS") {
            current_token = tokenizer.selectNext();
            return make_shared<BinOpNode>("-", make_shared<IntValNode>(0), parse_factor());
        }
        else if (current_token.type == "PLUS") {
            current_token = tokenizer.selectNext();
            return parse_factor();
        }
        else if (current_token.type == "NOT") {
            current_token = tokenizer.selectNext();
            return make_shared<UnOpNode>("not", parse_factor());
        }
        else if (current_token.type == "READ") {
            current_token = tokenizer.selectNext();
            if (current_token.type != "LPAREN") { throw invalid_argument("Expected '(' after read"); }
            current_token = tokenizer.selectNext();
            if (current_token.type != "RPAREN") { throw invalid_argument("Expected ')' after read"); }
            current_token = tokenizer.selectNext();
            return make_shared<ReadNode>();
        }
        else {
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
