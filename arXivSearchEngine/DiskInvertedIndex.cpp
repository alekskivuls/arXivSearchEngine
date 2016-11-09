#include "DiskInvertedIndex.h"
#include <sstream>


using std::ifstream;
namespace fs = boost::filesystem;

extern uint32_t Reverse(uint32_t);
extern uint64_t Reverse(uint64_t);
extern double_t Reverse(double_t);


uint32_t ReadInt(std::ifstream &stream) {
	uint32_t value = 0;
	stream.read((char*)&value, sizeof(value));
	return Reverse(value);// UNCOMMENT FOR LINUX
}

DiskInvertedIndex::DiskInvertedIndex(const boost::filesystem::path &path) : mPath(path) {
	mVocabList.open(boost::filesystem::path(mPath).append("/vocabList.bin").string(),
		std::ios_base::in | std::ios_base::binary);
	mPostings.open(boost::filesystem::path(mPath).append("/postings.bin").string(),
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
std::vector<VocabEntry> DiskInvertedIndex::ReadVocabTable(const fs::path & path) {
	fs::path tablePath = path;

	ifstream tableFile(tablePath.append("/vocabTable.bin").string(),
		std::ios::in | std::ios::binary);
	uint64_t buffer, buffer2;
	tableFile.read((char *)&buffer, sizeof(buffer));
	buffer = Reverse(buffer);// UNCOMMENT FOR LINUX

	int tableIndex = 0;

	std::vector<VocabEntry> vocabTable;
	vocabTable.reserve(buffer);

	while (tableFile.read((char*)&buffer, sizeof(buffer))) {
		tableFile.read((char*)&buffer2, sizeof(buffer2));

		// Again, you may not need the reverse calls.
		vocabTable.emplace_back(Reverse(buffer), Reverse(buffer2));// UNCOMMENT FOR LINUX
	}

	return vocabTable;
}

VocabEntry DiskInvertedIndex::BinarySearchVocabulary(const std::string &term) const { // const icu::UnicodeString &term
	std::size_t i = 0, j = mVocabTable.size() - 1;
	while (i <= j) {
		std::size_t m = i + (j - i) / 2;
		std::string uniStr = ReadVocabStringAtPosition(m);

		int comp = term.compare(uniStr);
		if (comp == 0) {
			return mVocabTable[m];
		}
		else if (comp < 0) {
			if (m == 0)
				return VocabEntry(-1, -1);
			j = m - 1;
		}
		else {
			i = m + 1;
		}
	}
	return VocabEntry(-1, -1);;
}

std::vector<DiskPosting> DiskInvertedIndex::ReadPostingsFromFile(std::ifstream &postings, uint64_t postingsPosition) {
	// seek the specified position in the file
	postings.clear(); // Trust me on this. fstream will fail all subsequent read calls if you ever read to the end of the file,
					  // say, if the term you are reading is last alphabetically. This was a nightmare to debug.
	postings.seekg(postingsPosition, postings.beg);

	// Read the document frequency.
	uint32_t docFreq = ReadInt(postings);
	// initialize the vector of document IDs to return.
	std::vector<DiskPosting> posts;
	posts.reserve(docFreq);

	// write the following code:
	// read 4 bytes at a time from the file, until you have read as many
	//    postings as the document frequency promised.
	//    
	// after each read, convert the bytes to an int posting. this value
	//    is the GAP since the last posting. decode the document ID from
	//    the gap and put it in the array.
	//
	// repeat until all postings are read.
	return posts;
}

std::string DiskInvertedIndex::ReadVocabStringAtPosition(size_t i) const {
	auto &entry = mVocabTable[i];

	uint32_t termLength;

	if (i == mVocabTable.size() - 1) {
		mVocabList.clear();
		mVocabList.seekg(0, mVocabList.end);
		auto end = mVocabList.tellg();
		termLength = end;
	}
	else {
		termLength = mVocabTable[i + 1].StringPosition - entry.StringPosition;
	}
	mVocabList.clear();
	mVocabList.seekg(entry.StringPosition, mVocabList.beg);
	char *buffer = new char[termLength + 1];
	buffer[termLength] = '\0';
	mVocabList.read(buffer, termLength);
	std::string str(buffer);
	delete[] buffer;
	return std::move(str);
}

std::vector<DiskPosting> DiskInvertedIndex::GetPostings(const std::string &term) const { // const icu::UnicodeString &term
	VocabEntry entry = BinarySearchVocabulary(term);
	if (entry.PostingPosition != -1 && entry.StringPosition != -1)
		return ReadPostingsFromFile(mPostings, entry.PostingPosition);
	return std::vector<DiskPosting>();
}
