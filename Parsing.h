#include <iostream>
#include <string>
using namespace std;

class Token {
public:
    string type;      
    int value;
};

class Tokenizer {
public:
    string source;
    int position;
    Token next;
    void selectNext(Token token) {
        next = token;
    }
};

class Parser {
public:
    static Tokenizer tokenizer;
    static void parseExpression(string source) {
        
    }
    static int run(string code) {
        tokenizer = Tokenizer();
        tokenizer.source = code;
        tokenizer.position = 0;
        parseExpression(code);
        if (tokenizer.next.type == "EOF") {
            return 0;
        }
        return 1;
    }
};