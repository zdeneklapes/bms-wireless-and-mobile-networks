#ifndef SHARED_HPP
#define SHARED_HPP

#include <string>
#include <bitset>
#include <stdexcept>

#define ODA_TYPE_A 0
#define CRC_BITS 10
#define DATA_BITS 16
#define BLOCK_ROW_SIZE (CRC_BITS + DATA_BITS)
#define BLOCK_PARTS_COUNT 4
#define BLOCKS_COUNT_IN_0A 4
#define BLOCKS_COUNT_IN_2A (BLOCKS_COUNT_IN_0A * BLOCKS_COUNT_IN_0A)
#define FREQUENCY_START 87.5
#define SIZE_0A (BLOCK_PARTS_COUNT * BLOCK_PARTS_COUNT * BLOCK_ROW_SIZE)
#define SIZE_2A (BLOCKS_COUNT_IN_2A * BLOCK_PARTS_COUNT * BLOCK_ROW_SIZE)

#define DEBUG 0
#define DEBUG_LITE DEBUG
#define DEBUG_PRINT_LITE(fmt, ...) \
            do { if (DEBUG_LITE) fprintf(stderr, fmt, __VA_ARGS__); } while (0)
#define DEBUG_PRINT(fmt, ...) \
        do { if (DEBUG) fprintf(stderr, "%s:%d:%s(): " fmt, __FILE__, \
                                __LINE__, __func__, __VA_ARGS__); } while (0)

//template<std::size_t N>
//void print_packet(const std::bitset<N> &packet);

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

#endif
