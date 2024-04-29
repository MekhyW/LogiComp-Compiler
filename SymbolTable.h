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
        variables[name] = value;
    }

    EvalResult getVariable(const string& name) {
        auto it = variables.find(name);
        if (it != variables.end()) { return it->second; }
        else { throw invalid_argument("Undefined variable: " + name); }
    }
};