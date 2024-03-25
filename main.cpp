#include <iostream>
#include <fstream>
#include <string>
#include "Preprocessor.h"
#include "Parser.h"
using namespace std;

Tokenizer Parser::tokenizer = Tokenizer("");
Token Parser::current_token;
Parser parser;
SymbolTable table;

int main(int argc, char *argv[]) {
    if (argc != 2) {
        cout << "Usage: " << argv[0] << " <input.lua>" << endl;
        return 1;
    }

    string filename = argv[1];
    ifstream file(filename);
    if (!file.is_open()) {
        cout << "Error: Unable to open file " << filename << endl;
        return 1;
    }
    string code;
    string line;
    while (getline(file, line)) {
        code += line + '\n';
    }
    file.close();

    string filtered_code = PrePro::filter(code);
    shared_ptr<Node> root = parser.run(filtered_code);
    root->Evaluate(table);

    return 0;
}
