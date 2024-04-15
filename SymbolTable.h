#include <iostream>
#include <unordered_map>
#include <string>
#include <stdexcept>
using namespace std;

class SymbolTable {
private:
    unordered_map<string, int> variables;

public:
    void setVariable(const string& name, int value) {
        variables[name] = value;
    }

    int getVariable(const string& name) {
        if (variables.find(name) != variables.end()) { return variables.at(name); }
        else { throw invalid_argument("Undefined variable: " + name); }
    }
};
