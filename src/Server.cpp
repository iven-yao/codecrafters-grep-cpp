#include <iostream>
#include <string>
bool match_pattern(const std::string& text, const std::string& pattern);

bool match_group(char c, const std::string group, bool neg) {
    bool match = group.find(c) != std::string::npos;
    return neg ? !match : match;
}

bool match_group_helper(const std::string& text, const std::string& pattern) {
    if(text.empty()) return false;

    bool neg = pattern.at(0) == '^';
    size_t end_pos = pattern.find_first_of(']');
    if(end_pos == std::string::npos) {
        throw std::runtime_error("Invalid pattern: close bracket not found");
    }

    size_t start_pos = neg? 1 : 0;
    bool match = match_group(text.at(0), pattern.substr(start_pos, end_pos-start_pos), neg);

    return match && match_pattern(text.substr(1), pattern.substr(end_pos+1));
}

bool match_special_char(char special, char c) {
    if(special == 'd') {
        return isdigit(c);
    }
    else if(special == 'w') {
        return isalnum(c);
    }
    else {
        throw std::runtime_error("Unhandled pattern \\" + special);
    }
}

bool match_pattern(const std::string& text, const std::string& pattern) {
    if(pattern.empty()) {
        return true;
    }

    if(pattern.at(0) == '^') {
        return match_pattern(text.substr(0, pattern.length() - 1), pattern.substr(1));
    }

    if(pattern.at(pattern.length() - 1) == '$') {
        return match_pattern(text.substr(text.length() - pattern.length() + 1), pattern.substr(0, pattern.length() - 1));
    }

    for(size_t i = 0; i < text.length(); i++) {
        if(pattern.at(0) == '\\') {
            if(pattern.length() < 2) {
                throw std::runtime_error("Invalid pattern: missing element after escape(\\)");
            }

            if(match_special_char(pattern.at(1), text.at(i))) {
                return match_pattern(text.substr(i+1), pattern.substr(2));
            }
        } else if(pattern.at(0) == '[') {
            return match_group_helper(text, pattern.substr(1));
        } else {
            if(pattern[0] == text[i]) {
                return match_pattern(text.substr(i+1), pattern.substr(1));
            }
        }
    }

    return false;
        
}

int main(int argc, char* argv[]) {
    // Flush after every std::cout / std::cerr
    std::cout << std::unitbuf;
    std::cerr << std::unitbuf;

    // You can use print statements as follows for debugging, they'll be visible when running tests.
    std::cout << "Logs from your program will appear here" << std::endl;

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

    // Uncomment this block to pass the first stage
    //
    std::string input_line;
    std::getline(std::cin, input_line);
    
    try {
        if (match_pattern(input_line, pattern)) {
            std::cout << "true";
            return 0;
        } else {
            std::cout << "false";
            return 1;
        }
    } catch (const std::runtime_error& e) {
        std::cerr << e.what() << std::endl;
        return 1;
    }
}
