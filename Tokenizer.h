#include <iostream>
#include <string>
#include <cctype>
#include <unordered_map>
#include <stdexcept>
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
    unordered_map<string, string> keywordMap;

public:
    Tokenizer(string src) : source(src), position(0), next({ "", 0 }) {
        keywordMap["+"] = "PLUS";
        keywordMap["-"] = "MINUS";
        keywordMap["*"] = "MULT";
        keywordMap["/"] = "DIV";
        keywordMap["("] = "LPAREN";
        keywordMap[")"] = "RPAREN";
        keywordMap["="] = "ASSIGN";
    }

    void updateNextToken() {
        if (source.empty()) { throw invalid_argument("Empty Input"); }
        while (position < source.size()) {
            char current_char = source[position];
            if (current_char == ' ' || current_char == '\n') { position++; }
            else if (isdigit(current_char)) {
                next.type = "NUMBER";
                next.value = current_char - '0';
                position++;
                while (position < source.size() && isdigit(source[position])) {
                    next.value = next.value * 10 + (source[position] - '0');
                    position++;
                }
                return;
            } else if (isalpha(current_char) || current_char == '_') {
                string identifier;
                identifier += current_char;
                position++;
                while (position < source.size() && (isalpha(source[position]) || isdigit(source[position]) || source[position] == '_')) {
                    identifier += source[position];
                    position++;
                }
                if (keywordMap.find(identifier) != keywordMap.end()) { next.type = keywordMap[identifier]; }
                else if (identifier == "print") { next.type = "PRINT"; }
                else { next.type = identifier; }
                return;
            } else {
                string current_char_str(1, current_char);
                if (keywordMap.find(current_char_str) != keywordMap.end()) { next.type = keywordMap[current_char_str]; }
                else { throw invalid_argument("Invalid character: '" + current_char_str + "'"); }
                next.value = 0;
                position++;
                return;
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