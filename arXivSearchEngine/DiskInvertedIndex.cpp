#include "DiskInvertedIndex.h"
#include <iostream>
#include <sstream>



using std::ifstream;
namespace fs = boost::filesystem;

extern uint32_t Reverse(uint32_t);
extern uint64_t Reverse(uint64_t);
extern double_t Reverse(double_t);


uint32_t DiskInvertedIndex::ReadInt(std::ifstream &stream) {
	uint32_t value = 0;
	stream.read((char*)&value, sizeof(value));
	return Reverse(value); // UNCOMMENT FOR WINDDOWS
						   //return value;
}
uint64_t DiskInvertedIndex::ReadInt64(std::ifstream &stream) {
	uint32_t value = 0;
	stream.read((char*)&value, sizeof(value));
	return (uint64_t)Reverse(value); // UNCOMMENT FOR WINDDOWS
						   //return value;
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

	/*
	std::cout << "READING FROM FILE: " << std::endl;
	for (auto v : mVocabTable) {
		std::cout << "V POSITION: " << v.StringPosition << std::endl;
		std::cout << "P POSITION: " << v.PostingPosition << std::endl;
	}*/
}

// Reads a table of vocabulary entries, each of which consists of 2 8-byte values:
// a file location for a term's ASCII-encoded string text in vocabList.bin, and
// a file location for the term's postings list in postings.bin.
std::vector<VocabEntry> DiskInvertedIndex::ReadVocabTable(const fs::path & path) { // THIS FUNCTION IS HUNDO PERCENT BROKEN
	fs::path tablePath = path;

	ifstream tableFile(tablePath.append("/vocabTable.bin").string(),
		std::ios::in | std::ios::binary);
	uint64_t buffer, buffer2;
	//uint32_t buffer, buffer2; // uint64_t uint32_t
	//tableFile.read((char *)&buffer, sizeof(buffer));
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

	std::cout << "size of vocabTable: " << vocabTable.size() << std::endl; // wait... but this is returning values...

	return vocabTable;
}

VocabEntry DiskInvertedIndex::BinarySearchVocabulary(const std::string &term) const { // const icu::UnicodeString &term
	std::size_t i = 0, j = mVocabTable.size() - 1; //  THIS MIGHT BE BROKEN?!?!
	while (i <= j) {
		std::size_t m = i + (j - i) / 2;


		std::string uniStr = ReadVocabStringAtPosition(m);

		int comp = term.compare(uniStr);
		if (comp == 0) {
			std::cout << "FOUND V POSITION: " << mVocabTable[m].StringPosition << std::endl;
			std::cout << "FOUND P POSITION: " << mVocabTable[m].PostingPosition  << std::endl;
			return mVocabTable[m];
		}
		else if (comp < 0) {
			if (m == 0) {
				std::cout << "Oh no... It got here... (Not Found)" << std::endl;
				return VocabEntry(-1, -1);
			}

			j = m - 1;
		}
		else {
			i = m + 1;
		}
	}
	std::cout << "Oh no... It got here... (Empty)" << std::endl;
	return VocabEntry(-1, -1);;
}

std::vector<DocInfo> DiskInvertedIndex::ReadPostingsFromFile(std::ifstream &postings, uint64_t postingsPosition) {
	// seek the specified position in the file
	postings.clear(); // Trust me on this. fstream will fail all subsequent read calls if you ever read to the end of the file,
					  // say, if the term you are reading is last alphabetically. This was a nightmare to debug.
	postings.seekg(postingsPosition, postings.beg);

	// Read the document frequency. size_t docFreq = (uint64_t) Reverse(postings.size());
	size_t docFreq = ReadInt(postings);

	// initialize the vector of document IDs to return.
	std::vector<DocInfo> posts;
	posts.reserve(docFreq);

	// read 4 bytes at a time from the file, until you have read as many
	//    postings as the document frequency promised.
	uint32_t i;
	uint32_t lastDocId = 0;
	//unsigned char j, byte;
	//std::cout << "printing doc gaps: ";
	for (i = 0; i < docFreq; ++i) {
		const uint32_t &encoded = ReadInt(postings); //  // try 64
		uint32_t currId = encoded + lastDocId;

		posts.push_back(DocInfo(currId));
		std::cout << currId << ' ';
		lastDocId = encoded;
		
	}// repeat until all postings are read.
	std::cout << std::endl;

	return posts;
}

// ignore this for loop for now
/*for (j = 0; j < 4; ++j) { // i MAY want to call readInt... uncomment this for now
byte = (j * 8) & 0xFF;
// after each read, convert the bytes to an int posting. this value
//    is the GAP since the last posting. decode the document ID from
//    the gap and put it in the array.
}*/

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
		termLength = (mVocabTable[i + 1].StringPosition - entry.StringPosition); // (uint32_t)
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

std::vector<DocInfo> DiskInvertedIndex::GetPostings(const std::string &term) const { // const icu::UnicodeString &term
	VocabEntry entry = BinarySearchVocabulary(term);

	std::cout << "entry.PostingPosition != -1 && entry.StringPosition != -1 is "
		<< (entry.PostingPosition != -1 && entry.StringPosition != -1) << std::endl;
	if (entry.PostingPosition != -1 && entry.StringPosition != -1)
		return ReadPostingsFromFile(mPostings, entry.PostingPosition);
	return std::vector<DocInfo>();
}

DocInfo ReadDocumentPosting(std::ifstream &postings, uint32_t lastDocId) {
	return DocInfo();
}