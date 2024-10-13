#include "rds_encoder.hpp"
#include <iostream>
#include <map>
#include <string>
#include <cstring>
#include <stdexcept>
#include <vector>

#define REGEX_TEXT "[a-zA-Z0-9 ]*"
#define DEBUG 1
#define DEBUG_LITE 1
#define DEBUG_PRINT_LITE(fmt, ...) \
            do { if (DEBUG_LITE) fprintf(stderr, fmt, __VA_ARGS__); } while (0)
#define DEBUG_PRINT(fmt, ...) \
        do { if (DEBUG) fprintf(stderr, "%s:%d:%s(): " fmt, __FILE__, \
                                __LINE__, __func__, __VA_ARGS__); } while (0)


class Args {
private:
    std::map<std::string, char *> cached_args;
    char **argv;
    int argc;

    /**
     * @brief Returns the argument value corresponding to the given short or long option.
     * Caches the result so subsequent lookups are faster.
     */
    char *_get_arg(const std::string &short_option, const std::string &long_option) {
        // Check if the argument is already cached
        std::string key = short_option.empty() ? long_option : short_option;
        if (cached_args.find(key) != cached_args.end()) {
            return cached_args[key]; // Return cached value
        }

        // If no cached value, search for it in argv
        for (int i = 0; i < argc; i++) {
            if (!short_option.empty() && strcmp(argv[i], short_option.c_str()) == 0) {
                cached_args[short_option] = argv[i + 1]; // Cache the result
                return argv[i + 1];
            } else if (!long_option.empty() && strcmp(argv[i], long_option.c_str()) == 0) {
                cached_args[long_option] = argv[i + 1]; // Cache the result
                return argv[i + 1];
            }
        }

        return nullptr; // If not found
    }

    /**
     * @brief Checks if a given option is defined.
     * Caches the result.
     */
    bool _is_defined(const std::string &short_option, const std::string &long_option) {
        // Check if the result is already cached
        std::string key = short_option.empty() ? long_option : short_option;
        if (cached_args.find(key) != cached_args.end()) {
            return true; // It was already defined
        }

        // Search argv for the option
        for (int i = 0; i < argc; i++) {
            if (!short_option.empty() && strcmp(argv[i], short_option.c_str()) == 0) {
                cached_args[short_option] = argv[i]; // Cache it
                return true;
            } else if (!long_option.empty() && strcmp(argv[i], long_option.c_str()) == 0) {
                cached_args[long_option] = argv[i]; // Cache it
                return true;
            }
        }

        return false; // Not defined
    }

    bool _is_same(const char *arg, const char *value) {
        return std::strcmp(arg, value) == 0;
    }


public:
    Args(char **argv, int argc) : argv(argv), argc(argc) {}

    ~Args() = default;

    enum class GroupType {
        A0,
        A2
    };

    /**
     * Program Identification (16-bit unsigned integer).
     */
    short int get_program_identifier() {
        const char *program_id = this->_get_arg("-pi", "--program-id");
        if (program_id == nullptr) {
            throw std::invalid_argument("Program ID is not specified.");
        }
        return static_cast<short int>(std::stoi(program_id));
    }

    /**
     * Common
     * Help flag
     */
    bool get_help() {
        return this->_is_defined("-h", "--help");
    }

    /**
     * Common
     * -gt
     * Group Type (5-bit unsigned integer, values 0-31).
     */
    GroupType get_group_type() {
        const char *group_type = this->_get_arg("-g", "--group-type");
        if (group_type == nullptr) {
            throw std::invalid_argument("Group type is not specified.");
        }
        if (this->_is_same(group_type, "0A")) {
            return GroupType::A0;
        } else if (this->_is_same(group_type, "2A")) {
            return GroupType::A2;
        }
        throw std::invalid_argument("Group type must be 0A or 2A. Option: -gt, --group-type");
    }


    /**
     * Common
     * -pty
     * Program Type (5-bit unsigned integer, values 0-31).
     */
    char get_program_type() {
        const char *program_type = this->_get_arg("-pty", "--program-type");
        if (program_type == nullptr) {
            throw std::invalid_argument("Program type is not specified.");
        }
        const auto type = static_cast<short int>(std::stoi(program_type));
        return static_cast<char>(type);
    }

    /**
     * Common
     * -tp
     * Traffic Program (1-bit boolean).
     *
     * @return bool
     *
     * @throws std::invalid_argument
     */
    bool get_traffic_program() {
        const char *traffic_program = this->_get_arg("-tp", "--traffic-program");
        if (traffic_program == nullptr) {
            throw std::invalid_argument("Traffic program is not specified.");
        }
        if (std::strcmp(traffic_program, "1") == 0) {
            return true;
        } else if (std::strcmp(traffic_program, "0") == 0) {
            return false;
        }
        throw std::invalid_argument("Traffic program must be 0 or 1. Option: -tp, --traffic-program");
    }

    /**
     * Group type: 0A
     * -ms = 1
     * Alternative Frequency (1-bit boolean).
     *
     * @return bool
     *
     * @throws std::invalid_argument
     */
    bool is_music() {
        const char *arg_value = this->_get_arg("-ms", "--music-speech");
        if (arg_value == nullptr) {
            throw std::invalid_argument("Music/Speech is not specified. Option: -ms, --music-speech. Group type: 0A");
        }
        return this->_is_same(arg_value, "1");
    }


    /**
     * Group type: 0A
     * -ms = 0
     * Music/Speech (1-bit boolean).
     *
     * @return bool
     *
     * @throws std::invalid_argument
     */
    bool is_speech() {
        const char *arg_value = this->_get_arg("-ms", "--music-speech");
        if (arg_value == nullptr) {
            throw std::invalid_argument("Music/Speech is not specified. Option: -ms, --music-speech. Group type: 0A");
        }
        return this->_is_same(arg_value, "0");
    }

    /**
     * Group type: 0A
     * -ta
     * Traffic Announcement (1-bit boolean).
     *
     * @return bool
     *
     * @throws std::invalid_argument
     */
    bool get_traffic_announcement() {
        const char *traffic_announcement = this->_get_arg("-ta", "--traffic-announcement");
        if (traffic_announcement == nullptr) {
            throw std::invalid_argument("Traffic announcement is not specified. Option: -ta, --traffic-announcement. Group type: 0A");
        }
        return this->_is_same(traffic_announcement, "1");
    }

    /**
     * Group type: 0A
     * -af
     * Alternative Frequencies (12-bit unsigned integer).
     *
     * @return std::vector<double>
     *
     * @throws std::invalid_argument
     */
    std::vector<double> get_alternative_frequencies() {
        std::vector<double> frequencies;
        char *arg_value = this->_get_arg("-af", "--alternative-frequencies");
        if (arg_value == nullptr) {
            throw std::invalid_argument("Alternative frequencies are not specified. Option: -af, --alternative-frequencies. Group type: 0A");
        }
        char *token = std::strtok(arg_value, ",");
        while (token != nullptr) {
            frequencies.push_back(std::stod(token));
            token = std::strtok(nullptr, ",");
        }
        return frequencies;
    }

    /**
     * Group type: 0A
     * -ps
     * Program Service (8-bit unsigned integer).
     *
     * @return const char *
     *
     * @throws std::invalid_argument
     */
    const char *get_program_service() {
        const char *program_service = this->_get_arg("-ps", "--program-service");
        if (program_service == nullptr) {
            throw std::invalid_argument("Program service is not specified. Option: -ps, --program-service. Group type: 0A");
        }
        return program_service;
    }

    /**
     * Group type: 2A
     * -ab
     * AB Flag (1-bit boolean).
     *
     * @return bool
     *
     * @throws std::invalid_argument
     */
    const char *get_radio_text() {
        const char *radio_text = this->_get_arg("-rt", "--radio-text");
        if (radio_text == nullptr) {
            throw std::invalid_argument("Radio text is not specified. Option: -rt, --radio-text. Group type: 2A");
        }
        return radio_text;
    }

    /**
     * Group type: 2A
     * -ab
     * AB Flag (1-bit boolean).
     *
     * @return bool
     *
     * @throws std::invalid_argument
     */
    bool get_radio_text_ab_flag() {
        const char *radio_text_ab_flag = this->_get_arg("-ab", "--ab-flag");
        if (radio_text_ab_flag == nullptr) {
            throw std::invalid_argument("AB flag is not specified. Option: -ab, --ab-flag. Group type: 2A");
        }
        return this->_is_same(radio_text_ab_flag, "1");
    }

    void print_usage() {
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
};

/**
 * @brief Class that holds global variables for the whole program
 */
class Program {
private:
    // TODO: Implement encoder logic here

public:
    Args *args;

    Program(Args *args) : args(args) {
    }

    ~Program() {
        delete args;
    }

    void process_0A() {

    }

    void process_2A() {

    }

    int exit_with_code(const int code, const std::string &message = "") {
        // Print message to stderr if code is not 0 and message is not empty
        if (code != 0 && !message.empty()) {
            std::cerr << message << std::endl;
        }

        // Print message to stdout if code is 0 and message is not empty
        if (code == 0 && !message.empty()) {
            std::cout << message << std::endl;
        }

        delete this;

        // Cleanup and exit
        exit(code);
    }
};

/**
 * @brief Exits the program with a given code and optional message
 *
 * @param program A pointer to the Program object
 * @param code The exit code
 * @param message The message to print (if any)
 */
int main(int argc, char *argv[]) {
    auto *program = new Program(new Args(argv, argc));

    if (program->args->get_help()) {
        program->args->print_usage();
        program->exit_with_code(0);
    }

    try {
        const auto group_type = program->args->get_group_type();
        DEBUG_PRINT_LITE("Group Type: %d\n", group_type);

        if (group_type == Args::GroupType::A0) {
            program->process_0A();
        }

        if (group_type == Args::GroupType::A2) {
            program->process_2A();
        }
//        const auto type = program->args->get_program_type();
//        DEBUG_PRINT_LITE("Program Type: %d\n", type);
//        const auto program_id = program->args->get_program_identifier();
//        DEBUG_PRINT_LITE("Program ID: %d\n", program_id);
//        const auto traffic_program = program->args->get_traffic_program();
//        DEBUG_PRINT_LITE("Traffic Program: %d\n", traffic_program);
//        const auto music = program->args->is_music();
//        DEBUG_PRINT_LITE("Music: %d\n", music);
//        const auto speech = program->args->is_speech();
//        DEBUG_PRINT_LITE("Speech: %d\n", speech);
//        const auto alternative_frequencies = program->args->get_alternative_frequencies();
//        for (const auto &frequency: alternative_frequencies) {
//            DEBUG_PRINT_LITE("Alternative Frequency: %f\n", frequency);
//        }
//        const auto traffic_announcement = program->args->get_traffic_announcement();
//        DEBUG_PRINT_LITE("Traffic Announcement: %d\n", traffic_announcement);
//        const auto program_service = program->args->get_program_service();
//        DEBUG_PRINT_LITE("Program Service: %s\n", program_service);
//        const auto radio_text = program->args->get_radio_text();
//        DEBUG_PRINT_LITE("Radio Text: %s\n", radio_text);
//        const auto radio_text_ab_flag = program->args->get_radio_text_ab_flag();
//        DEBUG_PRINT_LITE("Radio Text AB Flag: %d\n", radio_text_ab_flag);
    } catch (const std::invalid_argument &e) {
        program->exit_with_code(1, e.what());
    } catch (const std::exception &e) {
        program->exit_with_code(1, e.what());
    }

    // Exit with success code and no message
    program->exit_with_code(0);
}
