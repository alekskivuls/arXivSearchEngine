#ifndef ENGINE_H
#define ENGINE_H

#include "PorterStemmer.h"
#include "InvertedIndex.h"

#include <boost/filesystem.hpp>
#include "PorterStemmer.h"
#include "InvertedIndex.h"
#include <unordered_map>
#include "Tokenizer.h"
#include "QEngine.h"
#include "DocInfo.h"
#include <algorithm>
#include <iostream>
#include <cstdint>
#include <string>
#include <vector>
#include <chrono>
#include <cmath>
#include <list>

#include "KEngine.h"
#include "KgramIndex.h"
#include "DiskInvertedIndex.h"

class Engine {
    boost::filesystem::path dir;
	InvertedIndex idx;
	QEngine queryEngine;
    std::unordered_map<uint32_t, std::string> idTable;
	std::vector<double_t> ld; // DOCUMENT ID, SCORE

    KgramIndex kInd1 = KgramIndex(1);
    KgramIndex kInd2 = KgramIndex(2);
    KgramIndex kInd3 = KgramIndex(3);
    //DiskInvertedIndex dIdx;

public:
	Engine();
	
	/**
	* Takes a Porter Stemmer and a directory full of .json files to populate an inverted index that
	* is allocated on the heap. This method feeds the text file into a stringstream and passes the
	* stringstream into a boost, json property tree. The tokens are individually transformed to
	* lowercase and stemmed before being put into the inverted index.
	*/
    void populateIndex(const boost::filesystem::path &inDir, const boost::filesystem::path &outDir);

	void populateEucDist();

	/**
	* This method goes to a path and walks through the directory searching for all files that end
	* with the .json extension. The method will put all full file paths into a vector that is passed
	* by reference.
	*/
	void getPathNames(const boost::filesystem::path &directory, std::vector<std::string> &mPathList);

    void updateTf(std::unordered_map<std::string, uint32_t> &wdt, const std::string & term);

	double_t calcEucDist(std::unordered_map<std::string, uint32_t>& wdt);

	void index(const std::string &filepath);

    void createIndex(const std::string &filepath);

    void loadIndex(const std::string &filepath);

	void diskWriteTest(const std::string &filepath);

	void printVocab();

    std::list<std::string> getVocab();

	void printIndex();

	void printQuery(std::string &query);

    std::vector<std::string> getQuery(std::string &query);

    std::string stem(std::string &token);
};

#endif
