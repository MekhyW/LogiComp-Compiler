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
        keywordMap["\n"] = "NEWLINE";
        keywordMap["+"] = "PLUS";
        keywordMap["-"] = "MINUS";
        keywordMap["*"] = "MULT";
        keywordMap["/"] = "DIV";
        keywordMap["%"] = "MOD";
        keywordMap["("] = "LPAREN";
        keywordMap[")"] = "RPAREN";
        keywordMap["="] = "ASSIGN";
        keywordMap["=="] = "EQ";
        keywordMap["~="] = "NEQ";
        keywordMap["<"] = "LT";
        keywordMap["<="] = "LE";
        keywordMap[">"] = "GT";
        keywordMap[">="] = "GE";
        keywordMap["not"] = "NOT";
        keywordMap["and"] = "AND";
        keywordMap["or"] = "OR";
        keywordMap["if"] = "IF";
        keywordMap["else"] = "ELSE";
        keywordMap["then"] = "THEN";
        keywordMap["while"] = "WHILE";
        keywordMap["do"] = "DO";
        keywordMap["end"] = "END";
        keywordMap["read"] = "READ";
        keywordMap["print"] = "PRINT";
    }

    void updateNextToken() {
        if (source.empty()) { throw invalid_argument("Empty Input"); }
        while (position < source.size()) {
            char current_char = source[position];
            if (current_char == ' ' || current_char == '\t') { position++; }
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
                string identifier = "";
                identifier += current_char;
                position++;
                while (position < source.size() && (isalnum(source[position]) || source[position] == '_')) {
                    identifier += source[position];
                    position++;
                }
                if (keywordMap.find(identifier) != keywordMap.end()) { next.type = keywordMap[identifier]; }
                else { next.type = identifier; }
                return;
            } else {
                string identifier = "";
                identifier += current_char;
                position++;
                while (position < source.size() && source[position] == '=') {
                    identifier += source[position];
                    position++;
                }
                if (keywordMap.find(identifier) != keywordMap.end()) { next.type = keywordMap[identifier]; }
                else { throw invalid_argument("Invalid word: '" + identifier + "'"); }
                return;
            }
        }
        next.type = "EOF";
        next.value = 0;
    }

    Token selectNext() {
        updateNextToken();
        cout << "Type: " << next.type << ", Value: " << next.value << endl;
        return next;
    }
};