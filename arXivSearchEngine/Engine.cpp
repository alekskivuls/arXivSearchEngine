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

// Default constructors and destructors
Engine::Engine() { 
    // idTable = std::unordered_map<uint32_t, std::string>();
    // idx = InvertedIndex();
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

void Engine::updateTf(std::unordered_map<std::string, uint32_t> &wdt, const std::string &term) {
    if (wdt.find(term) == wdt.end())
        wdt[term] = 1;
    else
        wdt[term] = wdt.at(term) + 1;
}

double_t Engine::calcEucDist(std::unordered_map<std::string, uint32_t> &wdt) { // check for query: fire in yosemite... top rank should be 1.7
    double_t Ld = 0.0;
    for (const std::pair<std::string, uint32_t> &pr : wdt) {
        double_t tf = (double_t)pr.second;
        double_t wgt = 1.0 + log((double_t)tf);
        Ld += (wgt * wgt);
    }

    return sqrt(Ld);
}

void Engine::populateIndex(const boost::filesystem::path &inDir, const boost::filesystem::path &outDir) {
    std::chrono::time_point<std::chrono::system_clock> totalStart, totalEnd;
    totalStart = std::chrono::system_clock::now();

    auto idTable = std::unordered_map<uint32_t, std::string>();
    auto idx = InvertedIndex();
    std::unordered_map<std::string, std::string> cache;
    std::vector<std::string> mPathList;
    getPathNames(inDir, mPathList);

	std::vector<double_t> ld = std::vector<double_t>(); // VOCAB POSITION, SCORE
    ld.reserve(mPathList.size());

    std::sort(mPathList.begin(), mPathList.end());

    uint32_t i = 0;
    for (auto p : mPathList) {
        std::cout << "Processing Article (" << (i++) << "): " << boost::filesystem::path(p).stem() << ".json" << std::endl;

        std::unordered_map<std::string, uint32_t> wdt;

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
                        updateTf(wdt, stemmedToken);
                    }
                    else {
                        std::string total = "";
                        for (auto s : split(token)) {
                            std::string str = std::string(s);
                            PorterStemmer::stem(str);
                            idx.addTerm(str, i, posIndex);
                            updateTf(wdt, str);

                            total += s;
                        }
                        std::string &totalToken = total;
                        //std::string &totalToken = PorterStemmer::stem(total);
                        idx.addTerm(totalToken, i, posIndex);
                        updateTf(wdt, total);
                    }


                    posIndex++;
                }
            }
        }
        /*std::cout << "SIZE OF MAP = " << wdt.size() << std::endl;
		for (auto pr : wdt) {
			std::cout << "first = " << pr.first << " second = " << pr.second << std::endl;
        }*/
		ld.push_back(calcEucDist(wdt));
		wdt = std::unordered_map<std::string, uint32_t>();
    }
	// test write print
	/*std::cout << "TEST PRINT FOR WRITING EUCLIDEAN DISTANCE." << std::endl;
	for (double_t &d : ld) {
		std::cout << d << std::endl;
	}*/
    totalEnd = std::chrono::system_clock::now();
    std::chrono::duration<double> elapsed_seconds = totalEnd-totalStart;
    std::cout << "Total elapsed time for Populate Index: " << elapsed_seconds.count() << "s." << std::endl;

    Serializer::buildIndex(outDir, idx, ld);
    dir = outDir;

	// TEST PRINT READ

	//testRead(outDir.string());
}

std::vector<uint32_t> Engine::rank(std::string input) {
	DiskInvertedIndex dIdx(dir);
    return queryEngine.rankedQuery(input, dIdx);
}

/*void Engine::testRead(const std::string &filepath) {
	DiskInvertedIndex dIdx = DiskInvertedIndex(filepath);
	std::vector<double_t> ld = dIdx.ReadWeights();
	// test write print
	std::cout << "TEST PRINT FOR READING EUCLIDEAN DISTANCE." << std::endl;
	for (double_t &d : ld) {
		std::cout << d << std::endl;
	}
}*/


void Engine::createIndex(const std::string &filepath) {
    auto cwd = boost::filesystem::current_path();
    //std::cout << "Current path is : " << cwd << std::endl;
    boost::filesystem::path dir(filepath);
    populateIndex(dir, cwd);
}

void Engine::loadIndex(const std::string &filepath) {
    dir = filepath;
    //DiskInvertedIndex(boost::filesystem::path(filepath));
}


/*
void Engine::diskWriteTest(const std::string &filepath) { // change this later to a method called: INDEXDISK paul's C:/Users/pkim7/Desktop/corpus
    std::string file = "C:/Users/pkim7/Documents/Visual Studio 2015/Projects/arXivSearchEngine/test/documents/testCorpus"; // // change to your input directory C:\Users\pkim7\Documents\Visual Studio 2015\Projects\arXivSearchEngine\test\documents\testCorpus
    boost::filesystem::path dir(file); // change input back to: filepath
    boost::filesystem::directory_iterator it(dir), eod;

    idTable = std::unordered_map<uint32_t, std::string>();
    idx = InvertedIndex();

    std::string path = "C:/Users/pkim7/Desktop/output"; // change to your output directory
    boost::filesystem::path dirOut(path);
    Engine::populateIndex(dir, dirOut);
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
}*/

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
    auto vocab = getVocab();
    for(auto term : vocab)
        std::cout << term << std::endl;
    std::cout << vocab.size() << std::endl;
}

std::list<std::string> Engine::getVocab() {
    DiskInvertedIndex dIdx = DiskInvertedIndex(dir);
    return dIdx.getVocabList();
}

std::string Engine::stem(std::string &token) {
    return PorterStemmer::stem(token);
}

void Engine::printQuery(std::string &query) {
    auto output = getQuery(query);
    for (auto di : output)
        std::cout << di << '\t';
    std::cout << std::endl << output.size() << std::endl << std::endl;
}

std::vector<std::string> Engine::getQuery(std::string &query) {
    DiskInvertedIndex dIdx = DiskInvertedIndex(dir);
    std::cout << dIdx.ReadVocabStringAtPosition(1) << std::endl;

	KgramIndex kidx(3); // GET REFERENCE TO YOUR 3-GRAM


	/*
	std::istringstream iss(query);
	std::vector<std::string> tokens{ std::istream_iterator<std::string>{iss},
		std::istream_iterator<std::string>{} };
	for (std::string &token : tokens) {
		std::list<std::string> &candidates = KEngine::correctSpelling(token, kidx);
		if (candidates.size() != 1) { // mispelled
			std::cout << "Did you mean: " << candidates.front() << std::endl; // REPLACE LOGIC LATER (FOR ALEKS)
		}
		//KEngine::correctSpelling
		// did you mean?
	}*/

    std::list<DocInfo> output = queryEngine.processQuery(query, dIdx);
    std::vector<std::string> results;
    results.reserve(output.size());
    for (auto di : output)
        results.push_back(std::to_string(di.getDocId()));
    return results;
}
