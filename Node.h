#include <iostream>
#include <memory>
#include <stdexcept>

using namespace std;

class Node;
using NodePtr = shared_ptr<Node>;

class Node {
public:
    virtual int Evaluate() const = 0;
};

class BinOp : public Node {
public:
    BinOp(char op, NodePtr left, NodePtr right) : op(op), left(move(left)), right(move(right)) {}
    int Evaluate() const override {
        int left_value = left->Evaluate();
        int right_value = right->Evaluate();
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
private:
    char op;
    NodePtr left, right;
};

class UnOp : public Node {
public:
    UnOp(char op, NodePtr child) : op(op), child(move(child)) {}
    int Evaluate() const override {
        int child_value = child->Evaluate();
        switch (op) {
            case '+':
                return child_value;
            case '-':
                return -child_value;
            default:
                throw invalid_argument("Invalid unary operation");
        }
    }
private:
    char op;
    NodePtr child;
};

class IntVal : public Node {
public:
    IntVal(int val) : value(val) {}
    int Evaluate() const override {
        return value;
    }
private:
    int value;
};

class NoOp : public Node {
public:
    int Evaluate() const override {
        return 0;
    }
};