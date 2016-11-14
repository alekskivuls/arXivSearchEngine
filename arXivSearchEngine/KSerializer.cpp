#include "KSerializer.h"
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

/**
 * This method is to build the file to store all of the kgrams to term pairings.
 * It works like an index and maps a token from Kgrams file made by buildKgrams
 * to a terms file made by buildTerms.
 *
 * Calls build
 */
static void KSerializer::buildIndex(const boost::filesystem::path &filePath, const KgramIndex &auxIdx1, const KgramIndex &auxIdx2, const KgramIndex &auxIdx3){
    //do i continue to put & infront of these?
    std::vector<uint64_t> kgramPositions = Serializer::buildKgrams(filePath, auxIdx1, auxIdx2, auxIdx3); //returns the vector<int>
    //vector<int> is just like another int[]... but by reference?
    Serializer::buildTerms(filePath, auxIdx1, auxIdx2, auxIdx3, kgramPositions); //builds kgram table here
}

//used by build terms
//should first print out number of terms with terms preceeding.
void KSerializer::WriteTerms(std::ofstream &termsFile, const std::list<std::string> &terms) {
    size_t termCt = (uint64_t) Reverse(terms.size());
    termsFile.write((const char*)&termCt, sizeof(termCt)); //writing number of terms.

    //now i just want to write to terms.
//    uint32_t lastTerm = termsFile.tellp();
    for (auto &currTerm : terms) {
        // write document ID gap.
        // Uses Reverse to fix endianness issues on Windows. If not building on Windows, you may
        // want to remove the Reverse calls.
//        uint32_t TerAdGap = Reverse(termsFile.tellp() - lastTerm);
//        termsFile.write((const char*)&TerAdGap, sizeof(TerAdGap)); // variable byte encode this

        termsFile.write(currTerm.c_str(), (sizeof(char) * currTerm.length())); //writing null term string.
//        //i dont have positions...
//        size_t positionSize = (uint64_t)Reverse(currDoc.getPositions().size());
//        termsFile.write((const char*)&positionSize, sizeof(positionSize)); // write to disk size of position list

//        uint32_t lastPosGap = 0;
//        for (const uint32_t &pos : currDoc.getPositions()) {
//            uint32_t posGap = Reverse(pos - lastPosGap);
//            termsFile.write((const char*)&posGap, sizeof(posGap));
//            lastPosGap = pos;
//        }

//        lastTerm = (uint32_t)(currTerm.size() * sizeof(char)); //use this tellp instead of docid? can i?
        //where the term started.
        //dont want to close because of main one.
    }
}

//method to just print all kgrams out here.
//returns a vector<int> of all of the locations of the kgram youre going to write to file.
//intend to take in kgramindex of size 1, 2, 3.
std::vector<uint64_t> Serializer::buildKgrams(const boost::filesystem::path &filePath, const KgramIndex &auxIdx1, const KgramIndex &auxIdx2, const KgramIndex &auxIdx3) {
    // first build the vocabulary list: a file of each vocab word concatenated together.
    // also build an array associating each term with its byte location in this file.
    std::vector<uint64_t> positions(auxIdx1.getKgramList().size() + auxIdx2.getKgramList().size() + auxIdx3.getKgramList().size()); //all of your kgrams totaled
    int kgramIndex = 0; //thing you increment
    //int vocabPos = 0; //whenthefuckdoyouusethis
    std::list<std::string> allKgrams;

    //the main file you will write in.
    boost::filesystem::path vocabPath = filePath;
    //paul said i need to sort this.
    //setting output stream as the file. make /kgramList.bin put output there.
    std::ofstream kgramFile(vocabPath.append("/kgramList.bin", boost::filesystem::path::codecvt()).string(), std::ios::out | std::ios::binary);

    allKgrams = auxIdx1.getKgramList();
    allKgrams.merge(auxIdx2.getKgramList()); //not sure if i can do this
    allKgrams.merge(auxIdx3.getKgramList());
    //alphabetize
    allKgrams.sort(); //$ is first then shorter length should be.

    //for the terms in your list of kgrams in your index. so only 3-gram example.
    for (const std::string &kgram : allKgrams) {
        positions[kgramIndex] = kgramFile.tellp(); //pos of current char in output stream.
        //write the kgram reference's pointer to an array that contains a null-terminated sequence of characters.
        kgramFile.write(kgram.size(), (sizeof(int)));
        kgramFile.write(kgram.c_str(), (sizeof(char) * kgram.length())); //size of buffer is second param.
        kgramIndex++; //increment to move to writing in next positions, what is the vector <int> that you will return.
    }
    //Youre done writing the kgrams down, close the ofstream.
    kgramFile.close();
    return positions; //returning all positions. in order of how you wrote to kgramfile.
}


// This method will build the file that stores all of the terms. Writes term to disk.
static void KSerializer::buildTerms(const boost::filesystem::path &filePath, const KgramIndex &auxIdx1, const KgramIndex &auxIdx2, const KgramIndex &auxIdx3,
                                    const std::vector<uint64_t> &kgramPositions){

    //where you will write the terms and where you will write the tables.
    boost::filesystem::path termsPath = filePath, tablePath = filePath;

    // simultaneously build the vocabulary table on disk, mapping a term index to a
    // file location in the postings file.
    std::ofstream termsFile(postingsPath.append("/terms.bin", boost::filesystem::path::codecvt()).string(), std::ios::out | std::ios::binary);
    std::ofstream kgramTable(tablePath.append("/kgramTable.bin", boost::filesystem::path::codecvt()).string(), std::ios::out | std::ios::binary);
    std::list<std::string> allKgrams; //likely inefficient because recreated in buildKgrams method as well;

    // the first thing we must write to the vocabTable file is the number of kgram terms.
    int64_t termCount = Reverse(kgramPositions.size()); // UNCOMMENT FOR WINDOWS //this or: (auxIdx1.getKgramList().size() + auxIdx2.getKgramList().size() + auxIdx3.getKgramList().size())

    kgramTable.write((const char*)&termCount, sizeof(termCount)); //params are buffer and size.
    //you are just writing &termcount.
    //start with number of kgrams

    int64_t kgramIndex = 0; // FOR EVERY TERM //iterator pmuch.

    allKgrams = auxIdx1.getKgramList();
    allKgrams.merge(auxIdx2.getKgramList()); //not sure if i can do this
    allKgrams.merge(auxIdx3.getKgramList());
    //alphabetize
    allKgrams.sort();
    for (const std::string &kgram : allKgrams) { //for all kgrams in the index
        //if it doesn't fall in any it will break. vulnerability
        if(kgram.size() == 1) const std::list<std::string> &terms = auxIdx1.getTerms(kgram);
        if(kgram.size() == 2) const std::list<std::string> &terms = auxIdx2.getTerms(kgram);
        else const std::list<std::string> &terms = auxIdx3.getTerms(kgram); //must be three please.

        // write the kgram table entry for this kgram: the byte location of the kgram in the kgramlist file,
        // and the byte location of the terms for the term in the terms file.
        uint64_t kPosition = Reverse(kgramPositions[kgramIndex]); //get first //where the k is located you save.
        kgramTable.write((const char*)&kPosition, sizeof(kPosition)); //ideally the pointer to the k
        //writing where first kgram is.

        uint64_t tPosition = Reverse((uint64_t)termsFile.tellp()); //position where you will start writing terms for this kgram
        //also marks where the last term left off. hence where the new term set starts.
        kgramTable.write((const char*)&tPosition, sizeof(tPosition)); //write term position.

        WriteTerms(termsFile, terms); //you give it the place to write it and the terms to write.

        kgramIndex++;
    }

    kgramTable.close();
    termsFile.close();
}
