/**
 * @file shared.hpp
 * @author Zdeněk Lapeš <lapes.zdenek@gmail.com>
 * @date 2024-11-18
 */
#ifndef SHARED_HPP
#define SHARED_HPP

#include <string>
#include <bitset>
#include <stdexcept>

#define ODA_TYPE_A (0)
#define CRC_BITS (10)
#define DATA_BITS (16)
#define BLOCK_ROW_SIZE (CRC_BITS + DATA_BITS)
#define BLOCK_PARTS_COUNT (4)
#define BLOCKS_COUNT_IN_0A (4)
#define BLOCKS_COUNT_IN_2A (BLOCKS_COUNT_IN_0A * BLOCKS_COUNT_IN_0A)
#define FREQUENCY_START (87.5)
#define SIZE_0A (BLOCK_PARTS_COUNT * BLOCK_PARTS_COUNT * BLOCK_ROW_SIZE)
#define SIZE_2A (BLOCKS_COUNT_IN_2A * BLOCK_PARTS_COUNT * BLOCK_ROW_SIZE)
#define CRC_POLYNOMIAL (0b10110111001)
#define REGEX_TEXT "[a-zA-Z0-9 ]*"

#define DEBUG (0)
#define DEBUG_LITE (DEBUG)
#define DEBUG_PRINT_LITE(fmt, ...) \
            do { if (DEBUG_LITE) fprintf(stderr, fmt, __VA_ARGS__); } while (0)
#define DEBUG_PRINT(fmt, ...) \
        do { if (DEBUG) fprintf(stderr, "%s:%d:%s(): " fmt, __FILE__, \
                                __LINE__, __func__, __VA_ARGS__); } while (0)


/**
 * @brief Print the packet in blocks.
 *
 * @tparam N The size of the packet.
 * @param packet The packet to print.
 */
template<std::size_t N>
void print_packet(const std::bitset<N> &packet) {
    // Check size is multiple of 26
    if (packet.size() % BLOCK_ROW_SIZE != 0) {
        throw std::invalid_argument("Packet size must be a multiple of 26 bits.");
    }

    const auto blocks = packet.size() / BLOCK_ROW_SIZE;

    const auto foo = packet.to_string();
    DEBUG_PRINT_LITE("Packet/Block: %s\n", foo.c_str());

    // Print whole packet as blocks
    for (int i = 0; i < blocks; i++) {
        std::bitset<BLOCK_ROW_SIZE> data_22 = std::bitset<data_22.size()>((packet << (i * BLOCK_ROW_SIZE)).to_string());
        const auto group = i % 4 == 0 ? 'A' : (i - 1) % 4 == 0 ? 'B' : (i - 2) % 4 == 0 ? 'C' : 'D';
        DEBUG_PRINT_LITE("(%d) Block %c: %s | %s\n", i / 4, group, data_22.to_string().substr(0, 16).c_str(), data_22.to_string().substr(16, 10).c_str());
    }
}

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
 * @brief Offset words for each group.
 */
const auto OFFSET_WORDS = std::map<std::string, std::bitset<CRC_BITS>>{
        {"A", std::bitset<CRC_BITS>(0b0011111100)},
        {"B", std::bitset<CRC_BITS>(0b0110011000)},
        {"C", std::bitset<CRC_BITS>(0b0101101000)},
        {"D", std::bitset<CRC_BITS>(0b0110110100)},
};


#endif
