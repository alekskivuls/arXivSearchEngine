#define BOOST_SPIRIT_THREADSAFE
#include <boost/algorithm/string/predicate.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/filesystem.hpp>
#include "DiskInvertedIndex.h"
#include <boost/foreach.hpp>
#include "PorterStemmer.h"
#include "InvertedIndex.h"
#include <unordered_set>
#include "Serializer.h"
#include "Tokenizer.h"
#include "QEngine.h"
#include "DocInfo.h"
#include "Engine.h"

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

// Default constructors and destructors
Engine::Engine() { 
    idTable = std::unordered_map<unsigned int, std::string>();
    idx = InvertedIndex();
}

void Engine::getPathNames(const boost::filesystem::path &directory, std::vector<std::string> &mPathList) {
    boost::filesystem::directory_iterator end_itr;
    std::unordered_set<std::string> fileSet;

    for (boost::filesystem::directory_iterator itr(directory); itr != end_itr; ++itr) {
        if (is_regular_file(itr->path())) {
            std::string s = itr->path().string();
            if (boost::algorithm::ends_with(s, ".json")) {
                std::replace(s.begin(), s.end(), '\\', '/');
                //std::cout << s << '\n';
                fileSet.insert(s);
            }
        }
    }

    mPathList.resize(fileSet.size());
    int i = 0;
    for (auto s : fileSet)
        mPathList[i++] = s;
}

std::vector<std::string> split(std::string token) {
    std::vector<std::string> vect;
    std::stringstream ss(token);

    char c;
    std::string str = "";
    while (ss >> c)
    {
        if (c != '-') // if NOT hyphen
            str += c;
        else {
            vect.push_back(str);
            str = "";
        }
    }
    vect.push_back(str);

    return vect;
}

void Engine::populateIndex(const boost::filesystem::path &dir, InvertedIndex &idx, std::unordered_map<unsigned int, std::string> &idTable) {

    std::chrono::time_point<std::chrono::system_clock> totalStart, totalEnd;
    totalStart = std::chrono::system_clock::now();

    std::unordered_map<std::string, std::string> cache;
    boost::filesystem::directory_iterator it(dir), eod;
    std::vector<std::string> mPathList;
    getPathNames(dir, mPathList);

    std::sort(mPathList.begin(), mPathList.end());

    int i = 0;
    for (auto p : mPathList) {
        std::cout << "Processing Article (" << (i++) << "): " << boost::filesystem::path(p).stem() << ".json" << std::endl;

        // reads json file into stringstream and populates a json tree
        std::ifstream file(p);
        std::stringstream ss;
        ss << file.rdbuf();
        file.close();

        boost::property_tree::ptree pt;
        boost::property_tree::read_json(ss, pt);
        boost::filesystem::path dir(p);
        (idTable)[i] = dir.stem().string();

        //std::cout << "json to map...\n";
        // iterate through .json tree
        BOOST_FOREACH(boost::property_tree::ptree::value_type& pair, pt) {
            if (pair.first == "body") { // if author... get json array and process the authors as well. || pair.first == "title"
                std::string input = pair.second.get_value<std::string>();
                std::transform(input.begin(), input.end(), input.begin(), ::tolower);


                Tokenizer tkzr(input);
                std::string token;
                token.reserve(200);
                unsigned int posIndex = 0;
                bool hyphen = false;
                while (tkzr.nextToken(token, hyphen)) {
                    // while not end of file.
                    // Get stem the token or retrieve the value from a cache
                    if (!hyphen) {
                        std::string stemmedToken = (cache.find(token) != cache.end())
                                ? cache[token] : PorterStemmer::stem(token);
                        idx.addTerm(stemmedToken, i, posIndex); // stemmedToken
                    }
                    else {
                        std::string total = "";
                        for (auto s : split(token)) {
                            idx.addTerm(PorterStemmer::stem(s), i, posIndex);
                            total += s;
                        }
                        idx.addTerm(PorterStemmer::stem(total), i, posIndex);
                    }

                    posIndex++;
                }
            }
        }
    }
    totalEnd = std::chrono::system_clock::now();
    std::chrono::duration<double> elapsed_seconds = totalEnd-totalStart;
    std::cout << "Total elapsed time for Populate Index: " << elapsed_seconds.count() << "s." << std::endl;
}

void Engine::index(const std::string &filepath) {
    boost::filesystem::path dir(filepath);
    boost::filesystem::directory_iterator it(dir), eod;

    idTable = std::unordered_map<unsigned int, std::string>();
    idx = InvertedIndex();
    Engine::populateIndex(dir, idx, idTable);
    std::cout << "idx size = " << idx.getTermCount() << '\n';

    //printIndex();
}

void Engine::diskWriteTest(const std::string &filepath) { // change this later to a method called: INDEXDISK
    std::string file = "C:/Users/pkim7/Desktop/corpus"; // // change to your input directory
    boost::filesystem::path dir(file); // change input back to: filepath
    boost::filesystem::directory_iterator it(dir), eod;

    idTable = std::unordered_map<unsigned int, std::string>();
    idx = InvertedIndex();
    Engine::populateIndex(dir, idx, idTable);
    std::cout << "idx size = " << idx.getTermCount() << '\n';

    std::string path = "C:/Users/pkim7/Desktop/output"; // change to your output directory
    boost::filesystem::path dirOut(path);
    Serializer::buildIndex(dirOut, idx);

    std::cout << "Printing index: " << std::endl;
    std::cout << "size of index: " << idx.getIndex().size() << std::endl;

    //Engine::printIndex();

    DiskInvertedIndex auxIdx = DiskInvertedIndex(dirOut);

    /*for (auto ele : idx.getIndex()) {
        std::vector<DocInfo>
        VocabEntry entry = auxIdx.BinarySearchVocabulary(ele.first);
        if (entry.PostingPosition != -1 && entry.StringPosition != -1)
            return ReadPostingsFromFile(mPostings, entry.PostingPosition);
        return std::vector<DocInfo>();
    }


    std::cout << "entry.PostingPosition != -1 && entry.StringPosition != -1 is "
        << (entry.PostingPosition != -1 && entry.StringPosition != -1) << std::endl;
    */


    PorterStemmer stemmer;
    std::string input = "and"; // "breed" "explore"
    std::string stemmedToken = stemmer.stem(input);
    //std::string stemmedToken = "mannual";
    auto ve = auxIdx.BinarySearchVocabulary(stemmedToken);
    std::cout << "DOES VE EXIST FFS?\nV POSITION: " << ve.StringPosition << std::endl;
    std::cout << "P POSITION: " << ve.PostingPosition << std::endl;
    std::cout << "Reverse(ve.StringPosition): " << Reverse((uint32_t)ve.StringPosition) << std::endl;
    std::cout << "Reverse(ve.PostingPosition): " << Reverse((uint32_t)ve.PostingPosition) << std::endl;

    std::cout << std::endl << std::endl<< "PRINTING VOCAB TABLE: " << std::endl; // should NOT be emtpy
    typedef const std::pair<std::string, std::list<DocInfo>> pair;
    for (auto vocabE : auxIdx.mVocabTable) { // pair &element : auxIdx.getIndex()
        std::cout << "V POSITION: " << vocabE.StringPosition << std::endl; // Reverse(vocabE.StringPosition)
        std::cout << "P POSITION: " << vocabE.PostingPosition << std::endl; // Reverse(vocabE.PostingPosition)
    }



    std::vector<DocInfo> postingsFile = auxIdx.GetPostings(stemmedToken); //LKJASDJLKASJDKSJDKJKLDASDLJLKJDKASLJJKLADKLJDKSAJDKLASJDLKSJDLKSAJLKASDJ
    std::list<DocInfo> postingsMemory = idx.getPostings(stemmedToken);

    if (postingsFile.size() == postingsMemory.size())
        std::cout << "postings lists are same size!" << std::endl;
    else
        std::cout << "postings lists are NOT the same size... you done goofed." << std::endl;

    std::cout << "file size: " << postingsFile.size() << std::endl;
    std::cout << "memory size: " << postingsMemory.size() << std::endl;

    unsigned char i = 0;
    for (const DocInfo &doc : postingsMemory) {
        std::cout << "MEMORY DocInfo ID(" << doc.getDocId() << ") compared to ";

        if (i < postingsFile.size())
            std::cout << "FILE DocInfo ID(" << postingsFile[i++].getDocId() << ")" << std::endl;
        else
            std::cout << "No more DocInfos" << std::endl;
    }


    auxIdx.mPostings.close();
    auxIdx.mPostings.open(boost::filesystem::path(auxIdx.mPath).append("/postings.bin", boost::filesystem::path::codecvt()).string(),
                          std::ios_base::in | std::ios_base::binary);
    std::cout << "READING FROM FILE: " << std::endl;
    uint32_t val, itr = 0, count = 0, total = 0; // when reading... use int32_t or uint32_t... use the safer of 2 or the one that works.
    while (val = auxIdx.ReadInt(auxIdx.mPostings)) {
        std::cout << "SIZE(" << val << ")" << std::endl;
        count = val;
        total += count;

        for (itr = 0; itr < count; ++itr) {
            val = auxIdx.ReadInt(auxIdx.mPostings);
            std::cout << val << ' ';
        }
        std::cout << std::endl;
    }
    std::cout << "total should be (2000ish): " << total << std::endl;
    std::cout << std::endl;
    auxIdx.mPostings.close();
}

void Engine::printIndex() {
    typedef std::pair<std::string, std::list<DocInfo>> pair;
    for (const pair &p : idx.getIndex()) {
        std::cout << "Term (" << p.first << ") found in the following documents:" << std::endl;
        for (DocInfo doc : p.second) { // list of positions
            std::cout << "Document id " << doc.getDocId() << " positions(" << doc.getPositions().size() << "): " << std::endl;
            for (unsigned int &pos : doc.getPositions())
                std::cout << pos << " ";
            std::cout << std::endl;
        }
        std::cout << std::endl;
    }
}

void Engine::printVocab() {
    idx.vocab();
}


std::list<std::string> Engine::getVocab() {
    return idx.getVocabList();
}

std::string Engine::stem(std::string &token) {
    return PorterStemmer::stem(token);
}

void Engine::printQuery(std::string &query) {
    std::list<DocInfo> output = queryEngine.processQuery(query, idx);
    for (auto di : output)
        std::cout << idTable.at(di.getDocId()) << '\t';
    std::cout << std::endl << output.size() << std::endl;
    std::cout << std::endl;
}

std::vector<std::string> Engine::getQuery(std::string &query) {
    std::list<DocInfo> output = queryEngine.processQuery(query, idx);
    std::vector<std::string> results;
    results.reserve(output.size());
    for (auto di : output)
        results.push_back(idTable.at(di.getDocId()));
    return results;
}
