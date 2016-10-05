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
	//Get folder to parse documents of
	boost::filesystem::path dir;
	std::string filepath;

	InvertedIndex *idx;
	PorterStemmer stemmer;
	QEngine queryEngine;

	std::cout << "Enter directory of corpus" << std::endl;
	std::getline(std::cin, filepath);
	dir = boost::filesystem::path(filepath);
	system("pause");


	// calls new... make sure you properly deallocate memory space from the heap
	idx = new InvertedIndex();
	populateIndex(dir, stemmer, idx);
	std::cout << "idx size = " << idx->getTermCount() << '\n';

	
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
			std::list<DocInfo> output = queryEngine.processQuery(input, idx); // processQuery(, const InvertedIndex &idx)
			for (auto di : output) {
				std::cout << di.getDocName() << ":\n";
				for (auto i : di.getPositions()) 
					std::cout << i << " ";
				std::cout << std::endl;
			}
		}
	}

	delete idx;
}

void populateIndex(const boost::filesystem::path &dir, PorterStemmer &stemmer, InvertedIndex *&idx) {
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
			if (pair.first == "body") {
				std::string input = pair.second.get_value<std::string>();
				std::transform(input.begin(), input.end(), input.begin(), ::tolower);
				std::cout << "body = " << input << std::endl;
				
				boost::filesystem::path dir(p);
				Tokenizer tkzr(input);
				std::string token;
				int posIndex = 0;
				
				while (tkzr.nextToken(token)) {// while not end of file.
					idx->addTerm(stemmer.stem(token), dir.stem().string(), posIndex);
					posIndex++;
				}
			}
		}
	}
}

void getPathNames(const boost::filesystem::path &directory, std::vector<std::string> &mPathList) {
	boost::filesystem::directory_iterator end_itr;
	std::unordered_set<std::string> fileSet;

	for (boost::filesystem::directory_iterator itr(directory); itr != end_itr; ++itr) {
		if (is_regular_file(itr->path())) {
			std::string s = itr->path().string();
			if (boost::algorithm::ends_with(s, ".json")) {
				std::replace(s.begin(), s.end(), '\\', '/');
				std::cout << s << '\n';
				fileSet.insert(s);
			}
		}
	}

	mPathList.resize(fileSet.size());
	int i = 0;
	for (auto s : fileSet) 
		mPathList[i++] = s;
}
