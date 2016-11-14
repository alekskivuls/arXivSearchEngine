#ifndef DISK_INDEX_H
#define DISK_INDEX_H


#include "InvertedIndex.h"
#include <boost/filesystem.hpp>
#include "DocInfo.h"
#include <cstdint>
#include <string>
#include <fstream>
#include <list>
#include <vector> // argue with Neal about this..?


struct VocabEntry {
	uint64_t StringPosition;
	uint64_t PostingPosition;

	VocabEntry() : StringPosition(0), PostingPosition(0) {}
	VocabEntry(uint64_t str, uint64_t post) : StringPosition(str), PostingPosition(post) {}
};



class DiskInvertedIndex : public InvertedIndex {
public:
	static uint32_t ReadInt(std::ifstream &stream);
	static uint64_t ReadInt64(std::ifstream &stream);

	boost::filesystem::path mPath;
	mutable std::ifstream mVocabList;
	mutable std::ifstream mPostings;
	std::vector<VocabEntry> mVocabTable;
	
	static std::list<DocInfo> ReadPostingsFromFile(std::ifstream &postings, uint64_t postingsPosition); // std::vector<DocInfo>

	static std::vector<VocabEntry> ReadVocabTable(const boost::filesystem::path &path);

	VocabEntry BinarySearchVocabulary(const std::string &term) const;
	static DocInfo ReadDocumentPosting(std::ifstream &postings, uint32_t lastDocId); // implement this after i fix the read method...


	std::string ReadVocabStringAtPosition(size_t index) const;

//public:
	DiskInvertedIndex(const boost::filesystem::path  &path);

	std::list<DocInfo> GetPostings(const std::string &term) const; // std::vector<DocInfo>
};

#endif