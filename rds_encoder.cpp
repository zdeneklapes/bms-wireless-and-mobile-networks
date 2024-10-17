#include "rds_encoder.hpp"


// TODO: Check that group type 0A can send only text of 8 characters
// TODO: Check that all
// TODO: -ps padding to 8 chars and -rt padding to 64 chars

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
    bool get_music_speech() {
        const char *arg_value = this->_get_arg("-ms", "--music-speech");
        if (arg_value == nullptr) {
            throw std::invalid_argument("Music/Speech is not specified. Option: -ms, --music-speech. 0 for Speech, 1 for Music. Group type: 0A");
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
     * Alternative Frequency 1st (8-bit unsigned integer).
     *
     * @return std::vector<double>
     *
     * @throws std::invalid_argument
     */
    std::bitset<8> get_alternative_frequency_1() {
        std::vector<double> frequencies = this->_get_alternative_frequencies();

        // AF1: 8 - bit
//        DEBUG_PRINT_LITE("Frequency: %f, size: %d\n", frequencies[0], frequencies.size());
        auto alternative_frequency_1 = std::bitset<8>((short int) (frequencies[0] - FREQUENCY_START) * 10);
        return alternative_frequency_1;
    }

    /**
     * Group type: 0A
     * -af
     * Alternative Frequency 2nd (8-bit unsigned integer).
     *
     * @return std::vector<double>
     *
     * @throws std::invalid_argument
     */
    std::bitset<8> get_alternative_frequency_2() {
        std::vector<double> frequencies = this->_get_alternative_frequencies();

        // AF2: 8 - bit
        const auto offset = (frequencies[1] - FREQUENCY_START) * 10;
//        DEBUG_PRINT_LITE("Frequency 2: %f, size: %d, offset: %d\n", frequencies[1], frequencies.size(), offset);
        auto alternative_frequency_2 = std::bitset<8>(offset);
        return alternative_frequency_2;
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
    std::string get_program_service() {
        const char *program_service = this->_get_arg("-ps", "--program-service");
        if (program_service == nullptr) {
            throw std::invalid_argument("Program service is not specified. Option: -ps, --program-service. Group type: 0A");
        }

        // length of the string is smaller that 8 add padding spaces to the end
        if (strlen(program_service) < 8) {
            std::string str(program_service);
            str.resize(8, ' ');
            return str;
        }
        return program_service;
    }

    /**
     * Group type: 2A
     * -rt
     * Radio Text
     *
     * @return bool
     *
     * @throws std::invalid_argument
     */
    std::string get_radio_text() {
        const char *radio_text = this->_get_arg("-rt", "--radio-text");
        if (radio_text == nullptr) {
            throw std::invalid_argument("Radio text is not specified. Option: -rt, --radio-text. Group type: 2A");
        }

        // If radio text ength is smaller than 64 add padding spaces to the end
        if (strlen(radio_text) < 64) {
            std::string str(radio_text);
            str.resize(64, ' ');
            return str;
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

const auto OFFSET_WORDS = std::map<std::string, std::bitset<10>>{
        {"A", std::bitset<10>(0b0011111100)},
        {"B", std::bitset<10>(0b0110011000)},
        {"C", std::bitset<10>(0b0101101000)},
        {"D", std::bitset<10>(0b0110110100)},
        {"E", std::bitset<10>(0b0000000000)},
};

/**
 * @brief Optimized CRC-10 calculation function for a 16-bit message.
 *
 * @param message The 16-bit message to compute the CRC for.
 * @return std::bitset<10> The 10-bit CRC value.
 */
std::bitset<10> calculate_crc(const std::bitset<16> &message, std::bitset<10> offset) {
    uint32_t data = message.to_ulong();  // Convert the 16-bit message to an unsigned integer
    data <<= 10;  // Shift left by 10 bits to append 10 zero bits for CRC (message * x^10)

    const int total_bits = 26;  // 16-bit message + 10-bit CRC space

    // Perform modulo-2 division (XOR) for CRC calculation
    for (int i = total_bits - 1; i >= 10; --i) {
        if (data & (1 << i)) {  // If the leading bit is set
            data ^= (CRC_POLYNOMIAL << (i - 10));  // XOR with the generator polynomial shifted appropriately
        }
    }

    // The remainder is the CRC value
    auto crc = std::bitset<10>(data & 0x3FF);  // Mask out the 10 bits for the CRC
    crc ^= offset;  // XOR with the offset word
    return crc;
}

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

    std::bitset<SIZE_0A> process_0A() {
        try {
            ////////////////////////////
            /// BLOCK 1
            ////////////////////////////
            // PI Code: 16 - bit
            const auto program_id = args->get_program_identifier();
            const auto block_A = std::bitset<16>(program_id);
            DEBUG_PRINT_LITE("Block A: %s\n", block_A.to_string().c_str());

            // NOTE: Checkword + Offset A
            const auto crc_A = calculate_crc(block_A, OFFSET_WORDS.at("A"));
            DEBUG_PRINT_LITE("CRC A: %s\n", crc_A.to_string().c_str());

            ////////////////////////////
            /// BLOCK 2
            ////////////////////////////
            // Group Type: 4 - bit
            std::bitset<4> group_type_bits(0);
            DEBUG_PRINT_LITE("Program Type bits: %s\n", group_type_bits.to_string().c_str());

            // ODA Type: 1 - bit
            std::bitset<1> oda_type_bits(ODA_TYPE_A);
            DEBUG_PRINT_LITE("ODA Type bits: %s\n", oda_type_bits.to_string().c_str());

            // Traffic Program: 1 - bit
            const auto traffic_program = args->get_traffic_program();
            std::bitset<1> traffic_program_bits(traffic_program);
            DEBUG_PRINT_LITE("Traffic Program bits: %s\n", traffic_program_bits.to_string().c_str());

            // Program Type: 5 - bit
            const auto program_type = args->get_program_type();
            std::bitset<5> program_type_bits(program_type);
            DEBUG_PRINT_LITE("Program Type bits: %s\n", program_type_bits.to_string().c_str());

            // Traffic Announcement: 1 - bit
            const auto traffic_announcement = args->get_traffic_announcement();
            std::bitset<1> traffic_announcement_bits(traffic_announcement);
            DEBUG_PRINT_LITE("Traffic Announcement bits: %s\n", traffic_announcement_bits.to_string().c_str());

            // Music/Speech: 1 - bit
            const auto music_speech = args->get_music_speech();
            std::bitset<1> music_speech_bits(music_speech);
            DEBUG_PRINT_LITE("Music/Speech bits: %s\n", music_speech_bits.to_string().c_str());

            // Decode Identifier: 1 - bit
            std::bitset<1> decode_identifier_bits(0); // always 0
            DEBUG_PRINT_LITE("Decode Identifier bits: %s\n", decode_identifier_bits.to_string().c_str());

            auto block_B = std::bitset<16>(
                    (group_type_bits.to_ulong() << 12) |
                    (oda_type_bits.to_ulong() << 11) |
                    (traffic_program_bits.to_ulong() << 10) |
                    (program_type_bits.to_ulong() << 5) |
                    (traffic_announcement_bits.to_ulong() << 4) |
                    (music_speech_bits.to_ulong() << 3) |
                    (decode_identifier_bits.to_ulong() << 2)
            );
            DEBUG_PRINT_LITE("Block B %s\n", block_B.to_string().c_str());

            // NOTE: Word part: here, but calculated later

            // NOTE: Checkword + Offset B

            ////////////////////////////
            /// BLOCK 3
            ////////////////////////////
            // Alternative Frequency 1: 8 - bit
            const auto alternative_frequency_1 = args->get_alternative_frequency_1();
            DEBUG_PRINT_LITE("Alternative Frequency 1: %s\n", alternative_frequency_1.to_string().c_str());

            // Alternative Frequency 2: 8 - bit
            const auto alternative_frequency_2 = args->get_alternative_frequency_2();
            DEBUG_PRINT_LITE("Alternative Frequency 2: %s\n", alternative_frequency_2.to_string().c_str());

            // NOTE: Checkword + Offset C
            auto block_C = std::bitset<16>((alternative_frequency_1.to_ulong() << 8) | alternative_frequency_2.to_ulong());
            DEBUG_PRINT_LITE("Alternative Frequency bits: %s\n", block_C.to_string().c_str());
            auto crc_C = calculate_crc(block_C, OFFSET_WORDS.at("C"));
            DEBUG_PRINT_LITE("CRC 3: %s\n", crc_C.to_string().c_str());

            ////////////////////////////
            /// BLOCK 4
            ////////////////////////////
            const auto block_D = args->get_program_service();
            DEBUG_PRINT_LITE("Program Service: '%s'\n", block_D.c_str());

            // NOTE: Checkword + Offset D


            ////////////////////////////
            /// Assemble the packet
            ////////////////////////////
            std::bitset<BLOCKS_COUNT_IN_0A * BLOCK_ROW_SIZE> all_blocks(0);
            std::bitset<SIZE_0A> packet(0);
            for (int i = 0; i < block_D.size(); i = i + 2) {
//            for (int i = 0; i < 1; i = i + 2) {
                // BLOCK A:
                all_blocks |= std::bitset<BLOCKS_COUNT_IN_0A * BLOCK_ROW_SIZE>(block_A.to_ulong()) << (3 * 26 + 10);
                all_blocks |= std::bitset<BLOCKS_COUNT_IN_0A * BLOCK_ROW_SIZE>(crc_A.to_ulong()) << (3 * 26);

                // BLOCK B 1.:
                // because i increasing by 2, but the segment number must increase by 1
                const auto segment = std::bitset<2>(i / 2);
                block_B &= std::bitset<16>(0b1111111111111100); // clear 1st two bits in data_B
                block_B |= std::bitset<16>(segment.to_ulong());
                all_blocks |= std::bitset<BLOCKS_COUNT_IN_0A * BLOCK_ROW_SIZE>(block_B.to_ulong()) << (2 * 26 + 10);
                const auto crc_B = calculate_crc(block_B, OFFSET_WORDS.at("B"));
//                DEBUG_PRINT_LITE("(i=%d)CRC B: %s\n", i, crc_B.to_string().c_str());
                all_blocks |= std::bitset<BLOCKS_COUNT_IN_0A * BLOCK_ROW_SIZE>(crc_B.to_ulong()) << (2 * 26);

                // BLOCK C:
                if (i == 0) {
                    all_blocks |= std::bitset<BLOCKS_COUNT_IN_0A * BLOCK_ROW_SIZE>(block_C.to_ulong()) << (1 * 26 + 10);
                    all_blocks |= std::bitset<BLOCKS_COUNT_IN_0A * BLOCK_ROW_SIZE>(crc_C.to_ulong()) << (1 * 26);
                } else {
                    block_C = std::bitset<16>(0);
                    crc_C = calculate_crc(block_C, OFFSET_WORDS.at("C"));
                    all_blocks |= std::bitset<BLOCKS_COUNT_IN_0A * BLOCK_ROW_SIZE>(block_C.to_ulong()) << (1 * 26 + 10);
                    all_blocks |= std::bitset<BLOCKS_COUNT_IN_0A * BLOCK_ROW_SIZE>(crc_C.to_ulong()) << (1 * 26);
                }


                // BLOCK D:
                const auto chars = block_D.substr(i, 2);
                uint16_t combined_value = (static_cast<uint16_t>(chars[0]) << 8) | static_cast<uint16_t>(chars[1]);
                const auto crc_4 = calculate_crc(std::bitset<16>(combined_value), OFFSET_WORDS.at("D"));
                all_blocks |= std::bitset<BLOCKS_COUNT_IN_0A * BLOCK_ROW_SIZE>(combined_value) << (0 * 26 + 10);
                all_blocks |= std::bitset<BLOCKS_COUNT_IN_0A * BLOCK_ROW_SIZE>(crc_4.to_ulong()) << (0 * 26);

                const auto offset = BLOCKS_COUNT_IN_0A - (i / 2) - 1;
                const auto total_offset = offset * BLOCKS_COUNT_IN_0A * BLOCK_ROW_SIZE;
//                DEBUG_PRINT_LITE("offset: %d, total_offset: %d\n", offset, total_offset);
//                DEBUG_PRINT_LITE("all_blocks: %s\n", all_blocks.to_string().c_str());
                const auto or_bits = std::bitset<packet.size()>(all_blocks.to_string()) << total_offset;
//                DEBUG_PRINT_LITE("or_bits: %s\n", or_bits.to_string().c_str());
                packet |= or_bits;

//                DEBUG_PRINT_LITE("Packet: %d\n", i);
                print_packet(all_blocks);
                all_blocks.reset();
            }
            return packet;
        } catch (const std::exception &e) {
            std::cerr << "Error processing Group 0A: " << e.what() << std::endl;
        }
    }

    std::bitset<SIZE_2A> process_2A() {
        try {
            ////////////////////////////
            /// BLOCK 1
            ////////////////////////////
            // PI Code: 16 - bit
            const auto program_id = args->get_program_identifier();
            const auto block_A = std::bitset<16>(program_id);
            DEBUG_PRINT_LITE("Block A: %s\n", block_A.to_string().c_str());

            // NOTE: Checkword + Offset A
            const auto crc_A = calculate_crc(block_A, OFFSET_WORDS.at("A"));
            DEBUG_PRINT_LITE("CRC A: %s\n", crc_A.to_string().c_str());

            ////////////////////////////
            /// BLOCK 2
            ////////////////////////////
            // Group Type: 4 - bit
            std::bitset<4> group_type_bits(2); // 2 for Group 2A
            DEBUG_PRINT_LITE("Group Type bits: %s\n", group_type_bits.to_string().c_str());

            // ODA Type: 1 - bit
            std::bitset<1> oda_type_bits(ODA_TYPE_A);
            DEBUG_PRINT_LITE("ODA Type bits: %s\n", oda_type_bits.to_string().c_str());

            // Traffic Program: 1 - bit
            const auto traffic_program = args->get_traffic_program();
            std::bitset<1> traffic_program_bits(traffic_program);
            DEBUG_PRINT_LITE("Traffic Program bits: %s\n", traffic_program_bits.to_string().c_str());

            // Program Type: 5 - bit
            const auto program_type = args->get_program_type();
            std::bitset<5> program_type_bits(program_type);
            DEBUG_PRINT_LITE("Program Type bits: %s\n", program_type_bits.to_string().c_str());

            // Radio Text A/B Flag: 1 - bit
            const auto radio_text_ab_flag = args->get_radio_text_ab_flag();
            std::bitset<1> radio_text_ab_bits(radio_text_ab_flag);
            DEBUG_PRINT_LITE("Radio Text A/B Flag bits: %s\n", radio_text_ab_bits.to_string().c_str());

            // NOTE: Word part: here, but calculated later

            // NOTE: Checkword + Offset B

            // Segment Address: 4 - bit
            std::bitset<4> segment_address_bits(0); // Segment address starts from 0

            // Forming Block B
            auto block_B = std::bitset<16>(
                    (group_type_bits.to_ulong() << 12) |
                    (oda_type_bits.to_ulong() << 11) |
                    (traffic_program_bits.to_ulong() << 10) |
                    (program_type_bits.to_ulong() << 5) |
                    (radio_text_ab_bits.to_ulong() << 4) |
                    (segment_address_bits.to_ulong())
            );
            DEBUG_PRINT_LITE("Block B: %s\n", block_B.to_string().c_str());

            ////////////////////////////
            /// BLOCK 3
            ////////////////////////////
            // Radio Text: 16 bits
            const auto radio_text = args->get_radio_text();
            DEBUG_PRINT_LITE("Radio Text: '%s'\n", radio_text.c_str());

            ////////////////////////////
            /// Assemble the packet
            ////////////////////////////
            std::bitset<BLOCKS_COUNT_IN_0A * BLOCK_ROW_SIZE> all_blocks(0);
            std::bitset<SIZE_2A> packet(0);
            for (int i = 0; i < radio_text.size(); i += 4) { // Processing 4 characters per iteration
//            for (int i = 0; i < 2; i += 4) { // Processing 4 characters per iteration
                // BLOCK A:
                all_blocks |= std::bitset<BLOCK_PARTS_COUNT * BLOCK_ROW_SIZE>(block_A.to_ulong()) << (3 * 26 + 10);
                all_blocks |= std::bitset<BLOCK_PARTS_COUNT * BLOCK_ROW_SIZE>(crc_A.to_ulong()) << (3 * 26);

                // BLOCK B:
                segment_address_bits = std::bitset<4>(i / 4); // Update segment address
                block_B &= std::bitset<16>(0b1111111111110000); // Clear segment address bits in data_B
                block_B |= std::bitset<16>(segment_address_bits.to_ulong());
                all_blocks |= std::bitset<BLOCK_PARTS_COUNT * BLOCK_ROW_SIZE>(block_B.to_ulong()) << (2 * 26 + 10);
                const auto crc_B = calculate_crc(block_B, OFFSET_WORDS.at("B"));
                all_blocks |= std::bitset<BLOCK_PARTS_COUNT * BLOCK_ROW_SIZE>(crc_B.to_ulong()) << (2 * 26);

                // BLOCK C:
                const auto chars_C = radio_text.substr(i, 2);
                uint16_t combined_value_C = (static_cast<uint16_t>(chars_C[0]) << 8) | static_cast<uint16_t>(chars_C[1]);
                const auto crc_C = calculate_crc(std::bitset<16>(combined_value_C), OFFSET_WORDS.at("C"));
                all_blocks |= std::bitset<BLOCK_PARTS_COUNT * BLOCK_ROW_SIZE>(combined_value_C) << (1 * 26 + 10);
                all_blocks |= std::bitset<BLOCK_PARTS_COUNT * BLOCK_ROW_SIZE>(crc_C.to_ulong()) << (1 * 26);

                // BLOCK D:
                const auto chars_D = radio_text.substr(i + 2, 2);
                uint16_t combined_value_D = (static_cast<uint16_t>(chars_D[0]) << 8) | static_cast<uint16_t>(chars_D[1]);
                const auto crc_D = calculate_crc(std::bitset<16>(combined_value_D), OFFSET_WORDS.at("D"));
                all_blocks |= std::bitset<BLOCK_PARTS_COUNT * BLOCK_ROW_SIZE>(combined_value_D) << (0 * 26 + 10);
                all_blocks |= std::bitset<BLOCK_PARTS_COUNT * BLOCK_ROW_SIZE>(crc_D.to_ulong()) << (0 * 26);
                print_packet(all_blocks);

//                DEBUG_PRINT_LITE("all_blocks: %s\n", all_blocks.to_string().c_str());

                // Calculate total offset and combine blocks into the packet
                const auto offset = BLOCKS_COUNT_IN_2A - (i / 4) - 1;
                const auto total_offset = offset * BLOCK_PARTS_COUNT * BLOCK_ROW_SIZE;
                const auto or_bits = std::bitset<packet.size()>(all_blocks.to_string()) << total_offset;
                packet |= or_bits;

//                DEBUG_PRINT_LITE("Packet after iteration %d: %s\n", i, packet.to_string().c_str());

//                // Reset all_blocks for next iteration
                all_blocks.reset();
            }
            return packet;
        } catch (const std::exception &e) {
            std::cerr << "Error processing Group 2A: " << e.what() << std::endl;
            return {0};
        }
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

        if (group_type == Args::GroupType::A0) {
            DEBUG_PRINT_LITE("Processing Group %s\n", "0A");
            const auto packet = program->process_0A();
            std::cout << packet.to_string() << std::endl;
        }

        if (group_type == Args::GroupType::A2) {
            DEBUG_PRINT_LITE("Processing Group %s\n", "2A");
            const auto packet = program->process_2A();
            print_packet(packet);
            std::cout << packet.to_string() << std::endl;
        }
//        const auto type = program->args->get_program_type();
//        DEBUG_PRINT_LITE("Program Type: %d\n", type);
//        const auto program_id = program->args->get_program_identifier();
//        DEBUG_PRINT_LITE("Program ID: %d\n", program_id);
//        const auto traffic_program = program->args->get_traffic_program();
//        DEBUG_PRINT_LITE("Traffic Program: %d\n", traffic_program);
//        const auto music = program->args->get_music_speech();
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
