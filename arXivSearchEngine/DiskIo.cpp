#include "DiskIo.h"

void DiskIo::WriteChar(std::ofstream &stream, uint8_t &character) {
    stream.write((const char*)&character, sizeof(character));
}

uint8_t DiskIo::ReadChar(std::ifstream &stream) {
    uint8_t value = 0;
    stream.read((char*)&value, sizeof(value));
    return value;
}

void DiskIo::WriteInt(std::ofstream &stream, uint32_t &integer) {
    uint32_t reverse = Reverse(integer);
    stream.write((const char*)&reverse, sizeof(reverse));
}

uint32_t DiskIo::ReadInt(std::ifstream &stream) {
    uint32_t value = 0;
    stream.read((char*)&value, sizeof(value));
    return Reverse(value);
}

void DiskIo::WriteLong(std::ofstream &stream, uint64_t &number) {
    uint64_t reverse = Reverse(number);
    stream.write((const char*)&reverse, sizeof(number));
}

uint64_t DiskIo::ReadLong(std::ifstream &stream) {
    uint64_t value = 0;
    stream.read((char*)&value, sizeof(value));
    return Reverse(value);
}

double_t DiskIo::WriteDouble(std::ofstream &stream, double_t &decimal) {
    stream.write((const char*)&decimal, sizeof(decimal));
}

double_t DiskIo::ReadDouble(std::ifstream &stream) {
    double_t value = 0;
    stream.read((char*)&value, sizeof(value));
    return value;
}
