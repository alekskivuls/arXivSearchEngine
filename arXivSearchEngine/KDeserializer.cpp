#include "KDeserializer.h"
#include <iomanip>
#include <iostream>

using std::ifstream;
namespace fs = boost::filesystem;

extern uint32_t Reverse(uint32_t);
//extern uint64_t Reverse(uint64_t);
//extern double_t Reverse(double_t);

//reads 32 integer and returns it.
uint32_t KDeserializer::ReadInt(std::ifstream &stream) { //here
    uint32_t value = 0;
    stream.read((char*)&value, sizeof(value));
    return Reverse(value); // UNCOMMENT FOR WINDDOWS
}

//constructor just needs the location of the files.
KDeserializer::KDeserializer(const boost::filesystem::path  &path) : mPath(path) {
    mKgramList.open(boost::filesystem::path(mPath).append("/kgramList.bin", boost::filesystem::path::codecvt()).string(),
        std::ios_base::in | std::ios_base::binary);
    mTerms.open(boost::filesystem::path(mPath).append("/terms.bin", boost::filesystem::path::codecvt()).string(),
        std::ios_base::in | std::ios_base::binary); //used in getterms.
    //used in readterms
    //use mterms portal to read from the terms.bin.

    // open the vocabulary table and read it into memory.
    // we will end up with an array of T pairs of longs, where the first value is
    // a position in the vocabularyTable file, and the second is a position in
    // the postings file.
    std::cout << "sjfadhkjfh" << std::endl;
    mKgramTable = ReadKgramTable(path);
}



//where to start reading terms
//read table from /kgramTable.bin
//SHOULD READ THE WHOLE THING. SO EVERYTHIGNI S TRANSLATED INTO ALREADY
std::vector<KgramEntry> KDeserializer::ReadKgramTable(const boost::filesystem::path &path){
    fs::path tablePath = path;

    ifstream tableFile(tablePath.append("/kgramTable.bin", boost::filesystem::path::codecvt()).string(),
        std::ios::in | std::ios::binary);
    uint32_t buffer, buffer2;

    tableFile.read((char *)&buffer, sizeof(buffer)); //read 32int.
    buffer = Reverse(buffer);// UNCOMMENT FOR LINUX reverse int.

    int tableIndex = 0;

    std::vector<KgramEntry> kgramTable; //making a KgramEntry[] all the relations
    kgramTable.reserve(buffer);

    while (tableFile.read((char*)&buffer, sizeof(buffer))) { //thse are perfect 32 so fine.
        tableFile.read((char*)&buffer2, sizeof(buffer2));

        // Again, you may not need the reverse calls.
        kgramTable.emplace_back(Reverse(buffer), Reverse(buffer2));// UNCOMMENT FOR LINUX
    }

    std::cout << "KLAJSD SIZE OF KGRUM TABLEEEE HUUUUUUUU! " << kgramTable.size() << std::endl;

    return kgramTable;
}


//read terms from file and pass back as list.
//convert terms from file into list <string>
//ther idstream and then the position where the term starts
std::list<std::string> KDeserializer::ReadTermsFromFile(std::ifstream &terms, uint32_t termsPosition){ // change from vector to list...?
    terms.clear(); // Trust me on this. fstream will fail all subsequent read calls if you ever read to the end of the file,
                      // say, if the term you are reading is last alphabetically. This was a nightmare to debug.
    terms.seekg(termsPosition, terms.beg); //beg means you want it to start here. you can also do end to end there.
    uint32_t numTerms = ReadInt(terms); // size_t

    // initialize the vector of document IDs to return.
    std::list<std::string> trms;
    uint32_t i, j;
    uint8_t buff;
    std::string word;

    for (i = 0; i < numTerms; ++i) {
        uint32_t termLength = ReadInt(terms); //readnext int in the terms ifstream
        word = "";
        for (j = 0; j < termLength; ++j){
            terms.read((char*)&buff, sizeof(buff));
            word += std::string(1, buff);
        }
        trms.push_back(word);
    }// repeat until all postings are read.

    return trms;
}

//return kgramentry object that contains the requested kgram.
KgramEntry KDeserializer::BinarySearchKgrams(const std::string &kgram) const { // const icu::UnicodeString &term
    std::size_t i = 0, j = mKgramTable.size() - 1; //j equals last kgram index.
    std::cout << "binarysearchkgrams" << std::endl;
    while (i <= j) {
        std::size_t m = i + (j - i) / 2;


        std::string uniStr = ReadKgramStringAtPosition(m); //read middle kgram.
        //std::cout << "READ KGRUM AT: " << uniStr << std::endl;

        int comp = kgram.compare(uniStr);
        if (comp == 0)
            return mKgramTable[m]; //found it.
        else if (comp < 0) {
            if (m == 0) {
                std::cout << "UH OH IT GOT HERE 2!" << std::endl;
                return KgramEntry(-1, -1);
            }
            j = m - 1; //depending on compare search middles. binary.
        }
        else {
            i = m + 1;
        }
    }

    std::cout << "UH OH IT GOT HERE 2!" << std::endl;
    return KgramEntry(-1, -1);;
}

//accesses stuct's kgramposition.
std::string KDeserializer::ReadKgramStringAtPosition(uint32_t index) const{
    auto &entry = mKgramTable[index];

    uint32_t termLength;


    if (index == mKgramTable.size() - 1) {
        mKgramList.clear();
        //seekg sets the address of the next input token.
        mKgramList.seekg(0, mKgramList.end); // GETS LENGTH OF ENTIRE FILE
        auto end = mKgramList.tellg();
        termLength = (uint32_t) end - entry.KgramPosition;
    }
    else {
        termLength = (mKgramTable[index + 1].KgramPosition - entry.KgramPosition); // (uint32_t)
    }

    mKgramList.clear(); //mkgramlist is ifstream
    mKgramList.seekg(entry.KgramPosition, mKgramList.beg);
    char *buffer = new char[termLength + 1];
    buffer[termLength] = '\0';
    mKgramList.read(buffer, termLength);
    std::string str(buffer);
    delete[] buffer;
    return std::move(str);
}


std::list<std::string> KDeserializer::GetTerms(const std::string &kgram) const{ // const icu::UnicodeString &term
    KgramEntry entry = BinarySearchKgrams(kgram); //reyurns entree of it
    std::cout << "kgrams list here" << std::endl;
    if (entry.TermPosition != -1 && entry.KgramPosition != -1)
        return ReadTermsFromFile(mTerms, entry.TermPosition);
    //nothing
    std::cout << "DAMN IT! IT GOT HERE!" << std::endl;
    return std::list<std::string>(); //emty list

}


void KDeserializer::printAllTerms(KgramIndex &idx){
    std::cout << std::endl;
    std::cout << "PRINTING FROM FILE!" << std::endl;
    for (const std::string &kgram : idx.getKgramList()) {

        const std::list<std::string> &term = GetTerms(kgram);
        std::cout << "did reach!" << std::endl;
        std::cout << std::endl;
    }
    std::cout << std::endl;
}

