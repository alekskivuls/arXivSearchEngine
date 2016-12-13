#include "DiskReader.h"
#include <iostream>

// For correcting endianness issues
uint8_t DiskReader::Reverse(uint8_t value) {
    value = (value & 0xF0) >> 4 | (value & 0x0F) << 4;
    value = (value & 0xCC) >> 2 | (value & 0x33) << 2;
    value = (value & 0xAA) >> 1 | (value & 0x55) << 1;
    return value;
}

// For correcting endianness issues
uint32_t DiskReader::Reverse(uint32_t value) {
    return (value & 0xFF000000) >> 24 |
        (value & 0x00FF0000) >> 8 |
        (value & 0x0000FF00) << 8 |
        (value & 0x000000FF) << 24;
}

// For correcting endianness issues
uint64_t DiskReader::Reverse(uint64_t value) {
    return (value & 0xFF00000000000000) >> 56 |
        (value & 0x00FF000000000000) >> 40 |
        (value & 0x0000FF0000000000) >> 24 |
        (value & 0x000000FF00000000) >> 8 |
        (value & 0x00000000FF000000) << 8 |
        (value & 0x0000000000FF0000) << 24 |
        (value & 0x000000000000FF00) << 40 |
        (value & 0x00000000000000FF) << 56;
}

DiskReader::DiskReader(const std::string &fileName) {
    stream.open(boost::filesystem::current_path().append(fileName, boost::filesystem::path::codecvt()).string(),
                    std::ios_base::in | std::ios_base::binary);
}

DiskReader::DiskReader(const std::string &filePath, const std::string &fileName) {
    stream.open(boost::filesystem::path(filePath).append(fileName, boost::filesystem::path::codecvt()).string(),
                    std::ios_base::in | std::ios_base::binary);
}

uint8_t DiskReader::readChar() {
    uint8_t value = 0;
    stream.read((char*)&value, sizeof(value));
    std::cout << +value << std::endl;
    return value;
}

uint32_t DiskReader::readInt() {
    uint32_t value = 0;
    stream.read((char*)&value, sizeof(value));
    return Reverse(value);
}

uint64_t DiskReader::readLong() {
    uint64_t value = 0;
    stream.read((char*)&value, sizeof(value));
    return Reverse(value);
}

double_t DiskReader::readDouble() {
    double_t value = 0;
    stream.read((char*)&value, sizeof(value));
    return value;
}

uint32_t DiskReader::readVbeInt() {
    uint32_t value = 0;
    while(true) {
        char c = readChar();
        value |= (0x000000FF & c);
        if(!((c >> 7) & 1)) {
            return value;
        }
        value <<= 7;
    }
}
