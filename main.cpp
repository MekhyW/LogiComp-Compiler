#include <iostream>
#include <string>
#include "Parsing.h"
using namespace std;

Tokenizer Parser::tokenizer = Tokenizer("");

int main(int argc, char *argv[]) {
    if (argc < 2) {
        cout << "Usage: " << argv[0] << " <input>" << endl;
        return 1;
    }
    Parser::run(argv[1]);
    return 0;
}