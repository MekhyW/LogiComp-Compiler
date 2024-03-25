#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>
#include "SymbolTable.h"

using namespace std;

class Node;
using NodePtr = shared_ptr<Node>;

class Node {
public:
    virtual int Evaluate(SymbolTable& symbolTable) const = 0;
    void add_statement(NodePtr statement) {}
};

class BinOpNode : public Node {
public:
    BinOpNode(string op, NodePtr left, NodePtr right) : op(op), left(move(left)), right(move(right)) {}
    int Evaluate(SymbolTable& symbol_table) const override {
        int left_value = left->Evaluate(symbol_table);
        int right_value = right->Evaluate(symbol_table);
        if (op == "+") { return left_value + right_value; }
        else if (op == "-") { return left_value - right_value; }
        else if (op == "*") { return left_value * right_value; }
        else if (op == "/") {
            if (right_value == 0) { throw invalid_argument("Division by zero"); }
            return left_value / right_value;
        } 
        else { throw invalid_argument("Invalid binary operation"); }
    }
private:
    string op;
    NodePtr left, right;
};

class UnOpNode : public Node {
public:
    UnOpNode(string op, NodePtr child) : op(op), child(move(child)) {}
    int Evaluate(SymbolTable& symbol_table) const override {
        int child_value = child->Evaluate(symbol_table);
        if (op == "+") { return child_value; }
        else if (op == "-") { return -child_value; }
        else { throw invalid_argument("Invalid unary operation"); }
    }
private:
    string op;
    NodePtr child;
};

class NoOpNode : public Node {
public:
    int Evaluate(SymbolTable& symbol_table) const override { return 0; }
};


class IntValNode : public Node {
public:
    IntValNode(int val) : value(val) {}
    int Evaluate(SymbolTable& symbol_table) const override { return value; }
private:
    int value;
};

class VarNode : public Node {
public:
    VarNode(string identifier) : identifier(identifier) {}
    int Evaluate(SymbolTable& symbol_table) const override {
        if (symbol_table.getVariable(identifier)!= 0) { return symbol_table.getVariable(identifier); }
        else { throw invalid_argument("Undefined variable: " + identifier); }
    }
private:
    string identifier;
};

class PrintNode : public Node {
public:
    PrintNode(NodePtr expression) : expression(move(expression)) {}
    int Evaluate(SymbolTable& symbol_table) const override {
        int result = expression->Evaluate(symbol_table);
        std::cout << result << endl;
        return result;
    }
private:
    NodePtr expression;
};

class AssignmentNode : public Node {
public:
    AssignmentNode(string identifier, NodePtr expression) : identifier(identifier), expression(move(expression)) {}
    int Evaluate(SymbolTable& symbol_table) const override {
        int result = expression->Evaluate(symbol_table);
        symbol_table.setVariable(identifier, result);
        return result;
    }
private:
    string identifier;
    NodePtr expression;
};

class BlockNode : public Node {
public:
    void add_statement(NodePtr statement) { statements.push_back(statement); }
    int Evaluate(SymbolTable& symbol_table) const override {
        int result = 0;
        for (const auto& statement : statements) { result = statement->Evaluate(symbol_table); }
        return result;
    }
private:
    vector<NodePtr> statements;
};