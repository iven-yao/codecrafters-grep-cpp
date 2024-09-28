#include <iostream>
#include <string>

bool is_alpha_numeric(char c) {
    return isalpha(c) || isdigit(c);
}

bool is_in_group(char c, const std::string group) {
    for(char ele: group) {
        if(c == ele) return true;
    }

    return false;
}

bool match_pattern(const std::string& input_line, const std::string& pattern) {
    int i = 0;
    while( i < input_line.length()) {

        int j = 0;
        int start = i;
        // start matching the pattern
        while(j < pattern.length() && start < input_line.length()) {
            bool match = false;
            if(pattern.at(j) == '\\') {
                // match special characters
                j++;
                if(pattern.at(j) == 'd') {
                    match = isdigit(input_line.at(start));
                }
                else if(pattern.at(j) == 'w') {
                    match = is_alpha_numeric(input_line.at(start));
                }
                else {
                    throw std::runtime_error("Unhandled pattern " + pattern + " at index " + std::to_string(j));
                }
            } else if(pattern.at(j) == '[') {
                // match group
                j++;
                bool positive = true;
                if(pattern.at(j) == '^') {
                    positive = false;
                    j++;
                }
                int group_start = j;
                int group_size = 0;
                while(j < pattern.length() && pattern.at(j) != ']') {
                    j++;
                    group_size++;
                }

                std::string group = pattern.substr(group_start, group_size);
                if(positive) {
                    match = is_in_group(input_line.at(start), group);
                } else {
                    match = !is_in_group(input_line.at(start), group);
                }
            } else {
                // match specific char
                match = (input_line.at(start) == pattern.at(j));
            }

            if(match) {
                start++;
                j++;
            } else {
                break;
            }
        }
        // if match to the end of the pattern, return true
        if(j == pattern.length()) {
            return true;
        }
        // otherwise, start matching from next possible index
        i++;
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
