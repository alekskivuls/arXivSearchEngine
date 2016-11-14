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
#include "DocInfo.h"
#include "Engine.h"

inline uint32_t Reverse(uint32_t value) {
    return (value & 0xFF000000) >> 24 |
		(value & 0x00FF0000) >> 8 |
		(value & 0x0000FF00) << 8 |
		(value & 0x000000FF) << 24;
}

/*
inline uint64_t Reverse(uint64_t value) {
    return (value & 0xFF00000000000000) >> 56 |
		(value & 0x00FF000000000000) >> 40 |
        (value & 0x0000FF0000000000) >> 24 |
        (value & 0x000000FF00000000) >> 8 |
        (value & 0x00000000FF000000) << 8 |
        (value & 0x0000000000FF0000) << 24 |
        (value & 0x000000000000FF00) << 40 |
        (value & 0x00000000000000FF) << 56;
}*/

// Default constructors and destructors
Engine::Engine() { 
    idTable = std::unordered_map<uint32_t, std::string>();
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

void Engine::populateIndex(const boost::filesystem::path &dir, InvertedIndex &idx, std::unordered_map<uint32_t, std::string> &idTable) {
    std::chrono::time_point<std::chrono::system_clock> totalStart, totalEnd;
    totalStart = std::chrono::system_clock::now();

    std::unordered_map<std::string, std::string> cache;
    boost::filesystem::directory_iterator it(dir), eod;
    std::vector<std::string> mPathList;
    getPathNames(dir, mPathList);

	ld = std::vector<double_t>(); // VOCAB POSITION, SCORE
	ld.reserve(mPathList.size());

    std::sort(mPathList.begin(), mPathList.end());

    uint32_t i = 0;
    for (auto p : mPathList) {
        std::cout << "Processing Article (" << (i++) << "): " << boost::filesystem::path(p).stem() << ".json" << std::endl;
		ld.push_back(0.0);

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
                uint32_t posIndex = 0;
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

    idTable = std::unordered_map<uint32_t, std::string>();
    idx = InvertedIndex();
    Engine::populateIndex(dir, idx, idTable);
    std::cout << "idx size = " << idx.getTermCount() << '\n';

    //printIndex();
}

void Engine::diskWriteTest(const std::string &filepath) { // change this later to a method called: INDEXDISK paul's C:/Users/pkim7/Desktop/corpus
    std::string file = "C:/Users/pkim7/Documents/Visual Studio 2015/Projects/arXivSearchEngine/test/documents/testCorpus"; // // change to your input directory C:\Users\pkim7\Documents\Visual Studio 2015\Projects\arXivSearchEngine\test\documents\testCorpus
    boost::filesystem::path dir(file); // change input back to: filepath
    boost::filesystem::directory_iterator it(dir), eod;

    idTable = std::unordered_map<uint32_t, std::string>();
    idx = InvertedIndex();
    Engine::populateIndex(dir, idx, idTable);
    std::cout << "idx size = " << idx.getTermCount() << '\n';

    std::string path = "C:/Users/pkim7/Desktop/output"; // change to your output directory
    boost::filesystem::path dirOut(path);
    Serializer::buildIndex(dirOut, idx);

    std::cout << "Printing index: " << std::endl;
    std::cout << "size of index: " << idx.getIndex().size() << std::endl;

    DiskInvertedIndex auxIdx = DiskInvertedIndex(dirOut);


    std::string input = "park"; // "breed" "explore" "park"
	//std::string input = "mannual";
    std::string stemmedToken = PorterStemmer::stem(input);


    std::list<DocInfo> postingsFile = auxIdx.GetPostings(stemmedToken);
    std::list<DocInfo> postingsMemory = idx.getPostings(stemmedToken);

    if (postingsFile.size() == postingsMemory.size())
        std::cout << "postings lists are same size!" << std::endl;
    else
        std::cout << "postings lists are NOT the same size... you done goofed." << std::endl;

    std::cout << "file size: " << postingsFile.size() << std::endl; // THIS IS STATING ERROR
    std::cout << "memory size: " << postingsMemory.size() << std::endl;

	if (postingsFile.size() == postingsMemory.size()) {
		std::list<DocInfo>::iterator iter = postingsFile.begin();
		for (const DocInfo &doc : postingsMemory) {
            if ((*iter).getPositions().size() != doc.getPositions().size()) {
                std::cout << "FILE DocInfo.getPositions.size(" << (*iter).getPositions().size() <<
                    ") != Memory DocInfo.getPositions.size(" << doc.getPositions().size() << ')' << std::endl;
				break;
			}
			else {
				std::cout << "MEMORY DocInfo ID(" << doc.getDocId() << ") compared to ";
				std::cout << "File DocInfo ID(" << (*iter).getDocId() << ')' << std::endl;
			}

            std::list<uint32_t>::iterator posFile = (*iter).getPositions().begin();
            for(const int temp : doc.getPositions()) {
                std::cout << "VALUE: " << temp << std::endl;
            }

			for (auto posMemory : doc.getPositions()) {
				if (posMemory != *posFile)
					std::cout << "File Pos(" << *posFile << ") != Memory Pos(" << posMemory << ')' << std::endl;
				else 
					std::cout << "File Pos(" << *posFile << ") == Memory Pos(" << posMemory << ')' << std::endl;

				++posFile;
			}
			++iter;
		}
	}
	


    auxIdx.mPostings.close();
    auxIdx.mPostings.open(boost::filesystem::path(auxIdx.mPath).append("/postings.bin", boost::filesystem::path::codecvt()).string(),
                          std::ios_base::in | std::ios_base::binary);
    std::cout << "READING FROM FILE: " << std::endl;
    uint32_t val, itr = 0, count = 0, total = 0;
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
    std::cout << std::endl;
    auxIdx.mPostings.close();
}

void Engine::printIndex() {
    typedef std::pair<std::string, std::list<DocInfo>> pair;
    for (const pair &p : idx.getIndex()) {
        std::cout << "Term (" << p.first << ") found in the following documents:" << std::endl;
        for (DocInfo doc : p.second) { // list of positions
            std::cout << "Document id " << doc.getDocId() << " positions(" << doc.getPositions().size() << "): " << std::endl;
            for (uint32_t &pos : doc.getPositions())
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
