#include "PositionalIndexWriter.h"

using std::ofstream;
using std::vector;

namespace fs = boost::filesystem;

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

inline double_t Reverse(double_t value) {
	auto result = Reverse(*reinterpret_cast<uint64_t*>(&value));
	return *reinterpret_cast<double_t*>(&result);
}

void PositionalIndexWriter::WriteIndex(const fs::path& path) {
	auto vocab = mIndex.GetVocabulary();
	vector<uint64_t> vocabPositions = BuildVocabFile(path, vocab);
	BuildPostingsFile(path, vocab, vocabPositions);
}

// Writes a single list of postings for one vocabulary term to the given postings file.
// Currently writes only the document frequency (size of list) and each document ID as a gap.
void PositionalIndexWriter::WritePostings(std::ofstream &postingsFile, const PostingList &postings) {
	// positionalposting: pair (documentID, a vector of positions (uint32_t))

	// Write the document frequency.
	std::size_t docFreq = Reverse(postings.size());
	postingsFile.write((const char*)&docFreq, sizeof(docFreq));

	uint32_t lastDocId = 0;
	for (const PositionalPosting &positional : postings) {
		// write document ID gap.
		// Uses Reverse to fix endianness issues on Windows. If not building on Windows, you may
		// want to remove the Reverse calls.
		uint32_t docIdGap = Reverse(positional.GetDocumentID() - lastDocId);
		postingsFile.write((const char*)&docIdGap, sizeof(docIdGap));

		lastDocId = positional.GetDocumentID();
	}
}

vector<uint64_t> PositionalIndexWriter::BuildVocabFile(const fs::path& path,
	const vector<std::string> &vocab) {

	// first build the vocabulary list: a file of each vocab word concatenated together.
	// also build an array associating each term with its byte location in this file.
	vector<uint64_t> positions(vocab.size());
	int vocabIndex = 0;
	int vocabPos = 0;
	fs::path vocabPath = path;
	ofstream vocabList(vocabPath.append("/vocabList.bin").string(),
		std::ios::out | std::ios::binary);

	
	for (const std::string &vocabWord : vocab) {
		// for each string in dictionary, save the byte position where that term will start in the vocab file.
		positions[vocabIndex] = vocabList.tellp();
		vocabList.write(vocabWord.c_str(), sizeof(char) * vocabWord.length());
		vocabIndex++;
	}
	vocabList.close();
	return positions;
}

void PositionalIndexWriter::BuildPostingsFile(const fs::path& path,
	const vector<std::string> &vocab, const vector<uint64_t> &vocabPositions) {
	// simultaneously build the vocabulary table on disk, mapping a term index to a 
	// file location in the postings file.
	fs::path postingsPath = path, tablePath = path;

	ofstream postingsFile(postingsPath.append("/postings.bin").string(),
		std::ios::out | std::ios::binary);
	ofstream vocabTable(tablePath.append("/vocabTable.bin").string(),
		std::ios::out | std::ios::binary);

	// the first thing we must write to the vocabTable file is the number of vocab terms.
	int64_t termCount = Reverse(vocab.size());
	vocabTable.write((const char*)&termCount, sizeof(termCount));

	int vocabIndex = 0;
	for (const std::string &s : vocab) {
		auto postings = mIndex.GetPostings(s);

		// write the vocab table entry for this term: the byte location of the term in the vocab list file,
		// and the byte location of the postings for the term in the postings file.
		uint64_t vPosition = Reverse(vocabPositions[vocabIndex]);
		vocabTable.write((const char*)&vPosition, sizeof(vPosition));
		uint64_t pPosition = Reverse((uint64_t)postingsFile.tellp());
		vocabTable.write((const char*)&pPosition, sizeof(pPosition));

		// write the postings file for this term. 
		WritePostings(postingsFile, postings);

		vocabIndex++;
	}

	vocabTable.close();
	postingsFile.close();
}


