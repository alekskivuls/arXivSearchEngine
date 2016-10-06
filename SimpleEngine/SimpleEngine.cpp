/**
 *	@Authors: Paul Kim, Aleks Kivul, Cherie Woo
 *	@Course: CECS 429 - Search Engine Technology
 */

#include <boost\algorithm\string\predicate.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost\lambda\lambda.hpp>
#include <boost\filesystem.hpp>
#include <boost\foreach.hpp>
#include <boost/chrono.hpp>
#include "PorterStemmer.h"
#include "InvertedIndex.h"
#include "SimpleEngine.h"
#include <unordered_map>
#include <unordered_set>
#include "Tokenizer.h"
#include "QEngine.h"
#include "DocInfo.h"
#include <algorithm>
#include <iostream>
#include <stdio.h>
#include <fstream>
#include <vector>
#include "Time.h"
#include <list>

/*
 * This method has a fully functional and tested inverted index. We only need to change the
 * tokenizer methods for suitable parsing (single terms, k-grams and splitting hyphenated
 * words into 3 terms). The following main has sample uses of the functions.
 *
 * At the end of the day, the terms must be properly processed (stemmed) BEFORE they are
 * inserted into the inverted index.
 *
 * IMPORTANT NOTE: the following functions need to be removed AND the getPostings method
 * must also be changed from public to private. This is to ensure data integrity. There is
 * absolutely no need for getPostings to be public. There should be higher level funcition 
 * calls that call getPostings.
 */
int main() {
	// Variable declarations
	boost::filesystem::path dir;
	std::string filepath;
	InvertedIndex *idx;
	PorterStemmer stemmer;
	QEngine queryEngine;
	std::unordered_map<unsigned int, std::string> *idTable;

	// Set file directory
	std::cout << "Enter directory of corpus" << std::endl;
	std::getline(std::cin, filepath);
	dir = boost::filesystem::path(filepath);


	// Initialization
	idTable = new std::unordered_map<unsigned int, std::string>();
	idx = new InvertedIndex();
	SimpleEngine::populateIndex(dir, stemmer, idx, idTable);
	std::cout << "idx size = " << idx->getTermCount() << '\n';

	
	// Main loop
	std::string input;
	while (true) {
		std::cout << "Enter a query:" << std::endl;
		std::getline(std::cin, input);

		if (input.compare(":q") == 0)
			break;

		else if (boost::algorithm::starts_with(input, ":stem "))
			std::cout << stemmer.stem(input.substr(6, std::string::npos)) << std::endl;

		else if (input.compare(":vocab") == 0)
			idx->vocab();

		else if (boost::algorithm::starts_with(input, ":index ")) {
			filepath = input.substr(7, std::string::npos);
			boost::filesystem::path dir(filepath);
			boost::filesystem::directory_iterator it(dir), eod;

			delete idx;
			delete idTable;
			idTable = new std::unordered_map<unsigned int, std::string>();
			idx = new InvertedIndex();
			SimpleEngine::populateIndex(dir, stemmer, idx, idTable);
			std::cout << "idx size = " << idx->getTermCount() << '\n';
		}

		else if(!boost::algorithm::starts_with(input, ":")) { //Query
			std::list<DocInfo> output = queryEngine.processQuery(input, idx); // processQuery(, const InvertedIndex &idx)
			for (auto di : output) 
				std::cout << idTable->at(di.getDocId()) << '\t';
			std::cout << std::endl << output.size() << std::endl;
			std::cout << std::endl;
		}
		else {
			std::cout << "Incorrect command usage" << std::endl;
		}
	}
	delete idx;
	delete idTable;
}