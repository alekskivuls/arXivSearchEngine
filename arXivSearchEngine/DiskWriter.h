#ifndef DISKWRITER_H
#define DISKWRITER_H

#include <boost/filesystem.hpp>
#include <fstream>
#include <cstdint>
#include <cmath>


class DiskWriter {
private:
    std::ofstream stream;

    // For correcting endianness issues
    uint8_t Reverse(uint8_t value);
    uint32_t Reverse(uint32_t value);
    uint64_t Reverse(uint64_t value);

public:
    DiskWriter(const std::string &fileName);

    DiskWriter(const std::string &filePath, const std::string &fileName);

    void writeChar(const uint8_t &character);

    void writeInt(const uint32_t &integer);

    void writeLong(const uint64_t &number);

    void writeDouble(const double_t &decimal);

    void writeVbeInt(const uint32_t &integer);

};

#endif
