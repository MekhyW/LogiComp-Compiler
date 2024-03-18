#include <string>
using namespace std;

class PrePro {
public:
    static string filter(const string& input) {
        string filtered;
        bool in_comment = false;
        for (size_t i = 0; i < input.size(); ++i) {
            if (input[i] == '-' && i + 1 < input.size() && input[i + 1] == '-') {
                in_comment = true;
                ++i;
            } else if (input[i] == '\n') {
                in_comment = false;
                filtered += input[i];
            } else if (!in_comment) {
                filtered += input[i];
            }
        }
        return filtered;
    }
};