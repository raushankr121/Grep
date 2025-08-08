#include <iostream>
#include <string>

bool match_pattern(const std::string& input_line, const std::string& pattern) {
    return input_line.find(pattern) != std::string::npos;
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
    bool found_match = false;

    while (std::getline(std::cin, input_line)) {
        if (match_pattern(input_line, pattern)) {
            std::cout << input_line << std::endl; // grep prints matching lines
            found_match = true;
        }
    }

    return found_match ? 0 : 1;
}
