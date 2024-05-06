#include <iostream>
#include <fstream>
#include <string>
#include "Preprocessor.h"
#include "Assembly.h"
#include "Parser.h"
using namespace std;

Tokenizer Parser::tokenizer = Tokenizer("");
Token Parser::current_token;
Parser parser;
SymbolTable table;
Assembly assembly;

int main(int argc, char *argv[]) {
    // Read lua code from file
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
    while (getline(file, line)) { code += line + '\n'; }
    file.close();

    // Preprocess
    string filtered_code = PrePro::preprocess(code);

    // Parse
    shared_ptr<Node> root = parser.run(filtered_code);

    // Compile
    string asmcode = "";
    asmcode = assembly.add_file(asmcode, "Assembly/header.asm");
    root->Evaluate(table, assembly);
    asmcode = assembly.add_file(asmcode, "Assembly/footer.asm");
    asmcode = assembly.check_windows(asmcode);
    assembly.write_to_file(asmcode, filename.substr(0, filename.size() - 4) + ".asm");

    return 0;
}
