#ifndef TOKENIZER_H
#define TOKENIZER_H

#include "TokenStream.h"

#include <boost/filesystem.hpp>
#include <fstream>
#include <cstdint>

class Tokenizer : public TokenStream {
	std::fstream _file;
	std::string _buffer;
	bool isFileTokenizer;
	uint32_t pos;

public:
	/** variable byte and gap encoding (bytes -> .json files)*/
	Tokenizer(const boost::filesystem::path &p);
	/** parsing string by tokens into k-grams*/
	Tokenizer(const std::string &s);
	~Tokenizer();

	/** Retrieves the next token in the string or file depending on which 
	 * constructor was used. This method should be used like scanf.*/
	bool nextToken(std::string &token, bool &hyphen);

	std::string nextToken(std::string token, const int &gram);
};

#endif
