#ifndef ENGINE_H
#define ENGINE_H

#include "PorterStemmer.h"
#include "InvertedIndex.h"

#include <boost/filesystem.hpp>
#include "PorterStemmer.h"
#include "InvertedIndex.h"
#include <unordered_map>
#include "WildEngine.h"
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

#include "KSerializer.h"
#include "KEngine.h"
#include "KgramIndex.h"
#include "DiskInvertedIndex.h"

class Engine {

	QEngine queryEngine;
    std::unordered_map<uint32_t, std::string> idTable;
	std::vector<double_t> ld; // DOCUMENT ID, SCORE

    KgramIndex kIdx1 = KgramIndex(1);
    KgramIndex kIdx2 = KgramIndex(2);
    KgramIndex kIdx3 = KgramIndex(3);

public:
	Engine();
	
	void rocchio();

    DiskInvertedIndex dIdx;

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

	std::string getArticleName(const uint32_t &docid);

    void createIndex(const boost::filesystem::path &filepath);

    void createIndex(const boost::filesystem::path &inDir, const boost::filesystem::path &outDir);

    void loadIndex(const boost::filesystem::path &filepath);

    std::vector<std::pair<uint32_t, double_t>> getRank(std::string &query);

    void printRank(std::string &query);

	void printVocab();

    std::list<std::string> getVocab();

    //void printIndex();

	void printQuery(std::string &query);

    std::vector<std::string> getQuery(std::string &query);

    std::string stem(std::string &token);

    std::list<std::string> getAuthors();

    void printAuthors();

    std::list<DocInfo> getAuthorDocs(const std::string &author);

    void printAuthorDocs(const std::string &author);

    void classifyDocuments(const uint32_t numFeatures);

    void classifyFederalistDocuments(const uint32_t numFeatures);
};

#endif
