#include <iostream>
#include <string>
#include "Parsing.h"
using namespace std;

int main(int argc, char *argv[]) {
    if (argc < 2) {
        cout << "Usage: " << argv[0] << " <input>" << endl;
        return 1;
    }
    string input = argv[1];
    return Parser::run(input);
}