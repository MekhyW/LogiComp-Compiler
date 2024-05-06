#include <fstream>
#include <string>
using namespace std;

class Assembly {
public:
    static string add_instruction(string asmcode, string instruction) {
        asmcode += instruction + "\n";
        return asmcode;
    }

    static string add_label(string asmcode, string label) {
        asmcode += label + ":\n";
        return asmcode;
    }

    static string add_file(string asmcode, string filename) {
        ifstream file(filename);
        string line;
        while (getline(file, line)) { asmcode += line + "\n"; }
        file.close();
        return asmcode;
    }

    static string check_windows(string asmcode) {
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
        return asmcode;
    }

    static void write_to_file(string asmcode, string filename) {
        ofstream file(filename);
        file << asmcode;
        file.close();
    }
};