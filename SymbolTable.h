#include <iostream>
#include <unordered_map>
#include <string>
#include <stdexcept>
#include <variant>
using namespace std;

class Node;
using NodePtr = shared_ptr<Node>;
using EvalResult = std::variant<int, string, double, bool>;

struct FuncInfo {
    vector<string> args;
    NodePtr block;
};

class SymbolTable {
private:
    unordered_map<string, EvalResult> variables;
    unordered_map<string, int> offsets;
    int offset = 4;
public:
    bool is_parent = true;
    void setVariable(const string& name, EvalResult value, bool declare = false) {
        if (!declare && variables.find(name) == variables.end()) { throw invalid_argument("Undefined variable: " + name); }
        if (declare) { 
            offsets[name] = offset;
            offset += 4;
        }
        variables[name] = value;
    }
    EvalResult getVariable(const string& name) {
        auto it = variables.find(name);
        if (it != variables.end()) { return it->second; }
        else { throw invalid_argument("Undefined variable: " + name); }
    }
    int getOffset(const string& name) {
        auto it = offsets.find(name);
        if (it != offsets.end()) { return it->second; }
        else { throw invalid_argument("Undefined variable: " + name); }
    }
    SymbolTable copy() { 
        SymbolTable copy;
        copy.variables = variables;
        copy.offsets = offsets;
        copy.offset = offset;
        copy.is_parent = false;
        return copy;
    }
};

class FuncTable {
private:
    unordered_map<string, FuncInfo> functions;
public:
    void setFunction(const string& name, const vector<string>& args, const NodePtr& block) {
        if (functions.find(name) != functions.end()) { throw invalid_argument("Function already declared: " + name); }
        functions[name] = {args, block};
    }
    FuncInfo getFunction(const string& name) {
        auto it = functions.find(name);
        if (it != functions.end()) { return it->second; } 
        else { throw invalid_argument("Undefined function: " + name); }
    }
    FuncTable copy() { 
        FuncTable copy;
        copy.functions = functions;
        return copy;
    }
};