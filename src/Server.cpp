#include <iostream>
#include <string>
#include <vector>
#include <stdexcept>
using namespace std;

bool match_pattern(const string &input_line, const string &pattern) {
    if (pattern.length() == 1 && isalpha(pattern[0])) {
        return input_line.find(pattern) != string::npos;
    } 
    else if (pattern == "\\d") {
        return input_line.find_first_of("0123456789") != string::npos; // include 0
    } 
    else if (pattern == "\\w") {
        return input_line.find_first_of("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_") != string::npos;
    }
    else if(pattern.length() >= 2 && pattern[0] == '[' && pattern[pattern.length() - 1] == ']') {
        if (pattern[1] == '^') {
            string chars = pattern.substr(2, pattern.size() - 3); // fix substring
            return input_line.find_first_not_of(chars) != string::npos;
        }
        string chars = pattern.substr(1, pattern.size() - 2);
        return input_line.find_first_of(chars) != string::npos;
    } 
    else {
        throw runtime_error("Unhandled pattern " + pattern);
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
            return 0;
        } else {
            return 1;
        }
    } catch (const runtime_error& e) {
        cerr << e.what() << endl;
        return 1;
    }
}

