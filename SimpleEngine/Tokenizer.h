#ifndef TOKENIZER_H
#define TOKENIZER_H

#include "TokenStream.h"

#include <boost\filesystem.hpp>
#include <fstream>

class Tokenizer : public TokenStream {
	std::fstream _file;
	std::string _buffer;
	bool isFileTokenizer;
	unsigned int pos;

public:
	// variable byte and gap encoding (bytes -> .json files)
	Tokenizer(const boost::filesystem::path &p);
	// parsing string by tokens into k-grams
	Tokenizer(const std::string &s);
	~Tokenizer();

	bool nextToken(std::string &token);

	std::string nextToken(std::string token, const int &gram);
};

// ignore all of this... these are side comments for future methods that may be used for 
// implementation of k gram tokenizer



//file.clear();
//file.seekg(0, std::ios::end);
//long size = file.tellg();

//moves "get" pointer of file reader to beginning of next graycode
//file.seekg(j);
//prints to the end of the file
//std::getline(file, line);
//moves "put" pointer of file reader to the end of the file
//file.seekp(0, std::ios::end);


#endif