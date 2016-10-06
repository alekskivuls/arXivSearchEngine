#ifndef SIMPLE_ENGINE_H
#define SIMPLE_ENGINE_H

#include "PorterStemmer.h"
#include "InvertedIndex.h"
#include "boost\filesystem\path.hpp"

class SimpleEngine {

public:
	void populateIndex(const boost::filesystem::path &dir, PorterStemmer &stemmer, InvertedIndex *& idx,
		std::unordered_map<unsigned int, std::string> *idTable);
};

#endif