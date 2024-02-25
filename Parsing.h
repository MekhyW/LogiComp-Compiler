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
    Tokenizer(std::string src) : source(src), position(0), next({ "", 0 }) {}

    void updateNextToken() {
        if (source.empty()) {
            throw std::invalid_argument("Empty Input");
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
                throw std::invalid_argument("Invalid char: '" + std::string(1, current_char) + "' NOT ALLOWED !");
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
        while (tokenizer.selectNext().type != "EOF") {
            Token current_token = tokenizer.selectNext();
            if (current_token.type == "INT") {
                result += current_token.value;
                while (tokenizer.selectNext().value == 0) {
                    std::string op = tokenizer.selectNext().type;
                    Token next_token = tokenizer.selectNext();
                    if (next_token.type == "INT") {
                        if (op == "PLUS") {
                            result += next_token.value;
                        } else {
                            result -= next_token.value;
                        }
                    } else {
                        throw std::invalid_argument("Input must be a number for: " + next_token.value);
                    }
                }
                if (tokenizer.selectNext().type != "EOF") {
                    throw std::invalid_argument("Invalid token for end of file!");
                }
                return result;
            } else {
                throw std::invalid_argument("Invalid for char= '" + std::to_string(current_token.value) + "'");
            }
        }
        return result;
    }

    static void run(std::string code) {
        tokenizer = Tokenizer(code);
        std::cout << parse_expression() << std::endl;
    }
};
