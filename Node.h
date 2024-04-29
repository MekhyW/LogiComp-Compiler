#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>
#include <variant>
#include "SymbolTable.h"
using namespace std;

class Node;
using NodePtr = shared_ptr<Node>;
using EvalResult = variant<int, string, double, bool>;

class Node {
public:
    vector<NodePtr> statements;
    virtual EvalResult Evaluate(SymbolTable& symbolTable) const = 0;
    void add_statement(NodePtr statement) { statements.push_back(statement); }
};

class BinOpNode : public Node {
public:
    BinOpNode(string op, NodePtr left, NodePtr right) : op(op), left(move(left)), right(move(right)) {}
    EvalResult Evaluate(SymbolTable& symbol_table) const override {
        EvalResult left_value = left->Evaluate(symbol_table);
        EvalResult right_value = right->Evaluate(symbol_table);
        if (op != ".." && op != "==" && op != "!=" && (holds_alternative<string>(left_value) || holds_alternative<string>(right_value))) {
            throw invalid_argument("Unsupported operation on string type");
        }
        if (op == "..") { return get<string>(left_value) + get<string>(right_value); }
        else if (op == "+") { return get<int>(left_value) + get<int>(right_value); }
        else if (op == "-") { return get<int>(left_value) - get<int>(right_value); }
        else if (op == "*") { return get<int>(left_value) * get<int>(right_value); }
        else if (op == "/") {
            if (get<int>(right_value) == 0) { throw invalid_argument("Division by zero"); }
            return get<int>(left_value) / get<int>(right_value);
        }
        else if (op == "%") {
            if (get<int>(right_value) == 0) { throw invalid_argument("Division by zero"); }
            return get<int>(left_value) % get<int>(right_value);
        }
        else if (op == "==") { return get<int>(left_value) == get<int>(right_value); }
        else if (op == "!=") { return get<int>(left_value) != get<int>(right_value); }
        else if (op == "<") { return get<int>(left_value) < get<int>(right_value); }
        else if (op == "<=") { return get<int>(left_value) <= get<int>(right_value); }
        else if (op == ">") { return get<int>(left_value) > get<int>(right_value); }
        else if (op == ">=") { return get<int>(left_value) >= get<int>(right_value); }
        else if (op == "and") { return get<bool>(left_value) && get<bool>(right_value); }
        else if (op == "or") { return get<bool>(left_value) || get<bool>(right_value); }
        else { throw invalid_argument("Invalid binary operation"); }
    }
private:
    string op;
    NodePtr left, right;
};

class UnOpNode : public Node {
public:
    UnOpNode(string op, NodePtr child) : op(op), child(move(child)) {}
    EvalResult Evaluate(SymbolTable& symbol_table) const override {
        EvalResult child_value = child->Evaluate(symbol_table);
        if (op == "+") { return get<int>(child_value); }
        else if (op == "-") { return -get<int>(child_value); }
        else if (op == "not") { return !get<bool>(child_value); }
        else { throw invalid_argument("Invalid unary operation"); }
    }
private:
    string op;
    NodePtr child;
};

class NoOpNode : public Node {
public:
    EvalResult Evaluate(SymbolTable& symbol_table) const override { return 0; }
};

class IntValNode : public Node {
public:
    IntValNode(int val) : value(val) {}
    EvalResult Evaluate(SymbolTable& symbol_table) const override { return EvalResult(value); }
private:
    int value;
};

class StringValNode : public Node {
public:
    StringValNode(string val) : value(val) {}
    EvalResult Evaluate(SymbolTable& symbol_table) const override { return EvalResult(value); }
private:
    string value;
};

class VarNode : public Node {
public:
    VarNode(string identifier) : identifier(identifier) {}
    EvalResult Evaluate(SymbolTable& symbol_table) const override { return symbol_table.getVariable(identifier); }
private:
    string identifier;
};

class ReadNode : public Node {
public:
    ReadNode() = default;
    EvalResult Evaluate(SymbolTable& symbol_table) const override {
        int value;
        cin >> value;
        return value;
    }
};

class PrintNode : public Node {
public:
    PrintNode(NodePtr expression) : expression(move(expression)) {}
    EvalResult Evaluate(SymbolTable& symbol_table) const override {
        EvalResult result = expression->Evaluate(symbol_table);
        if (holds_alternative<int>(result)) { cout << get<int>(result); }
        else if (holds_alternative<string>(result)) { cout << get<string>(result); }
        else if (holds_alternative<double>(result)) { cout << get<double>(result); }
        else if (holds_alternative<bool>(result)) { cout << get<bool>(result); }
        return result;
    }
private:
    NodePtr expression;
};

class VarDeclareNode : public Node {
public:
    VarDeclareNode(string identifier, NodePtr expression = make_shared<IntValNode>(0))
        : identifier(identifier), expression(move(expression)) {}
    EvalResult Evaluate(SymbolTable& symbol_table) const override {
        EvalResult result = expression->Evaluate(symbol_table);
        symbol_table.setVariable(identifier, result);
        return result;
    }
private:
    string identifier;
    NodePtr expression;
};

class AssignmentNode : public Node {
public:
    AssignmentNode(string identifier, NodePtr expression) : identifier(identifier), expression(move(expression)) {}
    EvalResult Evaluate(SymbolTable& symbol_table) const override {
        EvalResult result = expression->Evaluate(symbol_table);
        symbol_table.setVariable(identifier, get<int>(result));
        return result;
    }
private:
    string identifier;
    NodePtr expression;
};

class WhileNode : public Node {
public:
    WhileNode(NodePtr condition, NodePtr block) : condition(move(condition)), block(move(block)) {}
    EvalResult Evaluate(SymbolTable& symbol_table) const override {
        EvalResult result = 0;
        while (get<bool>(condition->Evaluate(symbol_table))) { result = block->Evaluate(symbol_table); }
        return result;
    }
private:
    NodePtr condition, block;
};

class IfNode : public Node {
public:
    IfNode(NodePtr condition, NodePtr block, NodePtr else_block) : condition(move(condition)), block(move(block)), else_block(move(else_block)) {}
    EvalResult Evaluate(SymbolTable& symbol_table) const override {
        EvalResult result = condition->Evaluate(symbol_table);
        if (get<bool>(result)) { return block->Evaluate(symbol_table); }
        else { return else_block->Evaluate(symbol_table); }
    }
private:
    NodePtr condition, block, else_block;
};

class BlockNode : public Node {
public:
    EvalResult Evaluate(SymbolTable& symbol_table) const override {
        EvalResult result = 0;
        for (const auto& statement : statements) { result = statement->Evaluate(symbol_table); }
        return result;
    }
};