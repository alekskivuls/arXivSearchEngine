#ifndef __DISKPOSITIONALINDEX_H
#define __DISKPOSITIONALINDEX_H
#include "InvertedIndex.h"
#include <string>
#include <fstream>
#include <vector>
#include <boost/filesystem.hpp>


typedef PositionalPosting DiskPosting;

struct VocabEntry {
   uint64_t StringPosition;
   uint64_t PostingPosition;

   VocabEntry() : StringPosition(0), PostingPosition(0) {}
   VocabEntry(uint64_t str, uint64_t post) : StringPosition(str), PostingPosition(post) {}
};



class DiskPositionalIndex : public PositionalIndex {
	boost::filesystem::path mPath;
	mutable std::ifstream mVocabList;
	mutable std::ifstream mPostings;
	std::vector<VocabEntry> mVocabTable;

	static std::vector<DiskPosting> ReadPostingsFromFile(std::ifstream &postings, uint64_t postingsPosition);

	static std::vector<VocabEntry> ReadVocabTable(const boost::filesystem::path &path);

	VocabEntry BinarySearchVocabulary(const std::string &term) const;
	static DiskPosting ReadDocumentPosting(std::ifstream &postings, uint32_t lastDocId);

	std::string ReadVocabStringAtPosition(size_t index) const;

public:
	DiskPositionalIndex(const boost::filesystem::path  &path);

	std::vector<DiskPosting> GetPostings(const std::string &term) const;
}
}
}


#endif
