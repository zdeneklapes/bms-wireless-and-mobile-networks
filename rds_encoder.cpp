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
                    DEBUG_PRINT("Group type: %s\n", optarg);
//                    group_type = std::string(optarg);
                    break;
                case 'p':
                    DEBUG_PRINT("Program ID: %s\n", optarg);
//                    program_id = static_cast<uint16_t>(std::stoi(optarg));
                    break;
                case 't':
                    DEBUG_PRINT("Traffic program: %s\n", optarg);
//                    program_type = static_cast<uint8_t>(std::stoi(optarg));
                    break;
                case 'r':
                    DEBUG_PRINT("Traffic program: %s\n", optarg);
//                    radio_text = std::string(optarg);
                    break;
                case 'a':
                    DEBUG_PRINT("Traffic program: %s\n", optarg);
//                    alternative_freq_1 = std::stof(optarg);
                    break;
                case 'f':
                    DEBUG_PRINT("Traffic program: %s\n", optarg);
                    // handle alternative frequencies properly, e.g., split into two values
                    break;
                case 's':
                    DEBUG_PRINT("Traffic program: %s\n", optarg);
//                    program_service = std::string(optarg);
                    break;
                case 'm':
                    DEBUG_PRINT("Traffic program: %s\n", optarg);
//                    music_speech = std::stoi(optarg);
                    break;
                case 'T':
                    DEBUG_PRINT("Traffic program: %s\n", optarg);
//                    traffic_announcement = std::stoi(optarg);
                    break;
                case 'A':
                    DEBUG_PRINT("Traffic program: %s\n", optarg);
//                    ab_flag = std::stoi(optarg);
                    break;
                default:
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

void usage() {
    std::cout << "Usage: rds_encoder [options]" << std::endl;
    std::cout << "Options:" << std::endl;
    std::cout << "  -g <group type>        Group type (0A, 2A)" << std::endl;
    std::cout << "  -p <program id>        Program ID" << std::endl;
    std::cout << "  -t <program type>      Program type" << std::endl;
    std::cout << "  -r <radio text>        Radio text" << std::endl;
    std::cout << "  -a <alternative freq>  Alternative frequency" << std::endl;
    std::cout << "  -f <alternative freq>  Alternative frequency" << std::endl;
    std::cout << "  -s <program service>   Program service" << std::endl;
    std::cout << "  -m <music/speech>      Music/speech" << std::endl;
    std::cout << "  -T <traffic announcement> Traffic announcement" << std::endl;
    std::cout << "  -A <AB flag>           AB flag" << std::endl;
}

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
        const bool print_usage = false,
        const std::string &message = ""
) {
    // Print message to stderr if code is not 0 and message is not empty
    if (code != 0 && !message.empty()) {
        std::cerr << message << std::endl;
    }

    if (print_usage) {
        usage();
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
