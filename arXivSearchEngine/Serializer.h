#ifndef SERIALIZER_H
#define SERIALIZER_H

#include "InvertedIndex.h"
#include <boost/filesystem.hpp>
#include <fstream>
#include <vector>

// does not need to be an object. This should be a utility class 
// that is resposible for serializing and nothing else.
class Serializer { 
public:

	// this method MUST pass in a populated auxilary index ready to encode (vbe & gap) AND 
	static void buildIndex(const boost::filesystem::path &filePath, InvertedIndex &auxIdx);

	static std::vector<uint64_t> buildVocab(const boost::filesystem::path &filePath, const InvertedIndex &auxIdx);

	static void buildPostings(const boost::filesystem::path &filePath, const InvertedIndex &auxIdx, 
		const std::vector<uint64_t> &vocabPositions);

    static void WritePostings(std::ofstream &postingsFile, const std::list<DocInfo> &postings);
};


#endif
