#include <iomanip>
#include "rds_decoder.hpp"

class Args {
private:
    std::map<std::string, char *> cached_args;
    char **argv;
    int argc;

    /**
     * @brief Returns the alternative frequencies as a vector of doubles.
     * Caches the result so subsequent lookups are faster.
     */
    std::vector<double> _get_alternative_frequencies() {
        std::vector<double> frequencies;
        const char *arg_value = this->_get_arg("-af", "--alternative-frequencies");

        if (arg_value == nullptr) {
            throw std::invalid_argument("Alternative frequencies are not specified. Option: -af, --alternative-frequencies. Group type: 0A");
        }

        // NOTE: Separated this way, because we don't want to modify the original string
        std::string input_str(arg_value);  // Create a copy of the input string
        std::stringstream ss(input_str);   // Use stringstream for splitting
        std::string token;

        // Split the string by ',' without modifying the original string
        while (std::getline(ss, token, ',')) {
            try {
                frequencies.push_back(std::stod(token));  // Convert and add to the vector
            } catch (const std::invalid_argument &) {
                throw std::invalid_argument("Invalid frequency value: " + token);
            }
        }

//        for (const auto &frequency: frequencies) {
//            DEBUG_PRINT_LITE("%f\n", frequency);
//        }

        return frequencies;
    }


    /**
     * @brief Returns the argument value corresponding to the given short or long option.
     * Caches the result so subsequent lookups are faster.
     */
    char *_get_arg(const std::string &short_option, const std::string &long_option) {
        // Check if the argument is already cached
        std::string key = short_option.empty() ? long_option : short_option;
        if (cached_args.find(key) != cached_args.end()) {
//            DEBUG_PRINT_LITE("Get cached value: %s key %s\n", cached_args[key], key.c_str());
            return cached_args[key]; // Return cached value
        }

        // If no cached value, search for it in argv
        for (int i = 0; i < argc; i++) {
            if (!short_option.empty() && strcmp(argv[i], short_option.c_str()) == 0) {
//                DEBUG_PRINT_LITE("Found value: %s\n", argv[i + 1]);
                cached_args[short_option] = argv[i + 1]; // Cache the result
//                DEBUG_PRINT_LITE("Cached value: %s\n", cached_args[short_option]);
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

    bool get_help() {
        return this->_is_defined("-h", "--help");
    }

    std::string get_data() {
        const char *arg_value = this->_get_arg("-b", "--binary-data");

        if (arg_value == nullptr) {
            throw std::invalid_argument("Binary data is not specified. Option: -b, --binary-data");
        }

        const auto data = std::string(arg_value);

        if (data.empty()) {
            throw std::invalid_argument("Binary data is empty. Option: -b, --binary-data");
        }

        if (data.size() % SIZE_2A && data.size() % SIZE_0A) {
            throw std::invalid_argument("Invalid binary data size: " + std::to_string(data.size()) + ". Expected: " + std::to_string(SIZE_2A) + " or " + std::to_string(SIZE_0A));
        }

        return data;
    }

    void print_usage() {
        std::cout << "Usage: rds_decoder [options]" << std::endl;
        std::cout << "Options:" << std::endl;
        std::cout << "  -h, --help\t\t\tShow this help message and exit" << std::endl;
        std::cout << "  -b, --binary-data\t\tThe binary data to decode" << std::endl;
    }
};

class Block {
public:
    std::bitset<16> data_A;
    std::bitset<16> data_B;
    std::bitset<16> data_C;
    std::bitset<16> data_D;

    std::bitset<10> crc_A;
    std::bitset<10> crc_B;
    std::bitset<10> crc_C;
    std::bitset<10> crc_D;


    Block(
            // Data
            std::bitset<16> block_A,
            std::bitset<16> block_B,
            std::bitset<16> block_C,
            std::bitset<16> block_D,
            // CRC
            std::bitset<10> crc_A,
            std::bitset<10> crc_B,
            std::bitset<10> crc_C,
            std::bitset<10> crc_D
    ) :     // Data
            data_A(block_A),
            data_B(block_B),
            data_C(block_C),
            data_D(block_D),
            // CRC
            crc_A(crc_A),
            crc_B(crc_B),
            crc_C(crc_C),
            crc_D(crc_D) {}
};

class CRC {
public:
};

/**
 * @brief Class that holds global variables for the whole program
 */
class Program {
private:
    auto _convert_to_bitset_size_0A(const std::string binary_data) {
        return std::bitset<SIZE_0A>(binary_data);
    }

    auto _convert_to_bitset_size_2A(const std::string binary_data) {
        return std::bitset<SIZE_2A>(binary_data);
    }

    bool _is_group_type_0A(const std::string data) {
        const auto is = data.size() / SIZE_0A == 1;
        return is;
    }

    bool _is_group_type_2A(const std::string data) {
        const auto is = data.size() / SIZE_2A == 1;
        return is;
    }

    template<std::size_t N>
    auto _get_blocks(const std::bitset<N> data) {
        std::vector<Block> blocks;
//        print_packet(data);

        // Parse
        for (int i = 0; i < BLOCKS_COUNT_IN_0A; i += 1) {
//            DEBUG_PRINT_LITE("i: %d%c", i, '\n');
            // Extract blocks in reverse order (D to A)
            // Block A
            const int block_A_start = i * BLOCK_ROW_SIZE*BLOCKS_COUNT_IN_0A;
            std::bitset<DATA_BITS> block_A = std::bitset<DATA_BITS>(data.to_string().substr(block_A_start, DATA_BITS));
            std::bitset<CRC_BITS> crc_A = std::bitset<CRC_BITS>(data.to_string().substr(block_A_start + DATA_BITS, CRC_BITS));

            // Block B
            const int block_B_start = block_A_start + BLOCK_ROW_SIZE;
            std::bitset<DATA_BITS> block_B = std::bitset<DATA_BITS>(data.to_string().substr(block_B_start, DATA_BITS));
            std::bitset<CRC_BITS> crc_B = std::bitset<CRC_BITS>(data.to_string().substr(block_B_start + DATA_BITS, CRC_BITS));

            // Block C
            const int block_C_start = block_B_start + BLOCK_ROW_SIZE;
            std::bitset<DATA_BITS> block_C = std::bitset<DATA_BITS>(data.to_string().substr(block_C_start, DATA_BITS));
            std::bitset<CRC_BITS> crc_C = std::bitset<CRC_BITS>(data.to_string().substr(block_C_start + DATA_BITS, CRC_BITS));

            // Block D
            const int block_D_start = block_C_start + BLOCK_ROW_SIZE;
            std::bitset<DATA_BITS> block_D = std::bitset<DATA_BITS>(data.to_string().substr(block_D_start, DATA_BITS));
            std::bitset<CRC_BITS> crc_D = std::bitset<CRC_BITS>(data.to_string().substr(block_D_start + DATA_BITS, CRC_BITS));

            // Add to the vectors
            blocks.emplace_back(block_A, block_B, block_C, block_D, crc_A, crc_B, crc_C, crc_D);

//            DEBUG_PRINT_LITE("Block A: %s | %s%c", block_A.to_string().c_str(), crc_A.to_string().c_str(), '\n');
//            DEBUG_PRINT_LITE("Block B: %s | %s%c", block_B.to_string().c_str(), crc_B.to_string().c_str(), '\n');
//            DEBUG_PRINT_LITE("Block C: %s | %s%c", block_C.to_string().c_str(), crc_C.to_string().c_str(), '\n');
//            DEBUG_PRINT_LITE("Block D: %s | %s%c", block_D.to_string().c_str(), crc_D.to_string().c_str(), '\n');
        }
        return blocks;
    }

public:
    Args *args;

    Program(Args *args) : args(args) {
    }

    ~Program() {
        delete args;
    }

    void decode_0A(const std::bitset<SIZE_0A> data) {
        const auto blocks = this->_get_blocks(data);

        DEBUG_PRINT_LITE("Block count: %d%c", blocks.size(), '\n');
        DEBUG_PRINT_LITE("Data: %s%c", data.to_string().c_str(), '\n');

        //////////////////////////
        /// Decode Block A (Program Identifier - PI)
        //////////////////////////
        uint16_t program_id = static_cast<uint16_t>(blocks[0].data_A.to_ulong());
        std::cout << "PI: " << program_id << std::endl;

        //////////////////////////
        /// Decode Block B
        //////////////////////////
        uint16_t block_B = static_cast<uint16_t>(blocks[0].data_B.to_ulong());

        // Group Type (GT) is 0A for this task
        std::cout << "GT: 0A" << std::endl;

        // Traffic Program (TP)
        uint8_t tp = (block_B >> 10) & 0x1;
        std::cout << "TP: " << (tp == 1 ? "1" : "0") << std::endl;

        // Program Type (PTY)
        uint8_t pty = (block_B >> 5) & 0x1F;
        std::cout << "PTY: " << static_cast<int>(pty) << std::endl;

        // Traffic Announcement (TA)
        uint8_t ta = (block_B >> 4) & 0x1;
        std::cout << "TA: " << (ta == 1 ? "Active" : "Inactive") << std::endl;

        // Music/Speech (MS)
        uint8_t ms = (block_B >> 3) & 0x1;
        std::cout << "MS: " << (ms == 1 ? "Music" : "Speech") << std::endl;

        // Decoder Identifier (DI)
        uint8_t di = (block_B >> 2) & 0x1;
        std::cout << "DI: " << static_cast<int>(di) << std::endl;

        //////////////////////////
        /// Decode Block C (Alternative Frequencies - AF)
        //////////////////////////
        uint16_t block_C = static_cast<uint16_t>(blocks[0].data_C.to_ulong());
        uint8_t af1 = (block_C >> 8) & 0xFF;
        uint8_t af2 = block_C & 0xFF;

        // Convert AF to MHz
        double frequency1 = af1 / 10.0 + FREQUENCY_START;  // Assuming FREQUENCY_START is defined
        double frequency2 = af2 / 10.0 + FREQUENCY_START;
        // Print AF always with 1 decimal place
        std::cout << "AF: " << std::fixed << std::setprecision(1) << frequency1 << ", " << frequency2 << std::endl;

        //////////////////////////
        /// Decode Block D (Program Service - PS)
        //////////////////////////
//        print_packet(data);
        const auto buffer = new std::stringstream();
        for (int i = 0; i < blocks.size(); ++i) {
//            DEBUG_PRINT_LITE("i: %d\n", i);
//            DEBUG_PRINT_LITE("Block D: %s\n", blocks[i].data_D.to_string().c_str());

            // Extract data
            uint16_t radio_data = static_cast<uint16_t>(blocks[i].data_D.to_ulong());

            // Extract PS
            uint8_t ps1 = (radio_data >> 8) & 0xFF;
            uint8_t ps2 = radio_data & 0xFF;

            // Append to the buffer
            *buffer << static_cast<char>(ps1) << static_cast<char>(ps2);
        }

        std::cout << "PS: " << buffer->str() << std::endl;

        // Note: Full PS can be obtained by concatenating all segments from Block D over time
    }

    void decode_2A(const std::bitset<SIZE_2A> data) {
        // TODO
    }

    void decode() {
        DEBUG_PRINT_LITE("Decoding START%c", '\n');
        if (_is_group_type_0A(args->get_data())) {
            const auto converted_binary_data = _convert_to_bitset_size_0A(args->get_data());
            decode_0A(converted_binary_data);
        }
        if (_is_group_type_2A(args->get_data())) {
            const auto converted_binary_data = _convert_to_bitset_size_2A(args->get_data());
            decode_2A(converted_binary_data);
        }
        DEBUG_PRINT_LITE("Decoding DONE%c", '\n');
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
        program->decode();
    } catch (const std::invalid_argument &e) {
        program->exit_with_code(1, e.what());
    } catch (const std::exception &e) {
        program->exit_with_code(1, e.what());
    }

    // Exit with success code and no message
    program->exit_with_code(0);
}
