#include <iostream>
#include <string>
using namespace std;

bool match_pattern(const string &input_line, const string &pattern) {
    if (pattern.length() == 1 && isalpha(pattern[0])) {
        return input_line.find(pattern) != string::npos;
    } 
    else if (pattern == "\\d") {
        return input_line.find_first_of("0123456789") != string::npos;
    } 
    else if (pattern == "\\w") {
        return input_line.find_first_of("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_") != string::npos;
    }
    else if (pattern.length() >= 2 && pattern[0] == '[' && pattern[pattern.length() - 1] == ']') {
        if (pattern[1] == '^')
            return input_line.find_first_not_of(pattern.substr(2, pattern.size() - 3)) != string::npos;
        return input_line.find_first_of(pattern.substr(1, pattern.size() - 2)) != string::npos;
    } 
    else { // Multi-token sequential pattern
        auto matches_token = [&](char pat, char ch) {
            if (pat == 'd') return isdigit(static_cast<unsigned char>(ch));
            if (pat == 'w') return isalnum(static_cast<unsigned char>(ch)) || ch == '_';
            return false;
        };

        for (size_t start = 0; start < input_line.size(); ++start) {
            size_t i = start;
            size_t j = 0;
            bool matched = true;

            while (j < pattern.size() && i < input_line.size()) {
                if (pattern[j] == '\\') {
                    if (j + 1 >= pattern.size() || !matches_token(pattern[j + 1], input_line[i])) {
                        matched = false; break;
                    }
                    j += 2; i += 1;
                }
                else if (pattern[j] == '[') {
                    bool negate = (j + 1 < pattern.size() && pattern[j + 1] == '^');
                    size_t end = pattern.find(']', j + 1);
                    if (end == string::npos) return false; // invalid
                    string chars = pattern.substr(j + (negate ? 2 : 1), end - j - (negate ? 2 : 1));
                    bool in_set = chars.find(input_line[i]) != string::npos;
                    if ((negate && in_set) || (!negate && !in_set)) { matched = false; break; }
                    j = end + 1; i += 1;
                }
                else {
                    if (pattern[j] != input_line[i]) { matched = false; break; }
                    j += 1; i += 1;
                }
            }

            if (matched && j == pattern.size()) return true;
        }
        return false;
    }
}

int main(int argc, char* argv[]) {
    cout << unitbuf;
    cerr << unitbuf;

    cerr << "Logs from your program will appear here" << endl;

    if (argc != 3) {
        cerr << "Expected two arguments" << endl;
        return 1;
    }

    string flag = argv[1];
    string pattern = argv[2];

    if (flag != "-E") {
        cerr << "Expected first argument to be '-E'" << endl;
        return 1;
    }

    string input_line;
    getline(cin, input_line);
    
    try {
        if (match_pattern(input_line, pattern)) {
            return 0;
        } else {
            return 1;
        }
    } catch (const runtime_error& e) {
        cerr << e.what() << endl;
        return 1;
    }
}
