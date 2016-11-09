#ifndef DISK_INDEX_H
#define DISK_INDEX_H


#include "InvertedIndex.h"
#include "DocInfo.h"
#include <cstdint>
#include <string>
#include <fstream>
#include <vector>
#include <boost/filesystem.hpp>

struct VocabEntry {
	uint64_t StringPosition;
	uint64_t PostingPosition;

	VocabEntry() : StringPosition(0), PostingPosition(0) {}
	VocabEntry(uint64_t str, uint64_t post) : StringPosition(str), PostingPosition(post) {}
};



class DiskInvertedIndex : public InvertedIndex {
	boost::filesystem::path mPath;
	mutable std::ifstream mVocabList;
	mutable std::ifstream mPostings;
	std::vector<VocabEntry> mVocabTable;

	static std::vector<DocInfo> ReadPostingsFromFile(std::ifstream &postings, uint64_t postingsPosition);

	static std::vector<VocabEntry> ReadVocabTable(const boost::filesystem::path &path);

	VocabEntry BinarySearchVocabulary(const std::string &term) const;
	static DocInfo ReadDocumentPosting(std::ifstream &postings, uint32_t lastDocId);

	std::string ReadVocabStringAtPosition(size_t index) const;

public:
	DiskInvertedIndex(const boost::filesystem::path  &path);

	std::vector<DocInfo> GetPostings(const std::string &term) const;
};

#endif