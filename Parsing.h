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
    int position;
    Token next;

public:
    Tokenizer(string src) : source(src), position(0), next({ "", 0 }) {}

    void updateNextToken() {
        if (source.empty()) {
            throw invalid_argument("Empty Input");
        }
        while (position < source.size()) {
            char current_char = source[position];
            if (current_char == ' ') {
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


class Parser {
private:
    static Tokenizer tokenizer;

public:
    static int parse_expression() {
        int result = 0;
        string op = "PLUS";
        Token current_token = tokenizer.selectNext();
        if (current_token.type == "EOF") { throw invalid_argument("Input cannot be empty"); }
        while (current_token.type != "EOF") {
            if (current_token.type == "INT") {
                if (op == "PLUS") { result += current_token.value; } 
                else if (op == "MINUS") { result -= current_token.value; }
            } else { throw invalid_argument("Input must start with a number and signs cannot be followed by another sign"); }
            Token next_token = tokenizer.selectNext();
            op = next_token.type;
            current_token = tokenizer.selectNext();
            if (op == "INT") { throw invalid_argument("Numbers cannot be followed by another number"); }
            if (op != "EOF" && current_token.type == "EOF") { throw invalid_argument("Expected number"); }
        }
        return result;
    }

    static void run(string code) {
        tokenizer = Tokenizer(code);
        cout << parse_expression() << endl;
    }
};
