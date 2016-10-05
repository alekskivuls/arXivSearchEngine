#include <boost\algorithm\string\predicate.hpp>
#include <boost\lambda\lambda.hpp>
#include <boost\filesystem.hpp>
#include <boost\foreach.hpp>
#include "InvertedIndex.h"
#include <unordered_map>
#include <unordered_set>
#include "Tokenizer.h"
#include "QEngine.h"
#include "DocInfo.h"
#include "PorterStemmer.h"
#include <algorithm>
#include <iostream>
#include <stdio.h>
#include <fstream>
#include <vector>
#include <list>

#pragma warning(disable:4996)





/*****************************************FUNCTION PROTOTYPE*****************************************/
void getPathNames(std::vector<std::string> &mPathList, const boost::filesystem::path &directory);
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
	std::string filepath;
	std::cout << "Enter directory of corpus" << std::endl;
	std::getline(std::cin, filepath);

	boost::filesystem::path dir(filepath);
	boost::filesystem::directory_iterator it(dir), eod;

	//****************************************************************************************************
	std::vector<std::string> mPathList;
	getPathNames(mPathList, dir);
	int i;
	for (i = 0; i < mPathList.size(); ++i) 
		std::cout << ".json i: " << i << " = " << mPathList[i] << '\n';
	std::cout << "Number of .json files: " << mPathList.size() << '\n';
	//****************************************************************************************************
	system("pause");

	InvertedIndex *idx = new InvertedIndex();
	QEngine queryEngine;
	PorterStemmer stemmer;

	std::string input;
	BOOST_FOREACH(boost::filesystem::path const &p, std::make_pair(it, eod))
	{
		if (boost::filesystem::is_regular_file(p))
		{
			std::fstream file;
			file.open(p.string(), std::fstream::in | std::fstream::out | std::fstream::app);
			std::string input;
			int posIndex = 0;
			while (std::getline(file, input)) {
				std::transform(input.begin(), input.end(), input.begin(), ::tolower);
				Tokenizer tkzr(input);
				
				std::string token;
				while (tkzr.nextToken(token)) {// while not end of file.
					idx->addTerm(stemmer.stem(token), p.string(), posIndex);
					posIndex++;
				}
			}
			file.close();
		}
	}

	std::cout << "idx size = " << idx->getTermCount() << '\n';

	while (true) {
		std::cout << "Enter a query:" << std::endl;
		std::getline(std::cin, input);
		if (input.compare(":q") == 0) {
			break;
		}
		else if (input.substr(0, 6).compare(":stem ") == 0) {
			std::cout << stemmer.stem(input.substr(6, std::string::npos));
		}
		else if (input.compare(":vocab") == 0) {
			idx->printIndex(); //Not quite right
		}
		else if (input.substr(0, 7).compare(":index ") == 0) {
			filepath = input.substr(7, std::string::npos);
			boost::filesystem::path dir(filepath);
			boost::filesystem::directory_iterator it(dir), eod;

			//NEED TO EMPTY AND REST INDEX
			//InvertedIndex idx;
			//QEngine queryEngine(idx);

			// 

			BOOST_FOREACH(boost::filesystem::path const &p, std::make_pair(it, eod))
			{
				if (boost::filesystem::is_regular_file(p))
				{
					std::fstream file;
					file.open(p.string(), std::fstream::in | std::fstream::out | std::fstream::app);
					std::string input;
					int posIndex = 0;
					while (std::getline(file, input)) {
						Tokenizer tkzr(input);
						std::string token;
						while (tkzr.nextToken(token)) {// while not end of file.
							idx->addTerm(stemmer.stem(token), p.string(), posIndex);
							posIndex++;
						}
					}
					file.close();
				}
			}
		}
		else { //Query
			std::list<DocInfo> output = queryEngine.processQuery(input, idx); // processQuery(, const InvertedIndex &idx)
			for (auto di : output) {
				std::cout << di.getDocName() << ":\n";
				for (auto i : di.getPositions()) {
					std::cout << i << " ";
				}
				std::cout << std::endl;
			}
		}
	}

	delete idx; // remove this later
}

void getPathNames(std::vector<std::string> &mPathList, const boost::filesystem::path &directory) {
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


/*
void getFileNames(std::vector<std::string> &mPathList, std::vector<std::string> &mFileList) {
	//boost::filesystem::path p("c:/dir/dir/file.ext");
	//std::cout << "filename and extension : " << p.filename() << std::endl; // file.ext
	//std::cout << "filename only          : " << p.stem() << std::endl;     // file
	boost::filesystem::path p("C:/Users/Paul Kim/Documents/Visual Studio 2015/Projects/NaiveIndex/NaiveIndex");
	boost::filesystem::directory_iterator end_itr;

	std::unordered_set<std::string> fileSet;

	for (boost::filesystem::directory_iterator itr(p); itr != end_itr; ++itr) {
		if (is_regular_file(itr->path())) {
			std::string s = itr->path().string();
			if (boost::algorithm::ends_with(s, ".txt")) {
				std::replace(s.begin(), s.end(), '\\', '/');
				fileSet.insert(s);
			}

		}
	}

	mFileList.resize(fileSet.size());
	mPathList.resize(fileSet.size());
	int i = 0;
	for (auto s : fileSet) {


		boost::filesystem::path p(s);
		mPathList[i] = s;
		mFileList[i++] = p.filename().string();
	}
}

*/