#include "KSerializer.h"
#include <iomanip>
#include <iostream>

// For correcting endianness issues; you may not need these.
inline uint32_t Reverse(uint32_t value) {
    return (value & 0xFF000000) >> 24 |
        (value & 0x00FF0000) >> 8 |
        (value & 0x0000FF00) << 8 |
        (value & 0x000000FF) << 24;
}

//inline uint64_t Reverse(uint64_t value) {
//    return (value & 0xFF00000000000000) >> 56 |
//        (value & 0x00FF000000000000) >> 40 |
//        (value & 0x0000FF0000000000) >> 24 |
//        (value & 0x000000FF00000000) >> 8 |
//        (value & 0x00000000FF000000) << 8 |
//        (value & 0x0000000000FF0000) << 24 |
//        (value & 0x000000000000FF00) << 40 |
//        (value & 0x00000000000000FF) << 56;
//}

/**
 * This method is to build the file to store all of the kgrams to term pairings.
 * It works like an index and maps a token from Kgrams file made by buildKgrams
 * to a terms file made by buildTerms.
 *
 * Calls build
 */
void KSerializer::buildIndex(boost::filesystem::path &filePath, KgramIndex &auxIdx1, KgramIndex &auxIdx2, KgramIndex &auxIdx3){
    std::vector<uint32_t> kgramPositions = KSerializer::buildKgrams(filePath, auxIdx1, auxIdx2, auxIdx3); //returns the vector<int>
    for (auto thing : kgramPositions) {
        std::cout << thing << std::endl;
    }
    //vector<int> is just like another int[]... but by reference?
    std::cout << "finished buildkgrams, into buildterms" << std::endl;
    KSerializer::buildTerms(filePath, auxIdx1, auxIdx2, auxIdx3, kgramPositions); //builds kgram table here
}

/**
 * @brief KSerializer::WriteTerms : This method writes the terms to the termsFile.
 * @param termsFile : the outputfile stream of where you will write to.
 * @param terms : the list of strings you wish to store on disk.
 *
 * Terms file is stored like so: #terms, #chars in term1, term1...
 */
void KSerializer::WriteTerms(std::ofstream &termsFile, const std::list<std::string> &terms) {
    uint32_t termCt = Reverse((uint32_t)terms.size());
    termsFile.write((char*)&termCt, sizeof(termCt)); //writing number of terms.

    for (auto &currTerm : terms) {
        uint32_t termSz = Reverse((uint32_t)currTerm.size());
        termsFile.write((char*)&termSz, sizeof(termSz));
        termsFile.write(currTerm.c_str(), (sizeof(char) * currTerm.size())); //writing null term string.
    }
}

//method to just print all kgrams out here.
//returns a vector<int> of all of the locations of the kgram youre going to write to file.
//intend to take in kgramindex of size 1, 2, 3.
std::vector<uint32_t> KSerializer::buildKgrams(boost::filesystem::path &filePath, KgramIndex &auxIdx1, KgramIndex &auxIdx2, KgramIndex &auxIdx3) {
    // first build the vocabulary list: a file of each vocab word concatenated together.
    // also build an array associating each term with its byte location in this file.
    std::vector<uint32_t> positions(auxIdx1.getKgramList().size() + auxIdx2.getKgramList().size() + auxIdx3.getKgramList().size()); //all of your kgrams totaled
    int kgramIndex = 0; //thing you increment
    //int vocabPos = 0; //whenthefuckdoyouusethis
    std::list<std::string> allKgrams;

//    std::cout << "initialized variables" << std::endl;

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

    std::cout << "all grams sorted" << std::endl;

    //for the terms in your list of kgrams in your index. so only 3-gram example.
    for (std::string &kgram : allKgrams) {
        positions[kgramIndex] = kgramFile.tellp(); //pos of current char in output stream.
        //write the kgram reference's pointer to an array that contains a null-terminated sequence of characters.
        //kgramFile.write((char*)kgram.size(), (sizeof(kgram.size()))); //THIS LINE IS BROKEN... WHY.
        kgramFile.write(kgram.c_str(), (sizeof(char) * kgram.length())); //size of buffer is second param.
        kgramIndex++; //increment to move to writing in next positions, what is the vector <int> that you will return.
    }

    std::cout << "finished writing" << std::endl; //it never finishes writing
    //Youre done writing the kgrams down, close the ofstream.
    kgramFile.close();
    return positions; //returning all positions. in order of how you wrote to kgramfile.
}


// This method will build the file that stores all of the terms. Writes term to disk.
void KSerializer::buildTerms(boost::filesystem::path &filePath, KgramIndex &auxIdx1, KgramIndex &auxIdx2, KgramIndex &auxIdx3,
                                    std::vector<uint32_t> &kgramPositions){

    //where you will write the terms and where you will write the tables.
    boost::filesystem::path termsPath = filePath, tablePath = filePath;

    // simultaneously build the vocabulary table on disk, mapping a term index to a
    // file location in the postings file.
    std::ofstream termsFile(termsPath.append("/terms.bin", boost::filesystem::path::codecvt()).string(), std::ios::out | std::ios::binary);
    std::ofstream kgramTable(tablePath.append("/kgramTable.bin", boost::filesystem::path::codecvt()).string(), std::ios::out | std::ios::binary);
    std::list<std::string> allKgrams; //likely inefficient because recreated in buildKgrams method as well;


    // the first thing we must write to the vocabTable file is the number of kgram terms.
    int32_t termCount = Reverse(kgramPositions.size()); // UNCOMMENT FOR WINDOWS //this or: (auxIdx1.getKgramList().size() + auxIdx2.getKgramList().size() + auxIdx3.getKgramList().size())

    kgramTable.write((char*)&termCount, sizeof(termCount)); //params are buffer and size.
    //you are just writing &termcount.
    //start with number of kgrams

    int32_t kgramIndex = 0; // FOR EVERY TERM //iterator pmuch.

    allKgrams = auxIdx1.getKgramList();
    allKgrams.merge(auxIdx2.getKgramList()); //not sure if i can do this
    allKgrams.merge(auxIdx3.getKgramList());
    //alphabetize
    allKgrams.sort();

    std::list<std::string> terms;
    for (std::string &kgram : allKgrams) { //for all kgrams in the index
        //if it doesn't fall in any it will break. vulnerability
        if(kgram.size() == 1) terms = auxIdx1.getTerms(kgram);
        else if(kgram.size() == 2) terms = auxIdx2.getTerms(kgram);
        else terms = auxIdx3.getTerms(kgram); //must be three please.

        // write the kgram table entry for this kgram: the byte location of the kgram in the kgramlist file,
        // and the byte location of the terms for the term in the terms file.
        uint32_t kPosition = Reverse(kgramPositions[kgramIndex]); //get first //where the k is located you save.
        kgramTable.write((char*)&kPosition, sizeof(kPosition)); //ideally the pointer to the k
        //writing where first kgram is.

        uint32_t tPosition = Reverse((uint32_t)termsFile.tellp()); //position where you will start writing terms for this kgram
        //also marks where the last term left off. hence where the new term set starts.
        kgramTable.write((char*)&tPosition, sizeof(tPosition)); //write term position.

        WriteTerms(termsFile, terms); //you give it the place to write it and the terms to write.

        kgramIndex++;
    }

    kgramTable.close();
    termsFile.close();
}
