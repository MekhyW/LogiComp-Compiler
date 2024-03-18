#include <iostream>
#include <string>
using namespace std;

class Token {
public:
    string type;      
    int value;
};


class Tokenizer {
private:
    string source;
    size_t position;
    Token next;

public:
    Tokenizer(string src) : source(src), position(0), next({ "", 0 }) {}

    void updateNextToken() {
        if (source.empty()) {
            throw invalid_argument("Empty Input");
        }
        while (position < source.size()) {
            char current_char = source[position];
            if (current_char == ' ' || current_char == '\n') {
                position++;
            } else if (current_char == '+') {
                next.type = "PLUS";
                next.value = 0;
                position++;
                return;
            } else if (current_char == '-') {
                next.type = "MINUS";
                next.value = 0;
                position++;
                return;
            } else if (current_char == '*') {
                next.type = "MULT";
                next.value = 0;
                position++;
                return;
            } else if (current_char == '/') {
                next.type = "DIV";
                next.value = 0;
                position++;
                return;
            } else if (current_char == '(') {
                next.type = "LPAREN";
                next.value = 0;
                position++;
                return;
            } else if (current_char == ')') {
                next.type = "RPAREN";
                next.value = 0;
                position++;
                return;
            } else if (isdigit(current_char)) {
                next.type = "INT";
                next.value = current_char - '0';
                position++;
                while (position < source.size() && isdigit(source[position])) {
                    next.value = next.value * 10 + (source[position] - '0');
                    position++;
                }
                return;
            } else {
                throw invalid_argument("Invalid char: '" + string(1, current_char) + "' NOT ALLOWED !");
            }
        }
        next.type = "EOF";
        next.value = 0;
    }

    Token selectNext() {
        updateNextToken();
        return next;
    }
};