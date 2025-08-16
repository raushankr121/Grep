#include <bits/stdc++.h>
using namespace std;

bool matchHere(const string &pattern, const string &text);

// Check if `pattern` matches at the start of `text`
bool match(const string &pattern, const string &text) {
    if (!pattern.empty() && pattern[0] == '^') {
        return matchHere(pattern.substr(1), text);
    }
    for (size_t i = 0; i <= text.size(); i++) {
        if (matchHere(pattern, text.substr(i))) return true;
    }
    return false;
}

bool matchHere(const string &pattern, const string &text) {
    if (pattern.empty()) return true;

    // Handle '$' (end anchor)
    if (pattern == "$") return text.empty();

    // Handle "x?"
    if (pattern.size() >= 2 && pattern[1] == '?') {
        // Option 1: skip the "x?"
        if (matchHere(pattern.substr(2), text)) return true;

        // Option 2: if first char matches, consume one and continue
        if (!text.empty() && (pattern[0] == '.' || pattern[0] == text[0])) {
            if (matchHere(pattern.substr(2), text.substr(1))) return true;
        }
        return false;
    }

    // Handle "x*"
    if (pattern.size() >= 2 && pattern[1] == '*') {
        size_t i = 0;
        // Try all possible repetitions of pattern[0]
        while (i <= text.size() && (i == 0 || pattern[0] == '.' || pattern[0] == text[i-1])) {
            if (matchHere(pattern.substr(2), text.substr(i))) return true;
            i++;
        }
        return false;
    }

    // Handle "x+"
    if (pattern.size() >= 2 && pattern[1] == '+') {
        // Must match at least one occurrence
        if (text.empty() || !(pattern[0] == '.' || pattern[0] == text[0])) return false;

        size_t i = 1;
        while (i <= text.size() && (i == 1 || pattern[0] == '.' || pattern[0] == text[i-1])) {
            if (matchHere(pattern.substr(2), text.substr(i))) return true;
            i++;
        }
        return false;
    }

    // Normal character or dot
    if (!text.empty() && (pattern[0] == '.' || pattern[0] == text[0])) {
        return matchHere(pattern.substr(1), text.substr(1));
    }

    return false;
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        cerr << "Usage: " << argv[0] << " <pattern>\n";
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
