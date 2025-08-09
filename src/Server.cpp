#include <iostream>
#include <string>
#include <cctype>

bool match_pattern(const std::string& input_line, const std::string& pattern) {
    if (pattern.length() == 1) {
        return input_line.find(pattern) != std::string::npos;
    }
    else if (pattern == "\\d") { // match digits
        return input_line.find_first_of("0123456789") != std::string::npos;
    }else if(pattern == "\\w"){
        return input_line.find_first_of("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789") != std::string::npos;
    }
    else if (pattern == "\\s") { // match whitespace
        for (const auto& l : input_line) {
            if (std::isspace(static_cast<unsigned char>(l))) {
                return true;
            }
        }
        return false;
    }
    else if (pattern == "\\w") { // match alphanumeric
        for (const auto& l : input_line) {
            if (std::isdigit(static_cast<unsigned char>(l)) || std::isalpha(static_cast<unsigned char>(l))) {
                return true;
            }
        }
        return false;
    }
    else if (*pattern.begin() == '[' && *(pattern.end() - 1) == ']') {
        const std::string group = pattern.substr(1, pattern.length() - 2);
        const bool isNegativeGroup = group[0] == '^';

        if (isNegativeGroup) {
            for (const auto& l : group.substr(1)) {
                if (input_line.find(l) != std::string::npos) {
                    return false;
                }
            }
            return true;
        }

        for (const auto& l : group) {
            if (input_line.find(l) != std::string::npos) {
                return true;
            }
        }
        return false;
    }
    else {
        // Default: substring match for any other pattern
        return input_line.find(pattern) != std::string::npos;
    }
}

int main(int argc, char* argv[]) {
    std::cout << std::unitbuf;
    std::cerr << std::unitbuf;

    if (argc != 3) {
        std::cerr << "Expected two arguments" << std::endl;
        return 1;
    }

    std::string flag = argv[1];
    std::string pattern = argv[2];

    if (flag != "-E") {
        std::cerr << "Expected first argument to be '-E'" << std::endl;
        return 1;
    }

    std::string input_line;
    std::getline(std::cin, input_line);

    try {
        if (match_pattern(input_line, pattern)) {
            return 0; // match found
        } else {
            return 1; // no match
        }
    }
    catch (const std::runtime_error& e) {
        std::cerr << e.what() << std::endl;
        return 1;
    }
}
