#include "DiskInvertedIndex.h"
#include <sstream>
#include <iostream>

using std::ifstream;
namespace fs = boost::filesystem;

extern uint32_t Reverse(uint32_t);
extern uint64_t Reverse(uint64_t);
extern double_t Reverse(double_t);


uint32_t DiskInvertedIndex::ReadInt(std::ifstream &stream) {
	uint32_t value = 0;
	stream.read((char*)&value, sizeof(value));
	return Reverse(value); // UNCOMMENT FOR WINDDOWS
}

double_t DiskInvertedIndex::ReadDouble(std::ifstream &stream) {
	double_t value = 0;
	stream.read((char*)&value, sizeof(value));
	return Reverse(value); // UNCOMMENT FOR WINDDOWS
}

/*uint32_t DiskInvertedIndex::ReadInt64(std::ifstream &stream) {
    uint32_t value = 0;
	stream.read((char*)&value, sizeof(value));
    return Reverse(value); // UNCOMMENT FOR WINDDOWS
}*/

DiskInvertedIndex::DiskInvertedIndex(const boost::filesystem::path &path) : mPath(path) {
    mVocabList.open(boost::filesystem::path(mPath).append("/vocabList.bin", boost::filesystem::path::codecvt()).string(),
		std::ios_base::in | std::ios_base::binary);
    mPostings.open(boost::filesystem::path(mPath).append("/postings.bin", boost::filesystem::path::codecvt()).string(),
		std::ios_base::in | std::ios_base::binary);

	// open the vocabulary table and read it into memory. 
	// we will end up with an array of T pairs of longs, where the first value is
	// a position in the vocabularyTable file, and the second is a position in
	// the postings file.
	mVocabTable = ReadVocabTable(path);
}

// Reads a table of vocabulary entries, each of which consists of 2 8-byte values:
// a file location for a term's ASCII-encoded string text in vocabList.bin, and
// a file location for the term's postings list in postings.bin.
std::vector<VocabEntry> DiskInvertedIndex::ReadVocabTable(const fs::path & path) { // THIS FUNCTION IS HUNDO PERCENT BROKEN
	fs::path tablePath = path;

    ifstream tableFile(tablePath.append("/vocabTable.bin", boost::filesystem::path::codecvt()).string(),
		std::ios::in | std::ios::binary);
	uint32_t buffer, buffer2;

	tableFile.read((char *)&buffer, sizeof(buffer));
	buffer = Reverse(buffer);// UNCOMMENT FOR LINUX

    //int tableIndex = 0;

	std::vector<VocabEntry> vocabTable;
	vocabTable.reserve(buffer);

	while (tableFile.read((char*)&buffer, sizeof(buffer))) {
		tableFile.read((char*)&buffer2, sizeof(buffer2));

		// Again, you may not need the reverse calls.
		vocabTable.emplace_back(Reverse(buffer), Reverse(buffer2));// UNCOMMENT FOR LINUX
	}

    //std::cout << "Vocab table size: " << vocabTable.size() << std::endl;

	return vocabTable;
}

VocabEntry DiskInvertedIndex::BinarySearchVocabulary(const std::string &term) const { // const icu::UnicodeString &term
	std::size_t i = 0, j = mVocabTable.size() - 1;
	while (i <= j) {
		std::size_t m = i + (j - i) / 2;


		std::string uniStr = ReadVocabStringAtPosition(m);
        std::cout << "READ VOCAB AT: " << uniStr << std::endl;

		int comp = term.compare(uniStr);
		if (comp == 0) 
			return mVocabTable[m];
		else if (comp < 0) {
			if (m == 0) {
				std::cout << "UH OH IT GOT HERE 2!" << std::endl;
				return VocabEntry(-1, -1);
			}
			j = m - 1;
		}
		else {
			i = m + 1;
		}
	}

	std::cout << "UH OH IT GOT HERE 2!" << std::endl;
	return VocabEntry(-1, -1);;
}

std::list<DocInfo> DiskInvertedIndex::ReadPostingsFromFile(std::ifstream &postings, uint32_t postingsPosition) { // change from vector to list...?
	// seek the specified position in the file
	postings.clear(); // Trust me on this. fstream will fail all subsequent read calls if you ever read to the end of the file,
					  // say, if the term you are reading is last alphabetically. This was a nightmare to debug.
	postings.seekg(postingsPosition, postings.beg);

	// Read the document frequency. size_t docFreq = (uint32_t) Reverse(postings.size());
    uint32_t docFreq = ReadInt(postings); // size_t
	//std::cout << "docFreq(" << docFreq << ") ";

	// initialize the vector of document IDs to return.
	std::list<DocInfo> posts;

	// read 4 bytes at a time from the file, until you have read as many
	//    postings as the document frequency promised.
	uint32_t i;
	uint32_t lastDocId = 0;
	//unsigned char j, byte;
	//std::cout << "printing doc gaps: ";
	for (i = 0; i < docFreq; ++i) {
        const uint32_t &encodedDoc = ReadInt(postings);
		//std::cout << "docId(" << encodedDoc << ") ";

		uint32_t currId = encodedDoc + lastDocId;
		posts.push_back(DocInfo(currId));

        DocInfo &currDoc = posts.back();
        uint32_t positionSize = ReadInt(postings); // size_t
		//std::cout << "positionSize(" << positionSize << ") ";

		uint32_t i, lastPosGap = 0;
		for (i = 0; i < positionSize; ++i) {
			const uint32_t &encodedPos = ReadInt(postings);
			//std::cout << encodedPos << " ";

			uint32_t currPos = encodedPos + lastPosGap;

            currDoc.addPosition(currPos);

			lastPosGap = encodedPos;
		}

		lastDocId = currId; // currId
		
	}// repeat until all postings are read.

	return posts;
}

std::string DiskInvertedIndex::ReadVocabStringAtPosition(uint32_t i) const {
	auto &entry = mVocabTable[i];
	
	uint32_t termLength;

	
	if (i == mVocabTable.size() - 1) {
		mVocabList.clear();
		mVocabList.seekg(0, mVocabList.end); // GETS LENGTH OF ENTIRE FILE
		auto end = mVocabList.tellg();
		termLength = (uint32_t) end - entry.StringPosition;
	}
	else {
		termLength = (mVocabTable[i + 1].StringPosition - entry.StringPosition); // (uint32_t)

	}

    std::cout << "Term length:" << termLength << "String pos: " << entry.StringPosition;

	mVocabList.clear();
	mVocabList.seekg(entry.StringPosition, mVocabList.beg);
	char *buffer = new char[termLength + 1];
	buffer[termLength] = '\0';
	mVocabList.read(buffer, termLength);
	std::string str(buffer);
	delete[] buffer;
	return std::move(str);
}

std::list<DocInfo> DiskInvertedIndex::GetPostings(const std::string &term) const { // const icu::UnicodeString &term
	VocabEntry entry = BinarySearchVocabulary(term);
	if (entry.PostingPosition != -1 && entry.StringPosition != -1)
		return ReadPostingsFromFile(mPostings, entry.PostingPosition);

	std::cout << "DAMN IT! IT GOT HERE!" << std::endl;
	return std::list<DocInfo>();
}

std::vector<double_t> DiskInvertedIndex::ReadWeights(const boost::filesystem::path &path) {
	fs::path weightbPath = path;

	ifstream weightFile(weightbPath.append("/docWeights.bin", boost::filesystem::path::codecvt()).string(),
		std::ios::in | std::ios::binary);

	std::vector<double_t> weights;
	uint32_t size = ReadInt(weightFile);
	weights.reserve(size);

	uint32_t i;
	for (i = 0; i < size; ++i) {
		double_t w = ReadDouble(weightFile);
		weights.push_back(w);
	}

	weightFile.close();
}

/*DocInfo ReadDocumentPosting(std::ifstream &postings, uint32_t lastDocId) { // 
	return DocInfo();
}*/

void DiskInvertedIndex::printAllPostings(const InvertedIndex &idx) {
	std::cout << std::endl;
	std::cout << "PRINTING ALL POSTINGS FROM FILE!" << std::endl;
	for (const std::string &term : idx.getVocabList()) {
        const std::list<DocInfo> &posts = getPostings(term);

		for (auto docFile : posts) {
			std::cout << "DocId = " << docFile.getDocId() << std::endl << "pos: ";
			for (auto posFile : docFile.getPositions()) {
				std::cout << posFile << " ";
			}
		}
		std::cout << std::endl;
	}
	std::cout << std::endl;
}