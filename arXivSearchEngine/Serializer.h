#ifndef SERIALIZER_H
#define SERIALIZER_H

#include "InvertedIndex.h"
#include <boost/filesystem.hpp>
#include <cstdint>
#include <fstream>
#include <vector>

typedef double double_t;

// does not need to be an object. This should be a utility class 
// that is resposible for serializing and nothing else.
class Serializer { 
public:

	// this method MUST pass in a populated auxilary index ready to encode (vbe & gap) AND 
	static void buildIndex(const boost::filesystem::path &filePath, const InvertedIndex &auxIdx,
		const std::unordered_map<uint32_t, std::string> &idTable, const  std::vector<double_t> &weights);

	static std::vector<uint32_t> buildVocab(const boost::filesystem::path &filePath, const InvertedIndex &auxIdx);

	static void buildIdTable(const boost::filesystem::path &filePath, const std::unordered_map<uint32_t, std::string> &idTable);

	static void buildPostings(const boost::filesystem::path &filePath, const InvertedIndex &auxIdx, 
		const std::vector<uint32_t> &vocabPositions);

    static void WritePostings(std::ofstream &postingsFile, const std::list<DocInfo> &postings);

	static void buildEucDist(const boost::filesystem::path &filePath, const std::vector<double_t> &weights);
};

#endif
