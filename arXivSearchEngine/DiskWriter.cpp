#include "DiskWriter.h"

// For correcting endianness issues
uint8_t DiskWriter::Reverse(uint8_t value) {
    value = (value & 0xF0) >> 4 | (value & 0x0F) << 4;
    value = (value & 0xCC) >> 2 | (value & 0x33) << 2;
    value = (value & 0xAA) >> 1 | (value & 0x55) << 1;
    return value;
}

// For correcting endianness issues
uint32_t DiskWriter::Reverse(uint32_t value) {
    return (value & 0xFF000000) >> 24 |
        (value & 0x00FF0000) >> 8 |
        (value & 0x0000FF00) << 8 |
        (value & 0x000000FF) << 24;
}

// For correcting endianness issues
uint64_t DiskWriter::Reverse(uint64_t value) {
    return (value & 0xFF00000000000000) >> 56 |
        (value & 0x00FF000000000000) >> 40 |
        (value & 0x0000FF0000000000) >> 24 |
        (value & 0x000000FF00000000) >> 8 |
        (value & 0x00000000FF000000) << 8 |
        (value & 0x0000000000FF0000) << 24 |
        (value & 0x000000000000FF00) << 40 |
        (value & 0x00000000000000FF) << 56;
}

DiskWriter::DiskWriter(const std::string &fileName) {
    auto filepath = boost::filesystem::current_path().append(fileName, boost::filesystem::path::codecvt()).string();
    stream.open(filepath, std::ios_base::out | std::ios_base::binary);
}

DiskWriter::DiskWriter(const std::string &filePath, const std::string &fileName) {
    auto filepath = boost::filesystem::path(filePath).append(fileName, boost::filesystem::path::codecvt()).string();
     stream.open(filepath, std::ios_base::out | std::ios_base::binary);
}

void DiskWriter::writeChar(const uint8_t character) {
    stream.write((const char*)&character, sizeof(character));
}

void DiskWriter::writeInt(const uint32_t integer) {
    uint32_t reverse = Reverse(integer);
    stream.write((const char*)&reverse, sizeof(reverse));
}

void DiskWriter::writeLong(const uint64_t number) {
    uint64_t reverse = Reverse(number);
    stream.write((const char*)&reverse, sizeof(number));
}

void DiskWriter::writeDouble(const double_t decimal) {
    stream.write((const char*)&decimal, sizeof(decimal));
}

void DiskWriter::writeVbeInt(const uint32_t integer) {
    if(((integer & 0xF0000000) >> 28) > 0)
        writeChar(((integer & 0xF0000000) >> 28) | 0x80);
    if(((integer & 0x0FE00000) >> 21) > 0)
        writeChar(((integer & 0x0FE00000) >> 21) | 0x80);
    if(((integer & 0x001FC000) >> 14) > 0)
        writeChar(((integer & 0x001FC000) >> 14) | 0x80);
    if(((integer & 0x00003F80) >> 7) > 0)
        writeChar(((integer & 0x00003F80) >> 7) | 0x80);
    writeChar(integer & 0x0000007F);
}
