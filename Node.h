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
    string type;
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
    BinOpNode(string op, NodePtr left, NodePtr right) : op(op), left(move(left)), right(move(right)) {type = "BinOpNode";}
    EvalResult Evaluate(SymbolTable& symbol_table, FuncTable& func_table, Assembly& assembly) const override {
        EvalResult right_value = right->Evaluate(symbol_table, func_table, assembly);
        assembly.add_instruction("PUSH EAX");
        EvalResult left_value = left->Evaluate(symbol_table, func_table, assembly);
        assembly.add_instruction("POP EBX");
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
            if (op == "+") { 
                assembly.add_instruction("ADD EAX, EBX");
                return EvalResult(left_int + right_int); 
            }
            else if (op == "-") { 
                assembly.add_instruction("SUB EAX, EBX");
                return EvalResult(left_int - right_int); 
            }
            else if (op == "*") { 
                assembly.add_instruction("IMUL EAX, EBX");
                return EvalResult(left_int * right_int); 
            }
            else if (op == "/") {
                if (right_int == 0) { throw invalid_argument("Division by zero"); }
                assembly.add_instruction("IDIV EAX, EBX");
                return EvalResult(left_int / right_int);
            }
            else if (op == "%") {
                if (right_int == 0) { throw invalid_argument("Division by zero"); }
                assembly.add_instruction("IDIV EAX, EBX");
                assembly.add_instruction("MOV EAX, EBX");
                return EvalResult(left_int % right_int);
            }
        }
        else if (op == "==" || op == "!=" || op == "<" || op == "<=" || op == ">" || op == ">=") { 
            assembly.add_instruction("CMP EAX, EBX");
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
            if (op == "==") { 
                assembly.add_instruction("CALL binop_je");
                return EvalResult(left_int == right_int); 
            }
            else if (op == "!=") { 
                assembly.add_instruction("CALL binop_jne");
                return EvalResult(left_int != right_int); 
            }
            else if (op == "<") { 
                assembly.add_instruction("CALL binop_jl");
                return EvalResult(left_int < right_int); 
            }
            else if (op == "<=") { 
                assembly.add_instruction("CALL binop_jle");
                return EvalResult(left_int <= right_int); 
            }
            else if (op == ">") { 
                assembly.add_instruction("CALL binop_jg");
                return EvalResult(left_int > right_int); 
            }
            else if (op == ">=") { 
                assembly.add_instruction("CALL binop_jge");
                return EvalResult(left_int >= right_int); 
            }
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
            if (op == "and") { 
                assembly.add_instruction("AND EAX, EBX");
                return EvalResult(left_bool && right_bool); 
            }
            else if (op == "or") { 
                assembly.add_instruction("OR EAX, EBX");
                return EvalResult(left_bool || right_bool); 
            }
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
    UnOpNode(string op, NodePtr child) : op(op), child(move(child)) {type = "UnOpNode";}
    EvalResult Evaluate(SymbolTable& symbol_table, FuncTable& func_table, Assembly& assembly) const override {
        EvalResult child_value = child->Evaluate(symbol_table, func_table, assembly);
        if (op == "+") { return get<int>(child_value); }
        else if (op == "-") { 
            assembly.add_instruction("NEG EAX");
            return -get<int>(child_value); 
        }
        else if (op == "not") { 
            assembly.add_instruction("NOT EAX");
            return !get<bool>(child_value); 
        }
        else { throw invalid_argument("Invalid unary operation"); }
    }
private:
    string op;
    NodePtr child;
};

class NoOpNode : public Node {
public:
    NoOpNode() {type = "NoOpNode";}
    EvalResult Evaluate(SymbolTable& symbol_table, FuncTable& func_table, Assembly& assembly) const override { 
        assembly.add_instruction("NOP");
        return EvalResult("NULL"); 
    }
};

class IntValNode : public Node {
public:
    IntValNode(int val) : value(val) {type = "IntValNode";}
    EvalResult Evaluate(SymbolTable& symbol_table, FuncTable& func_table, Assembly& assembly) const override {
        assembly.add_instruction("MOV EAX, " + to_string(value));
        return EvalResult(value);
    }
private:
    int value;
};

class StringValNode : public Node {
public:
    StringValNode(string val) : value(val) {type = "StringValNode";}
    EvalResult Evaluate(SymbolTable& symbol_table, FuncTable& func_table, Assembly& assembly) const override { 
        return EvalResult(value);
    }
private:
    string value;
};

class VarNode : public Node {
public:
    VarNode(string identifier) : identifier(identifier) {type = "VarNode";}
    EvalResult Evaluate(SymbolTable& symbol_table, FuncTable& func_table, Assembly& assembly) const override {
        int offset = symbol_table.getOffset(identifier);
        assembly.add_instruction("MOV EAX, [EBP-" + to_string(offset) + "]");
        return symbol_table.getVariable(identifier); 
    }
private:
    string identifier;
};

class ReadNode : public Node {
public:
    ReadNode() {type = "ReadNode";}
    EvalResult Evaluate(SymbolTable& symbol_table, FuncTable& func_table, Assembly& assembly) const override {
        assembly.add_instruction("PUSH scanint");
        assembly.add_instruction("PUSH formatin");
        assembly.add_instruction("CALL scanf");
        assembly.add_instruction("ADD ESP, 8");
        assembly.add_instruction("MOV EAX, DWORD [scanint]");
        int value;
        cin >> value;
        return EvalResult(value);
    }
};

class PrintNode : public Node {
public:
    PrintNode(NodePtr expression) : expression(move(expression)) {type = "PrintNode";}
    EvalResult Evaluate(SymbolTable& symbol_table, FuncTable& func_table, Assembly& assembly) const override {
        EvalResult result = expression->Evaluate(symbol_table, func_table, assembly);
        assembly.add_instruction("PUSH EAX");
        assembly.add_instruction("PUSH formatout");
        assembly.add_instruction("CALL printf");
        assembly.add_instruction("ADD ESP, 8");
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
        : identifier(identifier), expression(move(expression)) {type = "VarDeclareNode";}
    EvalResult Evaluate(SymbolTable& symbol_table, FuncTable& func_table, Assembly& assembly) const override {
        assembly.add_instruction("PUSH DWORD 0");
        EvalResult result = expression->Evaluate(symbol_table, func_table, assembly);
        if (result == EvalResult("NULL")) { throw invalid_argument("Cannot assign NULL value to variable " + identifier); }
        symbol_table.setVariable(identifier, result, true);
        return result;
    }
private:
    string identifier;
    NodePtr expression;
};

class AssignmentNode : public Node {
public:
    AssignmentNode(string identifier, NodePtr expression) : identifier(identifier), expression(move(expression)) {type = "AssignmentNode";}
    EvalResult Evaluate(SymbolTable& symbol_table, FuncTable& func_table, Assembly& assembly) const override {
        EvalResult result = expression->Evaluate(symbol_table, func_table, assembly);
        if (result == EvalResult("NULL")) { throw invalid_argument("Cannot assign NULL value to variable " + identifier); }
        int offset = symbol_table.getOffset(identifier);
        assembly.add_instruction("MOV [EBP-" + to_string(offset) + "], EAX");
        symbol_table.setVariable(identifier, result, false);
        return result;
    }
private:
    string identifier;
    NodePtr expression;
};

class WhileNode : public Node {
public:
    WhileNode(NodePtr condition, NodePtr block) 
        : condition(move(condition)), block(move(block)) { type = "WhileNode"; }
    EvalResult Evaluate(SymbolTable& symbol_table, FuncTable& func_table, Assembly& assembly) const override {
        int label_id = this->id;
        string start_label = "LOOP_" + to_string(label_id);
        string end_label = "EXIT_" + to_string(label_id);
        assembly.add_label(start_label);
        condition->Evaluate(symbol_table, func_table, assembly);
        assembly.add_instruction("CMP EAX, False");
        assembly.add_instruction("JE " + end_label);
        block->Evaluate(symbol_table, func_table, assembly);
        assembly.add_instruction("JMP " + start_label);
        assembly.add_label(end_label);
        return EvalResult("NULL");
    }
private:
    NodePtr condition, block;
};

class IfNode : public Node {
public:
    IfNode(NodePtr condition, NodePtr block, NodePtr else_block) 
        : condition(move(condition)), block(move(block)), else_block(move(else_block)) { type = "IfNode"; }
    EvalResult Evaluate(SymbolTable& symbol_table, FuncTable& func_table, Assembly& assembly) const override {
        int label_id = this->id;
        string true_label = "IF_" + to_string(label_id);
        string false_label = "ELSE_" + to_string(label_id);
        string end_label = "EXIT_" + to_string(label_id);
        assembly.add_label(true_label);
        condition->Evaluate(symbol_table, func_table, assembly);
        assembly.add_instruction("CMP EAX, False");
        assembly.add_instruction("JE " + false_label);
        block->Evaluate(symbol_table, func_table, assembly);
        assembly.add_instruction("JMP " + end_label);
        assembly.add_label(false_label);
        else_block->Evaluate(symbol_table, func_table, assembly);
        assembly.add_label(end_label);
        return EvalResult("NULL");
    }

private:
    NodePtr condition, block, else_block;
};

class FuncDeclareNode : public Node {
public:
    FuncDeclareNode(const string& func_name, const vector<string>& args, NodePtr block_node)
        : func_name(func_name), args(args), block_node(move(block_node)) {type = "FuncDeclareNode";}
    virtual EvalResult Evaluate(SymbolTable& symbol_table, FuncTable& func_table, Assembly& assembly) const override {
        func_table.setFunction(func_name, args, block_node);
        return EvalResult("NULL");
    }
private:
    string func_name;
    vector<string> args;
    NodePtr block_node;
};

class FuncCallNode : public Node {
public:
    FuncCallNode(const string& identifier, const vector<NodePtr>& args) : identifier(identifier), args(args) {type = "FuncCallNode";}
    virtual EvalResult Evaluate(SymbolTable& symbol_table, FuncTable& func_table, Assembly& assembly) const override {
        FuncInfo func_info = func_table.getFunction(identifier);
        if (func_info.args.size() != args.size()) { throw invalid_argument("Function " + identifier + " expects " + to_string(func_info.args.size()) + " arguments, but " + to_string(args.size()) + " were given"); }
        SymbolTable new_symbol_table = SymbolTable();
        for (int i = 0; i < func_info.args.size(); i++) { new_symbol_table.setVariable(func_info.args[i], args[i]->Evaluate(symbol_table, func_table, assembly), true); }
        return func_info.block->Evaluate(new_symbol_table, func_table, assembly);
    }
private:
    string identifier;
    vector<NodePtr> args;
};

class ReturnNode : public Node {
public:
    ReturnNode(NodePtr return_node) : return_node(move(return_node)) {type = "ReturnNode";}
    virtual EvalResult Evaluate(SymbolTable& symbol_table, FuncTable& func_table, Assembly& assembly) const override {
        return return_node->Evaluate(symbol_table, func_table, assembly);
    }
private:
    NodePtr return_node;
};

class BlockNode : public Node {
public:
    BlockNode() {type = "BlockNode";}
    EvalResult Evaluate(SymbolTable& symbol_table, FuncTable& func_table, Assembly& assembly) const override {
        for (const auto& statement : statements) {
            if (statement->type == "ReturnNode") { return statement->Evaluate(symbol_table, func_table, assembly); }
            else { statement->Evaluate(symbol_table, func_table, assembly); }
        }
        return EvalResult("NULL");
    }
};