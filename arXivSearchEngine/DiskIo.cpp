#include "DiskIo.h"
#include <iostream>

// For correcting endianness issues
uint8_t DiskIo::Reverse(uint8_t value) {
    value = (value & 0xF0) >> 4 | (value & 0x0F) << 4;
    value = (value & 0xCC) >> 2 | (value & 0x33) << 2;
    value = (value & 0xAA) >> 1 | (value & 0x55) << 1;
    return value;
}

// For correcting endianness issues
uint32_t DiskIo::Reverse(uint32_t value) {
    return (value & 0xFF000000) >> 24 |
        (value & 0x00FF0000) >> 8 |
        (value & 0x0000FF00) << 8 |
        (value & 0x000000FF) << 24;
}

// For correcting endianness issues
uint64_t DiskIo::Reverse(uint64_t value) {
    return (value & 0xFF00000000000000) >> 56 |
        (value & 0x00FF000000000000) >> 40 |
        (value & 0x0000FF0000000000) >> 24 |
        (value & 0x000000FF00000000) >> 8 |
        (value & 0x00000000FF000000) << 8 |
        (value & 0x0000000000FF0000) << 24 |
        (value & 0x000000000000FF00) << 40 |
        (value & 0x00000000000000FF) << 56;
}

void DiskIo::WriteChar(std::ofstream &stream, const uint8_t &character) {
    stream.write((const char*)&character, sizeof(character));
}

uint8_t DiskIo::ReadChar(std::ifstream &stream) {
    uint8_t value = 0;
    stream.read((char*)&value, sizeof(value));
    return value;
}

void DiskIo::WriteInt(std::ofstream &stream, const uint32_t &integer) {
    uint32_t reverse = Reverse(integer);
    stream.write((const char*)&reverse, sizeof(reverse));
}

uint32_t DiskIo::ReadInt(std::ifstream &stream) {
    uint32_t value = 0;
    stream.read((char*)&value, sizeof(value));
    return Reverse(value);
}

void DiskIo::WriteLong(std::ofstream &stream, const uint64_t &number) {
    uint64_t reverse = Reverse(number);
    stream.write((const char*)&reverse, sizeof(number));
}

uint64_t DiskIo::ReadLong(std::ifstream &stream) {
    uint64_t value = 0;
    stream.read((char*)&value, sizeof(value));
    return Reverse(value);
}

void DiskIo::WriteDouble(std::ofstream &stream, const double_t &decimal) {
    stream.write((const char*)&decimal, sizeof(decimal));
}

double_t DiskIo::ReadDouble(std::ifstream &stream) {
    double_t value = 0;
    stream.read((char*)&value, sizeof(value));
    return value;
}

void DiskIo::WriteVbeInt(std::ofstream &stream, const uint32_t &integer) {
    if(((integer & 0xF0000000) >> 28) > 0)
        WriteChar(stream, ((integer & 0xF0000000) >> 28) | 0x80);
    if(((integer & 0x0FE00000) >> 21) > 0)
        WriteChar(stream, ((integer & 0x0FE00000) >> 21) | 0x80);
    if(((integer & 0x001FC000) >> 14) > 0)
        WriteChar(stream, ((integer & 0x001FC000) >> 14) | 0x80);
    if(((integer & 0x00003F80) >> 7) > 0)
        WriteChar(stream, ((integer & 0x00003F80) >> 7) | 0x80);
    WriteChar(stream, integer & 0x0000007F);
}

uint32_t DiskIo::ReadVbeInt(std::ifstream &stream) {
    uint32_t value = 0;
    while(true) {
        char c = ReadChar(stream);
        value |= (0x000000FF & c);
        if(!((c >> 7) & 1)) {
            return value;
        }
        value <<= 7;
    }
}
