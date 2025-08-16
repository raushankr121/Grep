#include <bits/stdc++.h>
using namespace std;

bool matchHere(const string &pattern, const string &text);

bool matchStar(char c, const string &pattern, const string &text) {
    for (int i = 0; i <= text.size(); i++) {
        if (i > 0 && (c == '.' || text[i-1] == c)) {
            if (matchHere(pattern, text.substr(i))) return true;
        } else if (i == 0) {
            if (matchHere(pattern, text)) return true;
        }
    }
    return false;
}

bool matchPlus(char c, const string &pattern, const string &text) {
    if (text.empty() || (c != '.' && text[0] != c)) return false;
    int i = 1;
    while (i <= text.size() && (c == '.' || text[i-1] == c)) {
        if (matchHere(pattern, text.substr(i))) return true;
        i++;
    }
    return false;
}

bool matchHere(const string &pattern, const string &text) {
    if (pattern.empty()) return true;

    if (pattern.size() >= 2 && pattern[1] == '*')
        return matchStar(pattern[0], pattern.substr(2), text);

    if (pattern.size() >= 2 && pattern[1] == '+')
        return matchPlus(pattern[0], pattern.substr(2), text);

    if (pattern.size() >= 2 && pattern[1] == '?') {
        // zero occurrence
        if (matchHere(pattern.substr(2), text)) return true;
        // one occurrence
        if (!text.empty() && (pattern[0] == '.' || pattern[0] == text[0]))
            return matchHere(pattern.substr(2), text.substr(1));
        return false;
    }

    if (pattern[0] == '$' && pattern.size() == 1) return text.empty();

    if (!text.empty() && (pattern[0] == '.' || pattern[0] == text[0]))
        return matchHere(pattern.substr(1), text.substr(1));

    return false;
}

bool match(const string &pattern, const string &text) {
    if (!pattern.empty() && pattern[0] == '^')
        return matchHere(pattern.substr(1), text);

    for (int i = 0; i <= text.size(); i++) {
        if (matchHere(pattern, text.substr(i)))
            return true;
    }
    return false;
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        cerr << "Usage: ./mygrep <pattern>\n";
        return 1;
    }

    string pattern = argv[1];
    string line;
    while (getline(cin, line)) {
        if (match(pattern, line)) {
            cout << line << "\n";
        }
    }
    return 0;
}
