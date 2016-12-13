#ifndef DISKIO_H
#define DISKIO_H

#include <fstream>
#include <cstdint>
#include <cmath>


class DiskIo {
private:
    // For correcting endianness issues
    static inline uint8_t Reverse(uint8_t value) {
        value = (value & 0xF0) >> 4 | (value & 0x0F) << 4;
        value = (value & 0xCC) >> 2 | (value & 0x33) << 2;
        value = (value & 0xAA) >> 1 | (value & 0x55) << 1;
        return value;
    }

    // For correcting endianness issues
    static inline uint32_t Reverse(uint32_t value) {
        return (value & 0xFF000000) >> 24 |
            (value & 0x00FF0000) >> 8 |
            (value & 0x0000FF00) << 8 |
            (value & 0x000000FF) << 24;
    }

    // For correcting endianness issues
    static inline uint64_t Reverse(uint64_t value) {
        return (value & 0xFF00000000000000) >> 56 |
            (value & 0x00FF000000000000) >> 40 |
            (value & 0x0000FF0000000000) >> 24 |
            (value & 0x000000FF00000000) >> 8 |
            (value & 0x00000000FF000000) << 8 |
            (value & 0x0000000000FF0000) << 24 |
            (value & 0x000000000000FF00) << 40 |
            (value & 0x00000000000000FF) << 56;
    }

public:
    static void WriteChar(std::ofstream &stream, uint8_t &character);
    static uint8_t ReadChar(std::ifstream &stream);

    static void WriteInt(std::ofstream &stream, uint32_t &integer);
    static uint32_t ReadInt(std::ifstream &stream);

    static void WriteLong(std::ofstream &stream, uint64_t &number);
    static uint64_t ReadLong(std::ifstream &stream);

    static double_t WriteDouble(std::ofstream &stream, double_t &decimal);
    static double_t ReadDouble(std::ifstream &stream);
};

#endif
