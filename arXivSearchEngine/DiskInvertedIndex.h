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

struct ListEntry {
	uint32_t StringPosition;
	uint32_t PostingPosition;

    ListEntry() : StringPosition(0), PostingPosition(0) {}
    ListEntry(uint32_t str, uint32_t post) : StringPosition(str), PostingPosition(post) {}
};


class DiskInvertedIndex {
public:
	static uint8_t ReadChar(std::ifstream &stream);
	static uint32_t ReadInt(std::ifstream &stream);
	static uint32_t ReadRawInt(std::ifstream &stream);
	static double_t ReadDouble(std::ifstream &stream);

	boost::filesystem::path mPath;
	mutable std::ifstream mVocabList;
    mutable std::ifstream mVocabPostings;
    mutable std::ifstream mAuthorList;
    mutable std::ifstream mAuthorPostings;
    std::vector<ListEntry> mVocabTable;
    std::vector<ListEntry> mAuthorTable;
	//std::vector<double_t> weights;
    std::unordered_map<uint32_t, std::string> mIdTable;
	
	static std::list<DocInfo> ReadPostingsFromFile(std::ifstream &postings, uint32_t postingsPosition); // std::vector<DocInfo>

    static std::vector<ListEntry> ReadTable(const boost::filesystem::path &path, const std::string &tableName);

	std::vector<double_t> ReadWeights();

	static std::unordered_map<uint32_t, std::string> ReadIdTableFromFile(const boost::filesystem::path &path);

    ListEntry binarySearchList(const std::string &term, std::vector<ListEntry> &table, std::ifstream &list);

    //static DocInfo ReadDocumentPosting(std::ifstream &postings, uint32_t lastDocId); // implement this after i fix the read method...

    std::string readStringAtPosition(uint32_t i, std::vector<ListEntry> &table, std::ifstream &list) const;

    std::list<uint32_t> readAuthorDocsFromFile(std::ifstream &postings, uint32_t postingsPosition);

//public:
    DiskInvertedIndex();

    DiskInvertedIndex(const boost::filesystem::path &path);

    std::list<DocInfo> GetPostings(const std::string &term);

    uint32_t getN();

    std::list<std::string> getVocabList();

    std::list<std::string> getAuthorList();

    std::list<uint32_t> getAuthorDocs(const std::string &author);
};

#endif
