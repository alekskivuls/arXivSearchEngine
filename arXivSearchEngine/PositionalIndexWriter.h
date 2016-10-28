#ifndef __POSITIONALINDEXWRITER_H
#define __POSITIONALINDEXWRITER_H

#include <boost/filesystem.hpp>
#include <vector>
#include <string>
#include <fstream>

// NOTE TO STUDENTS:

// Uncomment and modify in the typedef that follows. This code uses the alias "PostingList" as the type
// of a vector that contains positional postings. If your "postings lists" are of a different type/class,
// then change the typedef.

// typedef std::vector<PositionalPosting> PostingList;


class PositionalIndexWriter {
	PositionalInvertedIndex mIndex;

	void BuildPostingsFile(const boost::filesystem::path& path,
		const std::vector<std::string> &vocab, const std::vector<uint64_t> &vocabPositions);

public:
	// You must std::move an index to "hand it over" to this writer.
	PositionalIndexWriter(PositionalInvertedIndex &&index) : mIndex(index) {}

	void WriteIndex(const boost::filesystem::path& path);
	static void WritePostings(std::ofstream &file, const PostingList &postings);

	static std::vector<uint64_t> BuildVocabFile(const boost::filesystem::path& path,
		const std::vector<std::string> &vocab);

};
#endif