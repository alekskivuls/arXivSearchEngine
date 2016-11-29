#ifndef DISK_INDEX_H
#define DISK_INDEX_H

#include <boost/filesystem.hpp>
#include <unordered_map>
#include "DocInfo.h"
#include <cstdint>
#include <string>
#include <fstream>
#include <list>
#include <vector>


typedef double double_t;

struct VocabEntry {
	uint32_t StringPosition;
	uint32_t PostingPosition;

	VocabEntry() : StringPosition(0), PostingPosition(0) {}
	VocabEntry(uint32_t str, uint32_t post) : StringPosition(str), PostingPosition(post) {}
};


class DiskInvertedIndex {
public:
	static uint8_t ReadChar(std::ifstream &stream);
	static uint32_t ReadInt(std::ifstream &stream);
	static uint32_t ReadRawInt(std::ifstream &stream);
	static double_t ReadDouble(std::ifstream &stream);

	boost::filesystem::path mPath;
	mutable std::ifstream mVocabList;
	mutable std::ifstream mPostings;
	std::vector<VocabEntry> mVocabTable;
	//std::vector<double_t> weights;
    std::unordered_map<uint32_t, std::string> mIdTable;
	
	static std::list<DocInfo> ReadPostingsFromFile(std::ifstream &postings, uint32_t postingsPosition); // std::vector<DocInfo>

	static std::vector<VocabEntry> ReadVocabTable(const boost::filesystem::path &path);

	std::vector<double_t> ReadWeights();

	static std::unordered_map<uint32_t, std::string> ReadIdTableFromFile(const boost::filesystem::path &path);

	VocabEntry BinarySearchVocabulary(const std::string &term) const;

    //static DocInfo ReadDocumentPosting(std::ifstream &postings, uint32_t lastDocId); // implement this after i fix the read method...

	std::string ReadVocabStringAtPosition(uint32_t index) const;

//public:
    DiskInvertedIndex(const boost::filesystem::path &path);

    std::list<DocInfo> GetPostings(const std::string &term) const;

    uint32_t getN();

    std::list<std::string> getVocabList();
};

#endif
