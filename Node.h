#include <iostream>
#include <vector>
#include <variant>
using namespace std;
using NodeValue = variant<int, char>;

class Node;
using NodeList = vector<Node>;

class Node {
public:
    NodeValue value;
    NodeList children;
    Node(const NodeValue& val) : value(val) {}
    int Evaluate() { return 0; };
};

class BinOp : public Node {
public:
    BinOp(const NodeValue& val, const Node& left, const Node& right) : Node(val) {
        children.push_back(left);
        children.push_back(right);
    }
    int Evaluate() {
        int left_value = get<int>(children[0].value);
        int right_value = get<int>(children[1].value);
        char op = get<char>(value);
        switch (op) {
            case '+':
                return left_value + right_value;
            case '-':
                return left_value - right_value;
            case '*':
                return left_value * right_value;
            case '/':
                if (right_value == 0) { throw invalid_argument("Division by zero"); }
                return left_value / right_value;
            default:
                throw invalid_argument("Invalid binary operation");
        }
    }
};

class UnOp : public Node {
public:
    UnOp(const NodeValue& val, const Node& child) : Node(val) {
        children.push_back(child);
    }
    int Evaluate() {
        int child_value = get<int>(children[0].value);
        char op = get<char>(value);
        switch (op) {
            case '+':
                return child_value;
            case '-':
                return -child_value;
            default:
                throw invalid_argument("Invalid unary operation");
        }
    }
};

class IntVal : public Node {
public:
    IntVal(const int& val) : Node(val) {}
    int Evaluate() {
        return get<int>(value);
    }
};

class NoOp : public Node {
public:
    NoOp() : Node(0) {}
    int Evaluate() {
        return 0;
    }
};