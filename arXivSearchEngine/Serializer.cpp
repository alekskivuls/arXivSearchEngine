#include "Serializer.h"
#include <iomanip>

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
	std::vector<uint64_t> vocabPositions = Serializer::buildVocab(filePath, auxIdx);
	Serializer::buildPostings(filePath, auxIdx, vocabPositions);
}

void Serializer::buildPostings(const boost::filesystem::path &filePath, const InvertedIndex &auxIdx,
	const std::vector<uint64_t> &vocabPositions) {

	boost::filesystem::path postingsPath = filePath, tablePath = filePath;

	// simultaneously build the vocabulary table on disk, mapping a term index to a 
	// file location in the postings file.
    std::ofstream postingsFile(postingsPath.append("/postings.bin", boost::filesystem::path::codecvt()).string(),
		std::ios::out | std::ios::binary);
    std::ofstream vocabTable(tablePath.append("/vocabTable.bin", boost::filesystem::path::codecvt()).string(),
		std::ios::out | std::ios::binary);

	// the first thing we must write to the vocabTable file is the number of vocab terms.
	int64_t termCount = Reverse(auxIdx.getVocabList().size()); // UNCOMMENT FOR WINDOWS

	vocabTable.write((const char*)&termCount, sizeof(termCount));

	int64_t vocabIndex = 0; // FOR EVERY TERM
	for (const std::string &term : auxIdx.getVocabList()) {
		const std::list<DocInfo> &postings = auxIdx.getPostings(term);

		// write the vocab table entry for this term: the byte location of the term in the vocab list file,
		// and the byte location of the postings for the term in the postings file.
		uint64_t vPosition = Reverse(vocabPositions[vocabIndex]);
		vocabTable.write((const char*)&vPosition, sizeof(vPosition));

		uint64_t pPosition = Reverse((uint64_t)postingsFile.tellp());
		vocabTable.write((const char*)&pPosition, sizeof(pPosition));
		
		WritePostings(postingsFile, postings);
		
		vocabIndex++;
	}


	vocabTable.close();
	postingsFile.close();
}

/**
  * Writes a single list of postings for one vocabulary term to the given postings file.
  * Currently writes only the document frequency (size of list) and each document ID as a gap.
  */


void Serializer::WritePostings(std::ofstream &postingsFile, const std::list<DocInfo> &postings) { 
	// Write the document frequency.
	size_t docFreq = (uint64_t) Reverse(postings.size());
	postingsFile.write((const char*)&docFreq, sizeof(docFreq));

	uint32_t lastDocId = 0;
	for (const DocInfo &currDoc : postings) {
		// write document ID gap.
		// Uses Reverse to fix endianness issues on Windows. If not building on Windows, you may
		// want to remove the Reverse calls.
		uint32_t docIdGap = Reverse(currDoc.getDocId() - lastDocId);
		postingsFile.write((const char*)&docIdGap, sizeof(docIdGap)); // variable byte encode this

		size_t positionSize = (uint64_t)Reverse(currDoc.getPositions().size());
		postingsFile.write((const char*)&positionSize, sizeof(positionSize)); // write to disk size of position list

		uint32_t lastPosGap = 0;
		for (const uint32_t &pos : currDoc.getPositions()) {
			uint32_t posGap = Reverse(pos - lastPosGap);
			postingsFile.write((const char*)&posGap, sizeof(posGap));

            lastPosGap = pos;
		}

		lastDocId = (uint32_t)currDoc.getDocId();
	}
}

std::vector<uint64_t> Serializer::buildVocab(const boost::filesystem::path &filePath, const InvertedIndex &auxIdx) {
	// first build the vocabulary list: a file of each vocab word concatenated together.
	// also build an array associating each term with its byte location in this file.
	std::vector<uint64_t> positions(auxIdx.getVocabList().size());
	int vocabIndex = 0;
	int vocabPos = 0;

	boost::filesystem::path vocabPath = filePath;
    std::ofstream vocabFile(vocabPath.append("/vocabList.bin", boost::filesystem::path::codecvt()).string(),
		std::ios::out | std::ios::binary);

	for (const std::string &term : auxIdx.getVocabList()) {
		positions[vocabIndex] = vocabFile.tellp();
		vocabFile.write(term.c_str(), (sizeof(char) * term.length()));
		vocabIndex++;
	}

	vocabFile.close();
	return positions;
}
