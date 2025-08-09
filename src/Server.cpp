#include <iostream>
#include <string>
using namespace std;

bool match_pattern(const string &input_line, const string &pattern) {
    if (pattern.length() == 1 && isalpha(pattern[0])) {
        return input_line.find(pattern) != string::npos;
    } else if (pattern == "\\d") {
        return input_line.find_first_of("123456789") != string::npos;
    } else if (pattern == "\\w") {
        return input_line.find_first_of("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_") != string::npos;
    }else if(pattern.length() >= 2 && pattern[0] == '[' && pattern[pattern.length() - 1] == ']') {
        if(pattern[0] == '^') {
            return input_line.find_first_not_of(pattern.substr(1, pattern.length() - 2)) != string::npos;
        }
        string char_class = pattern.substr(1, pattern.length() - 2);
        return input_line.find_first_of(char_class) != string::npos;
    } else {
        throw runtime_error("Unhandled pattern " + pattern);
    }
}
int main(int argc, char* argv[]) {
    // Flush after every cout / cerr
    cout << unitbuf;
    cerr << unitbuf;

    // You can use print statements as follows for debugging, they'll be visible when running tests.
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

    // Uncomment this block to pass the first stage
    //
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
