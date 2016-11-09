#include "Serializer.h"
#include <iostream>

// For correcting endianness issues; you may not need these.
inline uint32_t Reverse(uint32_t value) {
	return (value & 0xFF000000) >> 24 |
		(value & 0x00FF0000) >> 8 |
		(value & 0x0000FF00) << 8 |
		(value & 0x000000FF) << 24;
}

inline uint64_t Reverse(uint64_t value) {
	return (value & 0xFF00000000000000) >> 56 |
		(value & 0x00FF000000000000) >> 40 |
		(value & 0x0000FF0000000000) >> 24 |
		(value & 0x000000FF00000000) >> 8 |
		(value & 0x00000000FF000000) << 8 |
		(value & 0x0000000000FF0000) << 24 |
		(value & 0x000000000000FF00) << 40 |
		(value & 0x00000000000000FF) << 56;
}

// at this point, "index" contains the in-memory inverted index 
// now we save the index to disk, building three files: the postings index,
// the vocabulary list, and the vocabulary table.
void Serializer::buildIndex(const boost::filesystem::path &filePath, const InvertedIndex &auxIdx) {
	// do i really need an array of terms? why not just iterate the hashmap with an advanced for loop?

    std::vector<uint64_t> vocabPositions = Serializer::buildVocab(filePath, auxIdx);
	Serializer::buildPostings(filePath, auxIdx, vocabPositions);
}

void Serializer::buildPostings(const boost::filesystem::path &filePath, const InvertedIndex &auxIdx,
	const std::vector<uint64_t> &vocabPositions) {

	boost::filesystem::path postingsPath = filePath, tablePath = filePath;

	// simultaneously build the vocabulary table on disk, mapping a term index to a 
	// file location in the postings file.
	std::ofstream postingsFile(postingsPath.append("/postings.bin").string(),
		std::ios::out | std::ios::binary);
	std::ofstream vocabTable(tablePath.append("/vocabTable.bin").string(),
		std::ios::out | std::ios::binary);

	// the first thing we must write to the vocabTable file is the number of vocab terms.
	int64_t termCount = Reverse(auxIdx.getIndex().size());// UNCOMMENT FOR LINUX
	vocabTable.write((const char*)&termCount, sizeof(termCount));


	std::cout << "num terms: " << auxIdx.getIndex().size() << std::endl;
	int vocabIndex = 0; // FOR EVERY TERM
	typedef const std::pair<std::string, std::list<DocInfo>> pair;
	for (pair &element : auxIdx.getIndex()) {
		//std::cout << "term " << element.first << std::endl;
		const std::list<DocInfo> &postings = auxIdx.getPostings(element.first);

		// write the vocab table entry for this term: the byte location of the term in the vocab list file,
		// and the byte location of the postings for the term in the postings file.

		uint64_t vPosition = Reverse(vocabPositions[vocabIndex]);// UNCOMMENT FOR LINUX
		vocabTable.write((const char*)&vPosition, sizeof(vPosition));
		uint64_t pPosition = Reverse((uint64_t)postingsFile.tellp());// UNCOMMENT FOR LINUX
		vocabTable.write((const char*)&pPosition, sizeof(pPosition));

		// write the postings file for this term. 
		Serializer::WritePostings(postingsFile, postings); // define later...

		vocabIndex++;
	}

	vocabTable.close();
	postingsFile.close();
}


// Writes a single list of postings for one vocabulary term to the given postings file.
// Currently writes only the document frequency (size of list) and each document ID as a gap.
void Serializer::WritePostings(std::ofstream &postingsFile, const std::list<DocInfo> &postings) { //const PostingList &postings
	// positionalposting: pair (documentID, a vector of positions (uint32_t))
	// Write the document frequency.
	std::size_t docFreq = Reverse(postings.size());// UNCOMMENT FOR LINUX
	postingsFile.write((const char*)&docFreq, sizeof(docFreq));

	//std::cout << "doc gaps: ";
	uint32_t lastDocId = 0;
	for (const DocInfo &currDoc : postings) {
		// write document ID gap.
		// Uses Reverse to fix endianness issues on Windows. If not building on Windows, you may
		// want to remove the Reverse calls.
		uint32_t docIdGap = Reverse(currDoc.getDocId() - lastDocId); // UNCOMMENT FOR LINUX
		//std::cout << docIdGap << ' ';

		postingsFile.write((const char*)&docIdGap, sizeof(docIdGap));
		lastDocId = currDoc.getDocId();
	}
	//std::cout << std::endl;
}


std::vector<uint64_t> Serializer::buildVocab(const boost::filesystem::path &filePath, const InvertedIndex &auxIdx) {
	// first build the vocabulary list: a file of each vocab word concatenated together.
	// also build an array associating each term with its byte location in this file.
	std::vector<uint64_t> positions(auxIdx.getIndex().size());
	int vocabIndex = 0;
	int vocabPos = 0;

	boost::filesystem::path vocabPath = filePath;
	std::ofstream vocabFile(vocabPath.append("/vocabList.bin").string(), 
		std::ios::out | std::ios::binary);

	//std::cout << "PRINTING TERM?" << std::endl;
	typedef const std::pair<std::string, std::list<DocInfo>> pair;
	for (pair &element : auxIdx.getIndex()) {
		positions[vocabIndex] = vocabFile.tellp();
		//std::cout << element.first.c_str() << std::endl;
		vocabFile.write(element.first.c_str(), sizeof(char) * element.first.length());
		vocabIndex++;
	}

	vocabFile.close();
	return positions;
}
