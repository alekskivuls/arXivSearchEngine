#ifndef COMPRESSOR_H
#define COMPRESSOR_H

#include <cstdint>
#include <string>
#include <list>

class Compressor {

public: // build inverted first. build kgram index based on key values of invertedindex AFTER

	static std::list<unsigned char> encodeNumber(uint64_t source);

	static std::string encode(const std::list<int> &source);

	static uint32_t decode(const uint32_t &source);
};

#endif