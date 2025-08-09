#include <bits/stdc++.h>
using namespace std;

bool match_pattern(const string& input_line, const string& pattern) {
    if (pattern.length() == 1) {
        return input_line.find(pattern) != string::npos;
    }
    else if (pattern == "\\d") { // match digits
        return input_line.find_first_of("0123456789") != string::npos;
    }else if(pattern == "\\w"){
        return input_line.find_first_of("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789") != string::npos;
    }
    else if (pattern == "\\s") { // match whitespace
        for (const auto& l : input_line) {
            if (isspace(static_cast<unsigned char>(l))) {
                return true;
            }
        }
        return false;
    }
    else if (pattern == "\\w") { // match alphanumeric
        for (const auto& l : input_line) {
            if (isdigit(static_cast<unsigned char>(l)) || isalpha(static_cast<unsigned char>(l))) {
                return true;
            }
        }
        return false;
    }
    else if (*pattern.begin() == '[' && *(pattern.end() - 1) == ']') {
        const string group = pattern.substr(1, pattern.length() - 2);
        const bool isNegativeGroup = group[0] == '^';

        if (isNegativeGroup) {
            for (const auto& l : group.substr(1)) {
                if (input_line.find(l) != string::npos) {
                    return false;
                }
            }
            return true;
        }

        for (const auto& l : group) {
            if (input_line.find(l) != string::npos) {
                return true;
            }
        }
        return false;
    }
    else {
        // Default: substring match for any other pattern
        return input_line.find(pattern) != string::npos;
    }
}

int main(int argc, char* argv[]) {
    cout << unitbuf;
    cerr << unitbuf;

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
            return 0; // match found
        } else {
            return 1; // no match
        }
    }
    catch (const runtime_error& e) {
        cerr << e.what() << endl;
        return 1;
    }
}
