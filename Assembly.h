#include <fstream>
#include <string>
using namespace std;

class Assembly {
private:
    string asmcode = "";
    string header = "";
    string footer = "";
    string function_definitions = "";

public:
    void add_instruction(string instruction) {
        asmcode += instruction + "\n";
    }

    void add_label(string label) {
        function_definitions += "\n" + label + ":\n";
    }

    void set_header(string filename) {
        ifstream file(filename);
        string line;
        while (getline(file, line)) { header += line + "\n"; }
        file.close();
    }

    void set_footer(string filename) {
        ifstream file(filename);
        string line;
        while (getline(file, line)) { footer += line + "\n"; }
        file.close();
    }

    void add_function_prologue(int n_args) {
        function_definitions += "PUSH EBP\n";
        function_definitions += "MOV EBP, ESP\n";
        for (int i = 0; i < n_args; ++i) {
            switch (i) {
                case 0:
                    function_definitions += "MOV EAX, [EBP+8]\n";
                    break;
                case 1:
                    function_definitions += "MOV EBX, [EBP+12]\n";
                    break;
                case 2:
                    function_definitions += "MOV ECX, [EBP+16]\n";
                    break;
                case 3:
                    function_definitions += "MOV EDX, [EBP+20]\n";
                    break;
                default:
                    function_definitions += "MOV ESI, [EBP+" + to_string(4 * (i + 2)) + "]\n";
                    break;
            }
        }
    }

    void add_function_epilogue() {
        function_definitions += "MOV ESP, EBP\n";
        function_definitions += "POP EBP\n";
        function_definitions += "RET\n";
    }

    void add_function_instruction(string instruction) {
        function_definitions += instruction + "\n";
    }

    void check_windows() {
        #ifdef _WIN32
            string keywords[] = {"main", "scanf", "printf", "fflush", "stdout", "stdin"};
            string replacements[] = {"_main", "_scanf", "_printf", "_fflush", "_stdout", "_stdin"};
            size_t numKeywords = sizeof(keywords) / sizeof(keywords[0]);
            for (size_t i = 0; i < numKeywords; ++i) {
                size_t pos = 0;
                string keyword = keywords[i];
                string replacement = replacements[i];
                while ((pos = asmcode.find(keyword, pos)) != string::npos) {
                    bool isStandalone = true;
                    if (pos > 0 && isalnum(asmcode[pos - 1])) { isStandalone = false; }
                    if (pos + keyword.length() < asmcode.length() && isalnum(asmcode[pos + keyword.length()])) { isStandalone = false; }
                    if (isStandalone) {
                        asmcode.replace(pos, keyword.length(), replacement);
                        pos += replacement.length();
                    } 
                    else { pos += keyword.length(); }
                }
            }
        #endif
    }

    void write_to_file(string filename) {
        ofstream file(filename);
        file << header;
        file << asmcode;
        file << function_definitions;
        file << footer;
        file.close();
    }

    void clear_asmcode() {
        asmcode.clear();
    }

    string get_asmcode() const {
        return asmcode;
    }
};