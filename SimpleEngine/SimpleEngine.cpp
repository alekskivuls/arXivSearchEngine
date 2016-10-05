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
#include "PorterStemmer.h"
#include "InvertedIndex.h"
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
#include <list>

#pragma warning(disable:4996)


/*****************************************FUNCTION PROTOTYPE*****************************************/
void populateIndex(const boost::filesystem::path &dir, PorterStemmer &stemmer, InvertedIndex *& idx);

void getPathNames(const boost::filesystem::path &directory, std::vector<std::string> &mPathList);
/****************************************************************************************************/


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


	// Set file directory
	std::cout << "Enter directory of corpus" << std::endl;
	std::getline(std::cin, filepath);
	dir = boost::filesystem::path(filepath);


	// Initialization
	idx = new InvertedIndex();
	populateIndex(dir, stemmer, idx);
	std::cout << "idx size = " << idx->getTermCount() << '\n';

	
	// Main loop
	std::string input;
	while (true) {
		std::cout << "Enter a query:" << std::endl;
		std::getline(std::cin, input);

		if (input.compare(":q") == 0) 
			break;

		else if (input.substr(0, 6).compare(":stem ") == 0) 
			std::cout << stemmer.stem(input.substr(6, std::string::npos));

		else if (input.compare(":vocab") == 0) 
			idx->vocab();

		else if (input.substr(0, 7).compare(":index ") == 0) {
			filepath = input.substr(7, std::string::npos);
			boost::filesystem::path dir(filepath);
			boost::filesystem::directory_iterator it(dir), eod;

			std::cout << "Enter directory of corpus" << std::endl;
			std::getline(std::cin, filepath);
			dir = boost::filesystem::path(filepath);

			delete idx;
			idx = new InvertedIndex();
			populateIndex(dir, stemmer, idx);
			std::cout << "idx size = " << idx->getTermCount() << '\n';
		}

		else { //Query
			try {
				std::list<DocInfo> output = queryEngine.processQuery(input, idx); // processQuery(, const InvertedIndex &idx)
				for (auto di : output)
					std::cout << di.getDocName() << ' ';
				std::cout << std::endl;
			} 
			catch (std::out_of_range e) {
				std::cout << e.what() << std::endl;
			}
		}
	}

	delete idx;
}

/*
 * Takes a Porter Stemmer and a directory full of .json files to populate an inverted index that 
 * is allocated on the heap. This method feeds the text file into a stringstream and passes the 
 * stringstream into a boost, json property tree. The tokens are individually transformed to 
 * lowercase and stemmed before being put into the inverted index. 
 */
void populateIndex(const boost::filesystem::path &dir, PorterStemmer &stemmer, InvertedIndex *& idx) {
	clock_t start = clock();

	std::unordered_map<std::string, std::string> cache;
	boost::filesystem::directory_iterator it(dir), eod;
	std::vector<std::string> mPathList;
	getPathNames(dir, mPathList);
	
	for (auto p : mPathList) {
		// reads json file into stringstream and populates a json tree
		std::ifstream file(p);
		std::stringstream ss;
		ss << file.rdbuf();
		file.close();

		boost::property_tree::ptree pt;
		boost::property_tree::read_json(ss, pt);

		// iterate through .json tree
		BOOST_FOREACH(boost::property_tree::ptree::value_type& pair, pt) {
			if (pair.first == "body" || pair.first == "title") { // if author... get json array and process the authors as well.
				std::string input = pair.second.get_value<std::string>();
				std::transform(input.begin(), input.end(), input.begin(), ::tolower);
				
				boost::filesystem::path dir(p);
				Tokenizer tkzr(input);
				std::string token;
				int posIndex = 0;
				
				while (tkzr.nextToken(token)) {// while not end of file.
					// Get stem the token or retrieve the value from a cache
					std::string stemmedToken = (cache.find(token) != cache.end()) 
						? cache[token] : stemmer.stem(token);

					idx->addTerm(stemmedToken, dir.stem().string(), posIndex);
					posIndex++;
				}
			}
		}
	}

	clock_t finish = clock();
	double elapsedTime = ((finish - start) / 1000000.0);
	std::cout << "Elapsed Time: " << elapsedTime << " ms." << std::endl;
}

/*
 * This method goes to a path and walks through the directory searching for all files that end 
 * with the .json extension. The method will put all full file paths into a vector that is passed 
 * by reference.
 */
void getPathNames(const boost::filesystem::path &directory, std::vector<std::string> &mPathList) {
	boost::filesystem::directory_iterator end_itr;
	std::unordered_set<std::string> fileSet;

	for (boost::filesystem::directory_iterator itr(directory); itr != end_itr; ++itr) {
		if (is_regular_file(itr->path())) {
			std::string s = itr->path().string();
			if (boost::algorithm::ends_with(s, ".json")) {
				std::replace(s.begin(), s.end(), '\\', '/');
				//std::cout << s << '\n';
				fileSet.insert(s);
			}
		}
	}

	mPathList.resize(fileSet.size());
	int i = 0;
	for (auto s : fileSet) 
		mPathList[i++] = s;
}
