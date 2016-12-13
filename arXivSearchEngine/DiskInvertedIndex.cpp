#include "DiskInvertedIndex.h"
#include <sstream>
#include <iostream>

using std::ifstream;
namespace fs = boost::filesystem;

extern uint8_t Reverse(uint8_t);
extern uint32_t Reverse(uint32_t);
extern uint64_t Reverse(uint64_t);
extern double_t Reverse(double_t);

//Reversing depends on how method was written, in reverse or not
//TODO: Refactor to one type (reverse or not) and move read and writing to same class

uint8_t DiskInvertedIndex::ReadChar(std::ifstream &stream) {
    uint8_t value = 0;
    stream.read((char*)&value, sizeof(value));
    //return Reverse(value);
    return value;
}

uint32_t DiskInvertedIndex::ReadInt(std::ifstream &stream) {
    uint32_t value = 0;
    stream.read((char*)&value, sizeof(value));
    return Reverse(value);
}

uint32_t DiskInvertedIndex::ReadRawInt(std::ifstream &stream) {
    uint32_t value = 0;
    stream.read((char*)&value, sizeof(value));
    return Reverse(value);
    return value;
}

double_t DiskInvertedIndex::ReadDouble(std::ifstream &stream) {
    double_t value = 0;
    stream.read((char*)&value, sizeof(value));
    //return Reverse(value);
    return value;
}

DiskInvertedIndex::DiskInvertedIndex() { }

DiskInvertedIndex::DiskInvertedIndex(const boost::filesystem::path &path) : mPath(path) {
    mVocabList.open(boost::filesystem::path(mPath).append("/vocabList.bin", boost::filesystem::path::codecvt()).string(),
                    std::ios_base::in | std::ios_base::binary);
    mVocabPostings.open(boost::filesystem::path(mPath).append("/postings.bin", boost::filesystem::path::codecvt()).string(),
                   std::ios_base::in | std::ios_base::binary);
    mAuthorList.open(boost::filesystem::path(mPath).append("/authorList.bin", boost::filesystem::path::codecvt()).string(),
                    std::ios_base::in | std::ios_base::binary);
    mAuthorPostings.open(boost::filesystem::path(mPath).append("/authorDocs.bin", boost::filesystem::path::codecvt()).string(),
                    std::ios_base::in | std::ios_base::binary);
    // open the vocabulary table and read it into memory.
    // we will end up with an array of T pairs of longs, where the first value is
    // a position in the vocabularyTable file, and the second is a position in
    // the postings file.
    mVocabTable = ReadTable(path, "vocabTable.bin");
    mAuthorTable = ReadTable(path, "authorTable.bin");
}

// Reads a table of vocabulary entries, each of which consists of 2 8-byte values:
// a file location for a term's ASCII-encoded string text in vocabList.bin, and
// a file location for the term's postings list in postings.bin.
std::vector<ListEntry> DiskInvertedIndex::ReadTable(const fs::path &path, const std::string &tableFilename) {
    fs::path tablePath = path;

    ifstream tableFile(tablePath.append("/"+tableFilename, boost::filesystem::path::codecvt()).string(),
        std::ios::in | std::ios::binary);
    uint32_t buffer, buffer2;

    tableFile.read((char *)&buffer, sizeof(buffer));
    buffer = Reverse(buffer);// UNCOMMENT FOR LINUX

    std::vector<ListEntry> table;
    table.reserve(buffer);

    while (tableFile.read((char*)&buffer, sizeof(buffer))) {
        tableFile.read((char*)&buffer2, sizeof(buffer2));

        // Again, you may not need the reverse calls.
        table.emplace_back(Reverse(buffer), Reverse(buffer2));// UNCOMMENT FOR LINUX
    }

    tableFile.close();

    return table;
}

std::unordered_map<uint32_t, std::string> DiskInvertedIndex::ReadIdTableFromFile(const boost::filesystem::path &path) {
    fs::path idTablePath = path;

    ifstream idTableFile(idTablePath.append("/idTable.bin", boost::filesystem::path::codecvt()).string(),
                         std::ios::in | std::ios::binary);

    uint32_t i,j, size = ReadInt(idTableFile);
    //std::cout << "size = " << size << std::endl;
    std::unordered_map<uint32_t, std::string> idTable;
    for (i = 0; i < size; ++i) {
        //std::cout << i << std::endl;
        uint32_t id = ReadInt(idTableFile);
        uint32_t len = ReadInt(idTableFile);

        std::string value = "";
        for (j = 0; j < len; j++) {
            uint8_t ch = ReadChar(idTableFile);
            //idTableFile.read((char*)&ch, sizeof(ch));
            value += (unsigned char) ch;
        }

        idTable[id] = value;
        std::cout << "ID: " << id << " Value = " << value << std::endl;
    }

    idTableFile.close();

    return idTable;
}

ListEntry DiskInvertedIndex::binarySearchList(const std::string &term, std::vector<ListEntry> &table, std::ifstream &list) { // const icu::UnicodeString &term
    std::size_t i = 0, j = table.size() - 1;
    while (i <= j) {
        std::size_t m = i + (j - i) / 2;

        std::string uniStr = readStringAtPosition(m, table, list);
        //std::cout << "READ VOCAB AT: " << uniStr << std::endl;

        int comp = term.compare(uniStr);
        if (comp == 0)
            return table[m];
        else if (comp < 0) {
            if (m == 0) {
                return ListEntry(-1, -1);
            }
            j = m - 1;
        }
        else {
            i = m + 1;
        }
    }
    return ListEntry(-1, -1);
}

std::list<DocInfo> DiskInvertedIndex::ReadPostingsFromFile(std::ifstream &postings, uint32_t postingsPosition) {
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

std::list<DocInfo> DiskInvertedIndex::readAuthorDocsFromFile(std::ifstream &postings, uint32_t postingsPosition) {
    // seek the specified position in the file
    postings.clear(); // Trust me on this. fstream will fail all subsequent read calls if you ever read to the end of the file,
    // say, if the term you are reading is last alphabetically. This was a nightmare to debug.
    postings.seekg(postingsPosition, postings.beg);

    // Read the document frequency. size_t docFreq = (uint32_t) Reverse(postings.size());
    uint32_t docFreq = ReadInt(postings); // size_t
    //std::cout << "docFreq(" << docFreq << ") ";

    // initialize the vector of document IDs to return.
    std::list<DocInfo> docs;

    // read 4 bytes at a time from the file, until you have read as many
    //    postings as the document frequency promised.
    uint32_t i;
    //unsigned char j, byte;
    //std::cout << "printing doc gaps: ";
    for (i = 0; i < docFreq; ++i) {
        const uint32_t &encodedDoc = ReadInt(postings);
        docs.push_back(DocInfo(encodedDoc));

    }// repeat until all postings are read.

    return docs;
}

std::string DiskInvertedIndex::readStringAtPosition(uint32_t i, std::vector<ListEntry> &table, std::ifstream &list) const {
    auto &entry = table[i];

    uint32_t termLength;


    if (i == table.size() - 1) {
        list.clear();
        list.seekg(0, list.end); // GETS LENGTH OF ENTIRE FILE
        auto end = list.tellg();
        termLength = (uint32_t) end - entry.StringPosition;
    }
    else {
        termLength = (table[i + 1].StringPosition - entry.StringPosition); // (uint32_t)
    }
    //std::cout << "Term length:" << termLength << "String pos: " << entry.StringPosition;
    list.clear();
    list.seekg(entry.StringPosition, list.beg);
    char *buffer = new char[termLength + 1];
    buffer[termLength] = '\0';
    list.read(buffer, termLength);
    std::string str(buffer);
    delete[] buffer;
    return std::move(str);
}

std::list<DocInfo> DiskInvertedIndex::getPostings(const std::string &term)  { // const icu::UnicodeString &term
    ListEntry entry = binarySearchList(term, mVocabTable, mVocabList);
    if (entry.PostingPosition != -1 && entry.StringPosition != -1)
        return ReadPostingsFromFile(mVocabPostings, entry.PostingPosition);
    return std::list<DocInfo>();
}
/**
 * @brief DiskInvertedIndex::getN Returns the total number of docs in the DiskInvertedIndex.
 * @return The number of documents in the DiskInvertedIndex.
 */
uint32_t DiskInvertedIndex::getN() {
    fs::path weightbPath = mPath;

    ifstream weightFile(weightbPath.append("/docWeights.bin", boost::filesystem::path::codecvt()).string(),
                        std::ios::in | std::ios::binary);

    uint32_t size = ReadInt(weightFile);
    weightFile.close();
    return size;
}

std::vector<double_t> DiskInvertedIndex::ReadWeights() {
    fs::path weightbPath = mPath;

    ifstream weightFile(weightbPath.append("/docWeights.bin", boost::filesystem::path::codecvt()).string(),
                        std::ios::in | std::ios::binary);

    std::vector<double_t> weights;
    uint32_t size = ReadInt(weightFile);
    std::cout << "Size: " << size << std::endl;

    std::cout << "READ FILE VECTOR SIZE: " << size << std::endl;
    weights.reserve(size);

    uint32_t i;
    for (i = 0; i < size; ++i) {
        double_t w = ReadDouble(weightFile);
        weights.push_back(w);
    }

    weightFile.close();

    return weights;
}

//list
std::list<std::string> DiskInvertedIndex::getVocabList() {
    std::list<std::string> vec;
    for(int i = 0; i < mVocabTable.size(); i++) {
        vec.push_back(readStringAtPosition(i, mVocabTable, mVocabList));
    }
    return vec;
}

std::list<std::string> DiskInvertedIndex::getAuthorList() {
    std::list<std::string> vec;
    for(int i = 0; i < mAuthorTable.size(); i++) {
        vec.push_back(readStringAtPosition(i, mAuthorTable, mAuthorList));
    }
    return vec;
}

std::list<DocInfo> DiskInvertedIndex::getAuthorDocs(const std::string &author)  {
    ListEntry entry = binarySearchList(author, mAuthorTable, mAuthorList);
    if (entry.PostingPosition != -1 && entry.StringPosition != -1)
        return readAuthorDocsFromFile(mAuthorPostings, entry.PostingPosition);
    return std::list<DocInfo>();
}

