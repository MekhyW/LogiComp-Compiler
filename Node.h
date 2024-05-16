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
using EvalResult = std::variant<int, string, double, bool>;

class Node {
public:
    vector<NodePtr> statements;
    static int i;
    int id;
    static int newId() { return ++i; }
    Node() : id(newId()) {}
    virtual EvalResult Evaluate(SymbolTable& symbol_table, FuncTable& func_table, Assembly& assembly) const = 0;
    void add_statement(NodePtr statement) { statements.push_back(statement); }
};

int Node::i = 0;

class BinOpNode : public Node {
public:
    BinOpNode(string op, NodePtr left, NodePtr right) : op(op), left(move(left)), right(move(right)) {}
    EvalResult Evaluate(SymbolTable& symbol_table, FuncTable& func_table, Assembly& assembly) const override {
        EvalResult right_value = right->Evaluate(symbol_table, func_table, assembly);
        EvalResult left_value = left->Evaluate(symbol_table, func_table, assembly);
        if (op != ".." && (holds_alternative<string>(left_value) != holds_alternative<string>(right_value))) {
            throw invalid_argument("Unsupported operation on string type");
        }
        if (op == "..") { 
            string left_str;
            string right_str;
            if (holds_alternative<int>(left_value)) { left_str = to_string(get<int>(left_value)); }
            else if (holds_alternative<double>(left_value)) { left_str = to_string(get<double>(left_value)); }
            else if (holds_alternative<string>(left_value)) { left_str = get<string>(left_value); }
            else if (holds_alternative<bool>(left_value)) { left_str = get<bool>(left_value) ? "1" : "0"; }
            if (holds_alternative<int>(right_value)) { right_str = to_string(get<int>(right_value)); }
            else if (holds_alternative<double>(right_value)) { right_str = to_string(get<double>(right_value)); }
            else if (holds_alternative<string>(right_value)) { right_str = get<string>(right_value); }
            else if (holds_alternative<bool>(right_value)) { right_str = get<bool>(right_value) ? "1" : "0"; }
            return EvalResult(left_str + right_str);
        }
        else if (op == "+" || op == "-" || op == "*" || op == "/" || op == "%") {
            int left_int;
            int right_int;
            if (holds_alternative<int>(left_value)) { left_int = get<int>(left_value); }
            else if (holds_alternative<double>(left_value)) { left_int = get<double>(left_value); }
            else if (holds_alternative<bool>(left_value)) { left_int = get<bool>(left_value); }
            if (holds_alternative<int>(right_value)) { right_int = get<int>(right_value); }
            else if (holds_alternative<double>(right_value)) { right_int = get<double>(right_value); }
            else if (holds_alternative<bool>(right_value)) { right_int = get<bool>(right_value); }
            if (op == "+") { return EvalResult(left_int + right_int); }
            else if (op == "-") { return EvalResult(left_int - right_int); }
            else if (op == "*") { return EvalResult(left_int * right_int); }
            else if (op == "/") {
                if (right_int == 0) { throw invalid_argument("Division by zero"); }
                return EvalResult(left_int / right_int);
            }
            else if (op == "%") {
                if (right_int == 0) { throw invalid_argument("Division by zero"); }
                return EvalResult(left_int % right_int);
            }
        }
        else if (op == "==" || op == "!=" || op == "<" || op == "<=" || op == ">" || op == ">=") { 
            int left_int;
            int right_int;
            if (holds_alternative<int>(left_value)) { left_int = get<int>(left_value); }
            else if (holds_alternative<double>(left_value)) { left_int = get<double>(left_value); }
            else if (holds_alternative<bool>(left_value)) { left_int = get<bool>(left_value); }
            if (holds_alternative<int>(right_value)) { right_int = get<int>(right_value); }
            else if (holds_alternative<double>(right_value)) { right_int = get<double>(right_value); }
            else if (holds_alternative<bool>(right_value)) { right_int = get<bool>(right_value); }
            if (holds_alternative<string>(left_value) && holds_alternative<string>(right_value)) {
                if (op == "==") { return EvalResult(get<string>(left_value) == get<string>(right_value)); }
                else if (op == "!=") { return EvalResult(get<string>(left_value) != get<string>(right_value)); }
                else if (op == "<") { return EvalResult(get<string>(left_value) < get<string>(right_value)); }
                else if (op == "<=") { return EvalResult(get<string>(left_value) <= get<string>(right_value)); }
                else if (op == ">") { return EvalResult(get<string>(left_value) > get<string>(right_value)); }
                else if (op == ">=") { return EvalResult(get<string>(left_value) >= get<string>(right_value)); }
                else { throw invalid_argument("Invalid operation on string type"); }
            }
            if (op == "==") { return EvalResult(left_int == right_int); }
            else if (op == "!=") { return EvalResult(left_int != right_int); }
            else if (op == "<") { return EvalResult(left_int < right_int); }
            else if (op == "<=") { return EvalResult(left_int <= right_int); }
            else if (op == ">") { return EvalResult(left_int > right_int); }
            else if (op == ">=") { return EvalResult(left_int >= right_int); }
            else if (op == "and") { return EvalResult(left_int && right_int); }
            else if (op == "or") { return EvalResult(left_int || right_int); }
        }
        else if (op == "and" || op == "or") {
            bool left_bool;
            bool right_bool;
            if (holds_alternative<int>(left_value)) { left_bool = get<int>(left_value) != 0; }
            else if (holds_alternative<double>(left_value)) { left_bool = get<double>(left_value) != 0; }
            else if (holds_alternative<bool>(left_value)) { left_bool = get<bool>(left_value); }
            if (holds_alternative<int>(right_value)) { right_bool = get<int>(right_value) != 0; }
            else if (holds_alternative<double>(right_value)) { right_bool = get<double>(right_value) != 0; }
            else if (holds_alternative<bool>(right_value)) { right_bool = get<bool>(right_value); }
            if (op == "and") { return EvalResult(left_bool && right_bool); }
            else if (op == "or") { return EvalResult(left_bool || right_bool); }
        }
        else { throw invalid_argument("Invalid binary operation"); }
        return EvalResult(0);
    }
private:
    string op;
    NodePtr left, right;
};

class UnOpNode : public Node {
public:
    UnOpNode(string op, NodePtr child) : op(op), child(move(child)) {}
    EvalResult Evaluate(SymbolTable& symbol_table, FuncTable& func_table, Assembly& assembly) const override {
        EvalResult child_value = child->Evaluate(symbol_table, func_table, assembly);
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
    EvalResult Evaluate(SymbolTable& symbol_table, FuncTable& func_table, Assembly& assembly) const override { return EvalResult(0); }
};

class IntValNode : public Node {
public:
    IntValNode(int val) : value(val) {}
    EvalResult Evaluate(SymbolTable& symbol_table, FuncTable& func_table, Assembly& assembly) const override {
        return EvalResult(value);
    }
private:
    int value;
};

class StringValNode : public Node {
public:
    StringValNode(string val) : value(val) {}
    EvalResult Evaluate(SymbolTable& symbol_table, FuncTable& func_table, Assembly& assembly) const override { 
        return EvalResult(value);
    }
private:
    string value;
};

class VarNode : public Node {
public:
    VarNode(string identifier) : identifier(identifier) {}
    EvalResult Evaluate(SymbolTable& symbol_table, FuncTable& func_table, Assembly& assembly) const override { return symbol_table.getVariable(identifier); }
private:
    string identifier;
};

class ReadNode : public Node {
public:
    ReadNode() = default;
    EvalResult Evaluate(SymbolTable& symbol_table, FuncTable& func_table, Assembly& assembly) const override {
        int value;
        cin >> value;
        return EvalResult(value);
    }
};

class PrintNode : public Node {
public:
    PrintNode(NodePtr expression) : expression(move(expression)) {}
    EvalResult Evaluate(SymbolTable& symbol_table, FuncTable& func_table, Assembly& assembly) const override {
        EvalResult result = expression->Evaluate(symbol_table, func_table, assembly);
        if (holds_alternative<int>(result)) { cout << get<int>(result) << endl; }
        else if (holds_alternative<string>(result)) { cout << get<string>(result) << endl; }
        else if (holds_alternative<double>(result)) { cout << get<double>(result) << endl; }
        else if (holds_alternative<bool>(result)) { cout << get<bool>(result) << endl; }
        return result;
    }
private:
    NodePtr expression;
};

class VarDeclareNode : public Node {
public:
    VarDeclareNode(string identifier, NodePtr expression = make_shared<IntValNode>(0))
        : identifier(identifier), expression(move(expression)) {}
    EvalResult Evaluate(SymbolTable& symbol_table, FuncTable& func_table, Assembly& assembly) const override {
        EvalResult result = expression->Evaluate(symbol_table, func_table, assembly);
        symbol_table.setVariable(identifier, result, true);
        return result;
    }
private:
    string identifier;
    NodePtr expression;
};

class AssignmentNode : public Node {
public:
    AssignmentNode(string identifier, NodePtr expression) : identifier(identifier), expression(move(expression)) {}
    EvalResult Evaluate(SymbolTable& symbol_table, FuncTable& func_table, Assembly& assembly) const override {
        EvalResult result = expression->Evaluate(symbol_table, func_table, assembly);
        symbol_table.setVariable(identifier, result, false);
        return result;
    }
private:
    string identifier;
    NodePtr expression;
};

class WhileNode : public Node {
public:
    WhileNode(NodePtr condition, NodePtr block) : condition(move(condition)), block(move(block)) {}
    EvalResult Evaluate(SymbolTable& symbol_table, FuncTable& func_table, Assembly& assembly) const override {
        EvalResult result = 0;
        while (get<bool>(condition->Evaluate(symbol_table, func_table, assembly))) { result = block->Evaluate(symbol_table, func_table, assembly); }
        return result;
    }
private:
    NodePtr condition, block;
};

class IfNode : public Node {
public:
    IfNode(NodePtr condition, NodePtr block, NodePtr else_block) : condition(move(condition)), block(move(block)), else_block(move(else_block)) {}
    EvalResult Evaluate(SymbolTable& symbol_table, FuncTable& func_table, Assembly& assembly) const override {
        EvalResult result = condition->Evaluate(symbol_table, func_table, assembly);
        if (get<bool>(result)) { return block->Evaluate(symbol_table, func_table, assembly); }
        else { return else_block->Evaluate(symbol_table, func_table, assembly); }
    }
private:
    NodePtr condition, block, else_block;
};

class FuncDeclareNode : public Node {
public:
    FuncDeclareNode(const string& func_name, const vector<string>& args, shared_ptr<Node> block_node)
        : func_name(func_name), args(args), block_node(move(block_node)) {}
    virtual EvalResult Evaluate(SymbolTable& symbol_table, FuncTable& func_table, Assembly& assembly) const override {
        // Evaluate of the declarenode
    }
private:
    string func_name;
    vector<string> args;
    shared_ptr<Node> block_node;
};

class FuncCallNode : public Node {
public:
    FuncCallNode(const string& identifier, const vector<shared_ptr<Node>>& args) : identifier(identifier), args(args) {}
    virtual EvalResult Evaluate(SymbolTable& symbol_table, FuncTable& func_table, Assembly& assembly) const override {
        // Evaluate of the callnode
    }
private:
    string identifier;
    vector<shared_ptr<Node>> args;
};

class ReturnNode : public Node {
public:
    ReturnNode(shared_ptr<Node> return_node) : return_node(move(return_node)) {}
    virtual EvalResult Evaluate(SymbolTable& symbol_table, FuncTable& func_table, Assembly& assembly) const override {
        return return_node->Evaluate(symbol_table, func_table, assembly);
    }
private:
    shared_ptr<Node> return_node;
};

class BlockNode : public Node {
public:
    EvalResult Evaluate(SymbolTable& symbol_table, FuncTable& func_table, Assembly& assembly) const override {
        EvalResult result = 0;
        for (const auto& statement : statements) { result = statement->Evaluate(symbol_table, func_table, assembly); }
        return result;
    }
};