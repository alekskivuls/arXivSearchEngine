#define BOOST_SPIRIT_THREADSAFE
#include <boost/algorithm/string/predicate.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/filesystem.hpp>
#include "DiskInvertedIndex.h"
#include "PorterStemmer.h"
#include "InvertedIndex.h"
#include <unordered_set>
#include "Serializer.h"
#include "Tokenizer.h"
#include "DocInfo.h"
#include "Engine.h"
#include "KSerializer.h"
#include "KDeserializer.h"

// Default constructors and destructors
Engine::Engine() {}

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

    idTable = std::unordered_map<uint32_t, std::string>();
    auto idx = InvertedIndex();
    std::unordered_map<std::string, std::string> cache;
    std::vector<std::string> mPathList;
    getPathNames(inDir, mPathList);

    std::vector<double_t> ld = std::vector<double_t>(); // VOCAB POSITION, SCORE
    ld.reserve(mPathList.size());

    std::sort(mPathList.begin(), mPathList.end());

    uint32_t i = 0;
    for (auto p : mPathList) {
        std::cout << "Processing Article (" << (i) << "): " << boost::filesystem::path(p).stem() << ".json" << std::endl;
		//ld.push_back(0.0);
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
        for (auto& pair : pt) {
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
						kIdx1.addTerm(token);
						kIdx2.addTerm(token);
						kIdx3.addTerm(token);

                        std::string stemmedToken = (cache.find(token) != cache.end())
                                ? cache[token] : PorterStemmer::stem(token);
                        idx.addTerm(stemmedToken, i, posIndex); // stemmedToken
                        updateTf(wdt, stemmedToken);
                    }
                    else {
                        std::string total = "";
                        for (auto s : split(token)) {
							kIdx1.addTerm(s);
							kIdx2.addTerm(s);
							kIdx3.addTerm(s);
							total += s;

                            std::string str = std::string(s);
                            idx.addTerm(PorterStemmer::stem(str), i, posIndex);
                            updateTf(wdt, str);
                        }
                        std::string &totalToken = total;
                        //std::string &totalToken = PorterStemmer::stem(total);
                        idx.addTerm(PorterStemmer::stem(totalToken), i, posIndex);
                        updateTf(wdt, totalToken);

                        kIdx1.addTerm(totalToken);
                        kIdx2.addTerm(totalToken);
                        kIdx3.addTerm(totalToken);
                    }


                    posIndex++;
                }
            } else if (pair.first == "title") {

            } else if(pair.first == "author") {
                std::string author = pair.second.get_value<std::string>();
                std::transform(author.begin(), author.end(), author.begin(), ::tolower);
                idx.addAuthorDoc(author, i);
            }
        }
        /*std::cout << "SIZE OF MAP = " << wdt.size() << std::endl;
        for (auto pr : wdt) {
            std::cout << "first = " << pr.first << " second = " << pr.second << std::endl;
        }*/
		ld.push_back(calcEucDist(wdt));
		wdt = std::unordered_map<std::string, uint32_t>();
        ++i;
    }
    // test write print
    /*std::cout << "TEST PRINT FOR WRITING EUCLIDEAN DISTANCE." << std::endl;
    for (double_t &d : ld) {
        std::cout << d << std::endl;
    }*/
    totalEnd = std::chrono::system_clock::now();
    std::chrono::duration<double> elapsed_seconds = totalEnd-totalStart;
    std::cout << "Total elapsed time for Populate Index: " << elapsed_seconds.count() << "s." << std::endl;

    Serializer::buildIndex(outDir, idx, idTable, ld); // populates all .bin files
    KSerializer::buildIndex(outDir, kIdx1, kIdx2, kIdx3);
    dIdx = DiskInvertedIndex(boost::filesystem::path(outDir));
}

void Engine::printRank(std::string &query) {
    auto list = getRank(query);
    for(auto element : list)
        std::cout << "doc name: " << getArticleName(element.first)  << " ->\tscore: " << element.second << std::endl;
}

std::vector<std::pair<uint32_t, double_t>> Engine::getRank(std::string &query) {
    return queryEngine.rankedQuery(query, dIdx, kIdx3);
}

void Engine::createIndex(const boost::filesystem::path &filepath) {
    auto cwd = boost::filesystem::current_path();
    //std::cout << "Current path is : " << cwd << std::endl;
    createIndex(filepath, cwd);
}

void Engine::createIndex(const boost::filesystem::path &inDir, const boost::filesystem::path &outDir) {
    populateIndex(inDir, outDir);
}

void Engine::loadIndex(const boost::filesystem::path &filepath) {
    dIdx = DiskInvertedIndex(boost::filesystem::path(filepath));
    KDeserializer kds(filepath);
    kds.toKgramIndex(kIdx1, kIdx2, kIdx3);
	idTable = DiskInvertedIndex::ReadIdTableFromFile(boost::filesystem::path(filepath));
}

std::string Engine::getArticleName(const uint32_t &docid) {
	return idTable.at(docid);
}

/*
void Engine::printIndex() {
    typedef std::pair<std::string, std::list<DocInfo>> pair;
    for (const pair &p : dIdx.getIndex()) {
        std::cout << "Term (" << p.first << ") found in the following documents:" << std::endl;
        for (DocInfo doc : p.second) { // list of positions
            std::cout << "Document id " << doc.getDocId() << " positions(" << doc.getPositions().size() << "): " << std::endl;
            for (uint32_t &pos : doc.getPositions())
                std::cout << pos << " ";
            std::cout << std::endl;
        }
        std::cout << std::endl;
    }
}*/

void Engine::printVocab() {
    auto vocab = getVocab();
    for(auto term : vocab)
        std::cout << term << std::endl;
    std::cout << vocab.size() << std::endl;
}

std::list<std::string> Engine::getVocab() {
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
    std::list<DocInfo> output = queryEngine.processQuery(query, dIdx, kIdx1, kIdx2, kIdx3);
    std::vector<std::string> results;
    results.reserve(output.size());
    for (auto di : output)
        results.push_back(getArticleName(di.getDocId()));

    return results;
}

std::list<std::string> Engine::getAuthors() {
    return dIdx.getAuthorList();
}

void Engine::printAuthors() {
    auto authors = getAuthors();
    for(auto author : authors) {
        std::cout << author << std::endl;
    }
    std::cout << authors.size() << std::endl;
}

std::list<uint32_t> Engine::getAuthorDocs(const std::string &author) {
    return dIdx.getAuthorDocs(author);
}

void Engine::printAuthorDocs(const std::string &author) {
    auto authorDocs = getAuthorDocs(author);
    for(auto docId : authorDocs) {
        std::cout << getArticleName(docId) << std::endl;
    }
    std::cout << authorDocs.size() << std::endl;
}
