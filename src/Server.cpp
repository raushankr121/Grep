#include <iostream>
#include <string>
#include <vector>
#include <stdexcept>
#include <cctype>

using namespace std;

struct PatternElement {
    enum Type { LITERAL, DIGIT, WORD, DIGIT_PLUS, WORD_PLUS, CHAR_GROUP, NEG_CHAR_GROUP };
    Type type;
    string value; // For literals and character groups
};

vector<PatternElement> parse_pattern(const string &pattern) {
    vector<PatternElement> elements;
    size_t i = 0;

    while (i < pattern.length()) {
        if (pattern[i] == '\\' && i + 1 < pattern.length()) {
            char next = pattern[i + 1];
            if (next == 'd') {
                if (i + 2 < pattern.length() && pattern[i + 2] == '+') {
                    elements.push_back({PatternElement::DIGIT_PLUS, ""});
                    i += 3;
                } else {
                    elements.push_back({PatternElement::DIGIT, ""});
                    i += 2;
                }
            }
            else if (next == 'w') {
                if (i + 2 < pattern.length() && pattern[i + 2] == '+') {
                    elements.push_back({PatternElement::WORD_PLUS, ""});
                    i += 3;
                } else {
                    elements.push_back({PatternElement::WORD, ""});
                    i += 2;
                }
            }
            else {
                // Escaped literal
                elements.push_back({PatternElement::LITERAL, string(1, next)});
                i += 2;
            }
        }
        else if (pattern[i] == '[') {
            size_t close = pattern.find(']', i + 1);
            if (close == string::npos) {
                throw runtime_error("Unmatched '[' in pattern");
            }
            string group = pattern.substr(i + 1, close - i - 1);
            if (!group.empty() && group[0] == '^') {
                elements.push_back({PatternElement::NEG_CHAR_GROUP, group.substr(1)});
            } else {
                elements.push_back({PatternElement::CHAR_GROUP, group});
            }
            i = close + 1;
        }
        else {
            elements.push_back({PatternElement::LITERAL, string(1, pattern[i])});
            i++;
        }
    }
    return elements;
}

bool matches_element(char ch, const PatternElement& elem) {
    switch (elem.type) {
        case PatternElement::LITERAL:
            return ch == elem.value[0];
        case PatternElement::DIGIT:
        case PatternElement::DIGIT_PLUS:
            return isdigit(static_cast<unsigned char>(ch));
        case PatternElement::WORD:
        case PatternElement::WORD_PLUS:
            return isalnum(static_cast<unsigned char>(ch)) || ch == '_';
        case PatternElement::CHAR_GROUP:
            return elem.value.find(ch) != string::npos;
        case PatternElement::NEG_CHAR_GROUP:
            return elem.value.find(ch) == string::npos;
    }
    return false;
}

bool match_at_position(const string& input, size_t pos, const vector<PatternElement>& elements, size_t elem_idx) {
    if (elem_idx >= elements.size()) return true;
    if (pos >= input.length()) return false;

    const PatternElement& elem = elements[elem_idx];

    if (elem.type == PatternElement::DIGIT_PLUS || elem.type == PatternElement::WORD_PLUS) {
        if (!matches_element(input[pos], elem)) return false;
        size_t match_end = pos + 1;
        while (match_end < input.length() && matches_element(input[match_end], elem)) {
            match_end++;
        }
        for (size_t end = match_end; end > pos; end--) {
            if (match_at_position(input, end, elements, elem_idx + 1)) return true;
        }
        return false;
    } else {
        if (matches_element(input[pos], elem)) {
            return match_at_position(input, pos + 1, elements, elem_idx + 1);
        }
        return false;
    }
}

bool match_pattern(const string &input_line, const string &pattern) {
    // Quick single-char check
    if (pattern.length() == 1) {
        return input_line.find(pattern) != string::npos;
    }

    // Parse and match
    vector<PatternElement> elements = parse_pattern(pattern);
    for (size_t i = 0; i < input_line.length(); i++) {
        if (match_at_position(input_line, i, elements, 0)) {
            return true;
        }
    }
    return false;
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
