#include <iostream>
#include <string>
#include <vector>
#include <stdexcept>
#include <cctype>

using namespace std;

struct PatternElement {
    enum Type {
        LITERAL, DIGIT, WORD,
        DIGIT_PLUS, WORD_PLUS,
        LITERAL_PLUS, CHAR_GROUP_PLUS, NEG_CHAR_GROUP_PLUS,
        CHAR_GROUP, NEG_CHAR_GROUP,
        // optional (zero-or-one)
        DIGIT_OPT, WORD_OPT, LITERAL_OPT, CHAR_GROUP_OPT, NEG_CHAR_GROUP_OPT,
        // anchors
        START_ANCHOR, END_ANCHOR
    };
    Type type;
    string value; // For literals and (neg)char groups
};

static bool is_plus_type(PatternElement::Type t) {
    return t == PatternElement::DIGIT_PLUS || t == PatternElement::WORD_PLUS ||
           t == PatternElement::LITERAL_PLUS || t == PatternElement::CHAR_GROUP_PLUS ||
           t == PatternElement::NEG_CHAR_GROUP_PLUS;
}

static bool is_opt_type(PatternElement::Type t) {
    return t == PatternElement::DIGIT_OPT || t == PatternElement::WORD_OPT ||
           t == PatternElement::LITERAL_OPT || t == PatternElement::CHAR_GROUP_OPT ||
           t == PatternElement::NEG_CHAR_GROUP_OPT;
}

vector<PatternElement> parse_pattern(const string &pattern) {
    vector<PatternElement> elements;
    size_t i = 0;

    auto make_escaped = [&](char cls, size_t &i_ref) {
        // handles \d, \w with optional + or ?
        if (cls == 'd') {
            if (i_ref + 2 < pattern.length() && pattern[i_ref + 2] == '+') {
                elements.push_back({PatternElement::DIGIT_PLUS, ""});
                i_ref += 3;
                return;
            } else if (i_ref + 2 < pattern.length() && pattern[i_ref + 2] == '?') {
                elements.push_back({PatternElement::DIGIT_OPT, ""});
                i_ref += 3;
                return;
            } else {
                elements.push_back({PatternElement::DIGIT, ""});
                i_ref += 2;
                return;
            }
        } else if (cls == 'w') {
            if (i_ref + 2 < pattern.length() && pattern[i_ref + 2] == '+') {
                elements.push_back({PatternElement::WORD_PLUS, ""});
                i_ref += 3;
                return;
            } else if (i_ref + 2 < pattern.length() && pattern[i_ref + 2] == '?') {
                elements.push_back({PatternElement::WORD_OPT, ""});
                i_ref += 3;
                return;
            } else {
                elements.push_back({PatternElement::WORD, ""});
                i_ref += 2;
                return;
            }
        } else {
            // escaped literal (e.g., \^, \$, \[, \\)
            elements.push_back({PatternElement::LITERAL, string(1, cls)});
            i_ref += 2;
            return;
        }
    };

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
            make_escaped(pattern[i + 1], i);
        }
        else if (pattern[i] == '[') {
            size_t close = pattern.find(']', i + 1);
            if (close == string::npos) throw runtime_error("Unmatched '[' in pattern");
            string group = pattern.substr(i + 1, close - i - 1);
            bool isNeg = (!group.empty() && group[0] == '^');
            string val = isNeg ? group.substr(1) : group;

            // check quantifier after the group
            PatternElement::Type t;
            size_t next = close + 1;
            if (next < pattern.length() && (pattern[next] == '+' || pattern[next] == '?')) {
                if (pattern[next] == '+')
                    t = isNeg ? PatternElement::NEG_CHAR_GROUP_PLUS : PatternElement::CHAR_GROUP_PLUS;
                else
                    t = isNeg ? PatternElement::NEG_CHAR_GROUP_OPT  : PatternElement::CHAR_GROUP_OPT;
                elements.push_back({t, val});
                i = next + 1;
            } else {
                t = isNeg ? PatternElement::NEG_CHAR_GROUP : PatternElement::CHAR_GROUP;
                elements.push_back({t, val});
                i = close + 1;
            }
        }
        else {
            // literal with possible + or ?
            string lit(1, pattern[i]);
            if (i + 1 < pattern.length() && (pattern[i + 1] == '+' || pattern[i + 1] == '?')) {
                if (pattern[i + 1] == '+') {
                    elements.push_back({PatternElement::LITERAL_PLUS, lit});
                } else {
                    elements.push_back({PatternElement::LITERAL_OPT, lit});
                }
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
        case PatternElement::LITERAL_OPT:
            return ch == elem.value[0];

        case PatternElement::DIGIT:
        case PatternElement::DIGIT_PLUS:
        case PatternElement::DIGIT_OPT:
            return isdigit(static_cast<unsigned char>(ch));

        case PatternElement::WORD:
        case PatternElement::WORD_PLUS:
        case PatternElement::WORD_OPT:
            return isalnum(static_cast<unsigned char>(ch)) || ch == '_';

        case PatternElement::CHAR_GROUP:
        case PatternElement::CHAR_GROUP_PLUS:
        case PatternElement::CHAR_GROUP_OPT:
            return elem.value.find(ch) != string::npos;

        case PatternElement::NEG_CHAR_GROUP:
        case PatternElement::NEG_CHAR_GROUP_PLUS:
        case PatternElement::NEG_CHAR_GROUP_OPT:
            return elem.value.find(ch) == string::npos;

        case PatternElement::START_ANCHOR:
        case PatternElement::END_ANCHOR:
            return false; // handled elsewhere
    }
    return false;
}

bool match_at_position(const string& input, size_t pos, const vector<PatternElement>& elements, size_t elem_idx) {
    if (elem_idx >= elements.size()) return true;

    const PatternElement& elem = elements[elem_idx];

    // anchors
    if (elem.type == PatternElement::START_ANCHOR) {
        if (pos != 0) return false;
        return match_at_position(input, pos, elements, elem_idx + 1);
    }
    if (elem.type == PatternElement::END_ANCHOR) {
        return pos == input.length();
    }

    // quantifiers
    if (is_plus_type(elem.type)) {
        if (pos >= input.length() || !matches_element(input[pos], elem)) return false;
        size_t match_end = pos + 1;
        while (match_end < input.length() && matches_element(input[match_end], elem)) {
            match_end++;
        }
        for (size_t end = match_end; end > pos; end--) {
            if (match_at_position(input, end, elements, elem_idx + 1)) return true;
        }
        return false;
    }

    if (is_opt_type(elem.type)) {
        // Option 1: take zero occurrence
        if (match_at_position(input, pos, elements, elem_idx + 1)) return true;
        // Option 2: take one occurrence (only if a char is available and matches)
        if (pos < input.length() && matches_element(input[pos], elem)) {
            return match_at_position(input, pos + 1, elements, elem_idx + 1);
        }
        return false;
    }

    // single element
    if (pos < input.length() && matches_element(input[pos], elem)) {
        return match_at_position(input, pos + 1, elements, elem_idx + 1);
    }
    return false;
}

bool match_pattern(const string &input_line, const string &pattern) {
    vector<PatternElement> elements = parse_pattern(pattern);

    if (!elements.empty() && elements[0].type == PatternElement::START_ANCHOR) {
        return match_at_position(input_line, 0, elements, 0);
    } else {
        // allow i == length to enable patterns that can match at end (e.g., "$", "a?$")
        for (size_t i = 0; i <= input_line.length(); i++) {
            if (match_at_position(input_line, i, elements, 0)) return true;
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
        return match_pattern(input_line, pattern) ? 0 : 1;
    } catch (const runtime_error& e) {
        cerr << e.what() << endl;
        return 1;
    }
}
