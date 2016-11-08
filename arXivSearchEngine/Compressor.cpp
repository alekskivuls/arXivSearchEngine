#include "Compressor.h"
#include <iostream>
#include <stdint.h>

/*
 * Future implementation must include gap encoding to reduce the amount of bytes consumed.
 * Return a smaller data type later...
 */
std::list<unsigned char> Compressor::encodeNumber(uint64_t source) { // pass in string
	std::list<unsigned char> output;
	unsigned char i, c = 0, pos = 0;

	for (i = 0; i < 7; ++i) {
		if ((source >> i) & 1) c |= 1 << i; // instead of c... use output
	}
	c |= 1 << 7;
	output.push_front(c);
	//std::cout << (int)c << std::endl;
	source = source >> 7; // erase bits

	while (source != 0) { // this will never execute if source < 128
		c = 0;
		for (i = 0; i < 7; ++i) {
			if ((source >> i) & 1) c |= 1 << i; // instead of c... use output
		}
		source = source >> 7; // erase bits
		output.push_front(c);
	}

	return output;
}


unsigned int Compressor::decode(const unsigned int &source) {



	return 0;
}

std::string Compressor::encode(const std::list<int> &source) {

	return "";
}