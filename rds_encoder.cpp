#include "rds_encoder.hpp"
#include <iostream>
#include <string>
#include <getopt.h>

#define REGEX_TEXT "[a-zA-Z0-9 ]*"
#define DEBUG 1
#define DEBUG_PRINT_LITE(fmt, ...) \
            do { if (DEBUG_LITE) fprintf(stderr, fmt, __VA_ARGS__); } while (0)
#define DEBUG_PRINT(fmt, ...) \
        do { if (DEBUG) fprintf(stderr, "%s:%d:%s(): " fmt, __FILE__, \
                                __LINE__, __func__, __VA_ARGS__); } while (0)


/**
 * @brief Class that holds arguments passed to the program
 */
class Args {
public:
    std::string group_type;
    uint16_t program_id;
    uint8_t program_type;
    bool traffic_program;
    bool music_speech; // Group 0A specific
    bool traffic_announcement; // Group 0A specific
    float alternative_freq_1; // Group 0A specific
    float alternative_freq_2; // Group 0A specific
    std::string program_service; // Group 0A specific
    std::string radio_text; // Group 2A specific
    bool ab_flag; // Group 2A specific

    Args() {
        std::cout << "Args created" << std::endl;
    }

    ~Args() {
        std::cout << "Args destroyed" << std::endl;
    }

    /**
     * @brief Parses the command line arguments
     */
    bool parse(int argc, char *argv[]) {
        int opt;
        const char *optString = "g:pi:pty:tp:ms:ta:af:ps:rt:ab:";

        while ((opt = getopt(argc, argv, optString)) != -1) {
            switch (opt) {
                case 'g':
                    group_type = std::string(optarg);
                    break;
                case 'p':
                    DEBUG_PRINT("Program ID: %d\n", program_id);
                    DEBUG_PRINT("Program ID: %s\n", optarg);
                    program_id = static_cast<uint16_t>(std::stoi(optarg));
                    break;
                case 't':
                    program_type = static_cast<uint8_t>(std::stoi(optarg));
                    break;
                case 'r':
                    radio_text = std::string(optarg);
                    break;
                case 'a':
                    alternative_freq_1 = std::stof(optarg);
                    break;
                case 'f':
                    // handle alternative frequencies properly, e.g., split into two values
                    break;
                case 's':
                    program_service = std::string(optarg);
                    break;
                case 'm':
                    music_speech = std::stoi(optarg);
                    break;
                case 'T':
                    traffic_announcement = std::stoi(optarg);
                    break;
                case 'A':
                    ab_flag = std::stoi(optarg);
                    break;
                default:
                    std::cerr << "Invalid argument" << std::endl;
                    return false;
            }
        }
        return true;
    }
};

/**
 * @brief Class that holds global variables for the whole program
 */
class Program {
public:
    Args *args;

    Program() {
        std::cout << "Program created" << std::endl;
        this->args = new Args();
    }

    ~Program() {
        std::cout << "Program destroyed" << std::endl;
        delete args;
    }
};

/**
 * @brief Exits the program with a given code and optional message
 *
 * @param program A pointer to the Program object
 * @param code The exit code
 * @param message The message to print (if any)
 */
int exit_with_code(
        Program *program,
        const int code,
        const std::string &message = ""
) {
    // Print message to stderr if code is not 0 and message is not empty
    if (code != 0 && !message.empty()) {
        std::cerr << message << std::endl;
    }

    // Print message to stdout if code is 0 and message is not empty
    if (code == 0 && !message.empty()) {
        std::cout << message << std::endl;
    }

    // Cleanup and exit
    delete program;
    exit(code);
}

int main(int argc, char *argv[]) {
    Program *program = new Program();

    if (!program->args->parse(argc, argv)) {
        exit_with_code(program, 1, "Error parsing arguments");
    }

    // Exit with success code and no message
    exit_with_code(program, 0);
}
