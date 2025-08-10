#include <iostream>
#include <string>
#include <vector>
using namespace std;

int tokenize_step(int i, const string& pattern) {
    if (pattern[i] == '[') {
        while (++i < pattern.size() && pattern[i] != ']');
    } else if (pattern[i] == '\\') {
        ++i; // skip the escape
    }
    return i + 1;
}

void tokenize_pattern(const string& pattern, vector<string>& tokens) {
    int i = 0;
    while (i < pattern.size()) {
        int j = tokenize_step(i, pattern);
        tokens.push_back(pattern.substr(i, j - i));
        i = j;
    }
}

bool match_token(const string& token, char c) {
    if (token.size() == 1) {
        return c == token[0];
    } else if (token == "\\d") {
        return isdigit(c);
    } else if (token == "\\w") {
        return c == '_' || isalnum(c);
    } else if (token.size() > 2 && token[0] == '[' && token.back() == ']') {
        if (token[1] == '^')
            return token.substr(2, token.size() - 3).find(c) == string::npos;
        return token.substr(1, token.size() - 2).find(c) != string::npos;
    }
    throw runtime_error("Unhandled token " + token);
}

bool match_sequence_at(int pos, const string& input, const vector<string>& tokens) {
    if (pos + tokens.size() > input.size()) return false;
    for (size_t i = 0; i < tokens.size(); i++) {
        if (!match_token(tokens[i], input[pos + i])) return false;
    }
    return true;
}

bool match_pattern(const string &input_line, const string &pattern) {
    vector<string> tokens;
    tokenize_pattern(pattern, tokens);

    if (tokens.size() == 1) {
        return match_token(tokens[0], input_line[input_line.find_first_not_of("")]);
    } else {
        for (int i = 0; i <= (int)input_line.size() - (int)tokens.size(); i++) {
            if (match_sequence_at(i, input_line, tokens)) return true;
        }
        return false;
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
