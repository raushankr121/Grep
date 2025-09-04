#include <iostream>
#include <string>

bool match_pattern(const std::string& input_line, const std::string& pattern) {

    if (pattern.length() == 1) {

        return input_line.find(pattern) != std::string::npos;

    } else if (pattern == "\\d") {

        return input_line.find_first_of("0123456789") != std::string::npos;

    } else if (pattern == "\\w") {

        return input_line.find_first_of("0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ_") != std::string::npos;

    } else if (pattern.front() == '[' && pattern.back() == ']') {

        if (pattern.at(1) == '^') return input_line.find_first_not_of(pattern.substr(2, pattern.length() - 3)) != std::string::npos;
        return input_line.find_first_of(pattern.substr(1, pattern.length() - 2)) != std::string::npos;

    } else {

        throw std::runtime_error("Unhandled pattern " + pattern);

    }

}


bool match_here(const std::string& input_line, const std::string& pattern){

        /*
    if (regexp[0] == '\0') 
        // std::cout << "End of regexp, return true" << std::endl;
        // End of regexp, must have been found
        return 1;
    if (regexp[0] == '\\' && regexp[1] == 'd' && (*input_line >= '0' && *input_line <= '9'))

        // Any digit, continue search if true
        return match_here(input_line + 1, regexp + 2);

    if (regexp[0] == '\\' && regexp[1] == 'w' && ((*input_line >= 'a' && *input_line <= 'z') || (*input_line >= 'A' && *input_line <= 'Z') || (*input_line == '_') ))

        // Any literal, continue search if true
        return match_here(input_line + 1, regexp + 2);

    if (regexp[0] == *input_line)

        // Specific char
        return match_here(input_line + 1, regexp + 1);

    if (input_line[0] == '\0')

        return 0;

    // Char in regexp hasn't been found
    return 0;
    */

    if (pattern.length() == 0) 

        // found, if remaining pattern is empty
        return 1;

    if (pattern.at(0) == '$' && pattern.length() == 1) {

        // found, if end of string anchor in pattern and actual end of string is reached
        return (input_line.length() == 0);
    }

    if (pattern.length() > 1) {
        if (pattern.at(1) == '?') {

            if (pattern.length() == 2 && input_line.length() == 0){

                return 1;

            } else if (pattern.at(0) == input_line.at(0)) {

                return match_here(input_line.substr(1), pattern.substr(2));

            } else {

                return match_here(input_line.substr(0), pattern.substr(2));

            }
        }
    }

    if (input_line.length() == 0)

        // end of string reached
        return 0;

    if (pattern.at(0) == '\\' && pattern.at(1) == 'd' && (input_line.substr(0,1).find_first_of("0123456789") != std::string::npos))

        // digit match
        return match_here(input_line.substr(1), pattern.substr(2)); 
    
    if (pattern.at(0) == '\\' && pattern.at(1) == 'w' && (input_line.substr(0,1).find_first_of("0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ_") != std::string::npos))

        // literal match
        return match_here(input_line.substr(1), pattern.substr(2)); 


    if (pattern.at(0) == input_line.at(0) || pattern.at(0) == '.'){

        
        if (pattern.length() > 1){

            
            if (pattern.at(1) == '+') {
                // std::cout << "Found +: " << pattern.at(1) << std::endl;
                int i = 0;
                do {
                    if (match_here(input_line.substr(i), pattern.substr(2))) return 1;
                } while (i < input_line.length() && (input_line.at(i++) == pattern.at(0) || pattern.at(0) == '.'));
                return 0;
            } 
        }

        // char match
        return match_here(input_line.substr(1), pattern.substr(1));
    }

    if (pattern.at(0) == '[') {

        int end_pos = pattern.find(']');

        if (pattern.at(1) == '^') {
            if (input_line.find_first_not_of(pattern.substr(2, end_pos - 2)) != std::string::npos) 
            // return match_here(input_line.substr(end_pos - 2), pattern.substr(end_pos + 1));
            return match_here(input_line, pattern.substr(end_pos + 1));
        } else {

            if (input_line.find_first_of(pattern.substr(1, end_pos - 1)) != std::string::npos)
            // return match_here(input_line.substr(end_pos - 1), pattern.substr(end_pos + 1));
            return match_here(input_line, pattern.substr(end_pos + 1));
        }
    }

    return 0;
}

bool match(const std::string& input_line, const std::string& pattern){

    /*
    // Iterate through input line
    do {
        if(match_here(input_line, regexp)) return 1;
    } while (*input_line++ != '\0');

    return 0;
    */

    int pos = 0;

    if (pattern.at(0) == '^') {
        return match_here(input_line, pattern.substr(1));
    }

    do {
        if (match_here(input_line.substr(pos), pattern)) return 1;
    } while (pos++ < input_line.length());

    return 0;

}


int main(int argc, char* argv[]) {
    // Flush after every std::cout / std::cerr
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
        // if (match_pattern(input_line, pattern)) {
        if (match(input_line.data(), pattern.data())) {
            return 0;
        } else {
            return 1;
        }
    } catch (const std::runtime_error& e) {
        std::cerr << e.what() << std::endl;
        return 1;
    }
}
