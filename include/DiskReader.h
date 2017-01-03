#ifndef DISKREADER_H
#define DISKREADER_H

#include <boost/filesystem.hpp>
#include <fstream>
#include <cstdint>
#include <cmath>


class DiskReader {
private:
    std::ifstream stream;

    // For correcting endianness issues
    uint8_t Reverse(uint8_t value);
    uint32_t Reverse(uint32_t value);
    uint64_t Reverse(uint64_t value);

public:
    DiskReader();

    DiskReader(const std::string &fileName);

    DiskReader(const std::string &filePath, const std::string &fileName);

    DiskReader(const boost::filesystem::path &filePath, const std::string &fileName);

    uint8_t readChar();

    uint32_t readInt();

    uint64_t readLong();

    double_t readDouble();

    uint32_t readVbeInt();

};

#endif
