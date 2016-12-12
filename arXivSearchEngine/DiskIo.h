#ifndef DISKIO_H
#define DISKIO_H

#include <fstream>
#include <cstdint>
#include <cmath>


class DiskIo {
private:
    // For correcting endianness issues
    static inline uint8_t Reverse(uint8_t value);
    static inline uint32_t Reverse(uint32_t value);
    static inline uint64_t Reverse(uint64_t value);

public:
    static void WriteChar(std::ofstream &stream, const uint8_t &character);
    static uint8_t ReadChar(std::ifstream &stream);

    static void WriteInt(std::ofstream &stream, const uint32_t &integer);
    static uint32_t ReadInt(std::ifstream &stream);

    static void WriteLong(std::ofstream &stream, const uint64_t &number);
    static uint64_t ReadLong(std::ifstream &stream);

    static void WriteDouble(std::ofstream &stream, const double_t &decimal);
    static double_t ReadDouble(std::ifstream &stream);

    static void WriteVbeInt(std::ofstream &stream, const uint32_t &integer);
    static uint32_t ReadVbeInt(std::ifstream &stream);

};

#endif
