#include <iostream>
#include <string>
#include <vector>

bool match_pattern_helper(const std::string& text, const std::string& pattern, bool end_anchor);

bool match_group(const std::string& text, const std::string& pattern) {
    if(text.empty()) return false;

    bool neg = pattern[0] == '^';
    size_t start_pos = neg? 1 : 0;
    bool match = pattern.substr(start_pos).find(text[0]) != std::string::npos;
    if(neg) {
        match = !match;
    }

    return match;
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

bool match_option(const std::string& text, const std::string& pattern, bool end_anchor) {
    std::vector<std::string> v = {};
    size_t end_pos = 0;
    size_t start_pos = 0;
    int open_bracket_count = 1;
    while(open_bracket_count > 0) {
        end_pos++;
        if(open_bracket_count == 1 && pattern[end_pos] == '|') {
            v.push_back(pattern.substr(start_pos, end_pos - start_pos));
            start_pos = end_pos+1;
        }
        if(pattern[end_pos] == ')') {
            open_bracket_count--;
            v.push_back(pattern.substr(start_pos, end_pos - start_pos));
            start_pos = end_pos+1;
        }

        if(pattern[end_pos] == '(') {
            open_bracket_count++;
        }
    }

    for(std::string option: v) {
        if(match_pattern_helper(text, option+pattern.substr(start_pos), end_anchor)) return true;
    }

    return false;
}

bool match_pattern_helper(const std::string& text, const std::string& pattern, bool end_anchor) {
    // debug log
    // std::cout << text << "," << pattern << "\n";

    if(end_anchor) {
        if(pattern.empty() && text.empty()) {
            return true;
        }

        if(pattern.size() == 2 && pattern[1] == '?' && text.empty()) {
            return true;
        }

        if(text.empty()) {
            return false;
        }

        if(pattern.empty()) {
            return false;
        }
    } else {
        if(pattern.empty()){
            return true;
        }

        if(pattern.size() == 2 && pattern[1] == '?') {
            return true;
        }

        if(text.empty()) {
            return false;
        }
    }

    if(pattern[0] == '\\') {
        // match special character
        if(pattern.length() < 2) {
            throw std::runtime_error("Invalid pattern: missing element after escape(\\)");
        }

        if(match_special_char(pattern[1], text[0])) {
            return match_pattern_helper(text.substr(1), pattern.substr(2), end_anchor);
        }
    } else if(pattern[0] == '[') {
        // match in group
        size_t end_pos = pattern.find_first_of(']');
        if(end_pos == std::string::npos) {
            throw std::runtime_error("Invalid pattern: close bracket not found");
        }
        return match_group(text, pattern.substr(1, end_pos - 1)) && match_pattern_helper(text.substr(1), pattern.substr(end_pos+1), end_anchor);
    } else if(pattern[0] == '(') {
        // match either option
        return match_option(text, pattern.substr(1), end_anchor);
    } else {
        // check +
        if(pattern.length() > 1 && pattern[1] == '+') {
            if(pattern[0] == text[0]) {
                size_t tmp = 1;
                while(tmp < text.length() && text[tmp] == pattern[0]) {
                    tmp++;
                }
                return match_pattern_helper(text.substr(tmp), pattern.substr(2), end_anchor);
            }
        }
        // check ?
        else if(pattern.length() > 1 && pattern[1] == '?') {
            if(pattern[0] == text[0]) {
                return match_pattern_helper(text.substr(1), pattern.substr(2), end_anchor);
            }

            return match_pattern_helper(text, pattern.substr(2), end_anchor);
        }
        // wildcard
        else if(pattern[0] == '.') {
            return match_pattern_helper(text.substr(1), pattern.substr(1), end_anchor);
        }
        else if(pattern[0] == text[0]) {
            return match_pattern_helper(text.substr(1), pattern.substr(1), end_anchor);
        }
    }

    return false;
}

bool match_pattern(const std::string& text, const std::string& pattern) {
    if(pattern.empty()) {
        return true;
    }

    // start anchor
    if(pattern[0] == '^') {
        return match_pattern_helper(text, pattern.substr(1), false);
    }

    // end anchor
    if(pattern[pattern.length() - 1] == '$') {
        for(size_t i = 0; i < text.length(); i++) {
            if(match_pattern_helper(text.substr(i), pattern.substr(0, pattern.size() - 1), true)) {
                return true;
            }
        }

        return false;
    }

    // regular match from any index
    for(size_t i = 0; i < text.length(); i++) {
        if(match_pattern_helper(text.substr(i), pattern, false)) {
            return true;
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
