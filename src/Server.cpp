#include <iostream>
#include <string>
#include <vector>
#include <stdexcept>
#include <cctype>

using namespace std;

struct PatternElement {
    enum Type { 
        LITERAL, DIGIT, WORD, DIGIT_PLUS, WORD_PLUS, 
        CHAR_GROUP, NEG_CHAR_GROUP, LITERAL_PLUS, CHAR_GROUP_PLUS, NEG_CHAR_GROUP_PLUS,
        START_ANCHOR, END_ANCHOR 
    };
    Type type;
    string value;
};

vector<PatternElement> parse_pattern(const string &pattern) {
    vector<PatternElement> elements;
    size_t i = 0;

    while (i < pattern.length()) {
        if (pattern[i] == '^' && i == 0) {
            elements.push_back({PatternElement::START_ANCHOR, ""});
            i++;
        }
        else if (pattern[i] == '$' && i == pattern.length() - 1) {
            elements.push_back({PatternElement::END_ANCHOR, ""});
            i++;
        }
        else if (pattern[i] == '\\' && i + 1 < pattern.length()) {
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
            bool isNeg = (!group.empty() && group[0] == '^');
            PatternElement::Type t = isNeg ? PatternElement::NEG_CHAR_GROUP : PatternElement::CHAR_GROUP;
            string val = isNeg ? group.substr(1) : group;
            // Check if followed by +
            if (close + 1 < pattern.length() && pattern[close + 1] == '+') {
                t = isNeg ? PatternElement::NEG_CHAR_GROUP_PLUS : PatternElement::CHAR_GROUP_PLUS;
                close++;
            }
            elements.push_back({t, val});
            i = close + 1;
        }
        else {
            // Literal
            string lit(1, pattern[i]);
            if (i + 1 < pattern.length() && pattern[i + 1] == '+') {
                elements.push_back({PatternElement::LITERAL_PLUS, lit});
                i += 2;
            } else {
                elements.push_back({PatternElement::LITERAL, lit});
                i++;
            }
        }
    }
    return elements;
}

bool matches_element(char ch, const PatternElement& elem) {
    switch (elem.type) {
        case PatternElement::LITERAL:
        case PatternElement::LITERAL_PLUS:
            return ch == elem.value[0];
        case PatternElement::DIGIT:
        case PatternElement::DIGIT_PLUS:
            return isdigit(static_cast<unsigned char>(ch));
        case PatternElement::WORD:
        case PatternElement::WORD_PLUS:
            return isalnum(static_cast<unsigned char>(ch)) || ch == '_';
        case PatternElement::CHAR_GROUP:
        case PatternElement::CHAR_GROUP_PLUS:
            return elem.value.find(ch) != string::npos;
        case PatternElement::NEG_CHAR_GROUP:
        case PatternElement::NEG_CHAR_GROUP_PLUS:
            return elem.value.find(ch) == string::npos;
        case PatternElement::START_ANCHOR:
        case PatternElement::END_ANCHOR:
            return false;
    }
    return false;
}

bool is_plus_type(PatternElement::Type t) {
    return t == PatternElement::DIGIT_PLUS || t == PatternElement::WORD_PLUS ||
           t == PatternElement::LITERAL_PLUS || t == PatternElement::CHAR_GROUP_PLUS ||
           t == PatternElement::NEG_CHAR_GROUP_PLUS;
}

bool match_at_position(const string& input, size_t pos, const vector<PatternElement>& elements, size_t elem_idx) {
    if (elem_idx >= elements.size()) return true;

    const PatternElement& elem = elements[elem_idx];

    if (elem.type == PatternElement::START_ANCHOR) {
        if (pos != 0) return false;
        return match_at_position(input, pos, elements, elem_idx + 1);
    }

    if (elem.type == PatternElement::END_ANCHOR) {
        return pos == input.length();
    }

    if (pos >= input.length()) return false;

    if (is_plus_type(elem.type)) {
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
    vector<PatternElement> elements = parse_pattern(pattern);

    if (!elements.empty() && elements[0].type == PatternElement::START_ANCHOR) {
        return match_at_position(input_line, 0, elements, 0);
    } else {
        for (size_t i = 0; i <= input_line.length(); i++) {
            if (match_at_position(input_line, i, elements, 0)) {
                return true;
            }
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
