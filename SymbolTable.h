#include <iostream>
#include <unordered_map>
#include <string>
#include <stdexcept>
#include <variant>
using namespace std;

using EvalResult = std::variant<int, string, double, bool>;

class SymbolTable {
private:
    unordered_map<string, EvalResult> variables;
public:
    void setVariable(const string& name, EvalResult value, bool declare = false) {
        if (declare && variables.find(name) != variables.end()) { throw invalid_argument("Variable already declared: " + name); }
        else if (!declare && variables.find(name) == variables.end()) { throw invalid_argument("Undefined variable: " + name); }
        variables[name] = value;
    }
    EvalResult getVariable(const string& name) {
        auto it = variables.find(name);
        if (it != variables.end()) { return it->second; }
        else { throw invalid_argument("Undefined variable: " + name); }
    }
};

class FuncTable {
private:
    static unordered_map<string, shared_ptr<Node>> functions;
public:
    static void setFunction(const string& name, shared_ptr<Node> value, bool declare = false) {
        if (declare && functions.find(name) != functions.end()) { throw invalid_argument("Function already declared: " + name); }
        else if (!declare && functions.find(name) == functions.end()) { throw invalid_argument("Undefined function: " + name); }
        functions[name] = value;
    }
    static shared_ptr<Node> getFunction(const string& name) {
        auto it = functions.find(name);
        if (it != functions.end()) { return it->second; }
        else { throw invalid_argument("Undefined function: " + name); }
    }
};