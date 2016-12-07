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
#include "ClassifierEngine.h"
#include "KSerializer.h"
#include "KDeserializer.h"

// Default constructors and destructors
ClassifierEngine::ClassifierEngine() {
    // idTable = std::unordered_map<uint32_t, std::string>();
    // idx = InvertedIndex();
}

void ClassifierEngine::getPathNames(const boost::filesystem::path &directory, std::vector<std::string> &mPathList) {
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

void ClassifierEngine::populateIndex(boost::filesystem::path &inDir, boost::filesystem::path &outDir) {
    std::chrono::time_point<std::chrono::system_clock> totalStart, totalEnd;
    totalStart = std::chrono::system_clock::now(); //leaving timer

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
            if (pair.first == "author") {
                std::string author = pair.second.get_value<std::string>();
                std::transform(author.begin(), author.end(), author.begin(), ::tolower);

                idx.addAuthorDoc(author, i);
            }
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
                    std::string total = "";
//                    for (auto s : split(token)) {
//                        kIdx1.addTerm(s);
//                        kIdx2.addTerm(s);
//                        kIdx3.addTerm(s);
//                        total += s;

//                        std::string str = std::string(s);
//                        idx.addTerm(PorterStemmer::stem(str), i, posIndex);
//                        updateTf(wdt, str);
//                    }
                    std::string &totalToken = total;
                    //std::string &totalToken = PorterStemmer::stem(total);
                    idx.addTerm(PorterStemmer::stem(totalToken), i, posIndex);
//                    updateTf(wdt, totalToken);

//                    kIdx1.addTerm(totalToken);
//                    kIdx2.addTerm(totalToken);
//                    kIdx3.addTerm(totalToken);
                    posIndex++;
                }
            }
        }
        /*std::cout << "SIZE OF MAP = " << wdt.size() << std::endl;
        for (auto pr : wdt) {
            std::cout << "first = " << pr.first << " second = " << pr.second << std::endl;
        }*/
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

    dir = outDir;
}

double ClassifierEngine::featureSelect(double classTerm, double noClassTerm, double noTermClass, double noTermNoClass){
    double totalDoc = classTerm + noClassTerm + noTermClass + noTermNoClass;
    double classTermCal = (classTerm/totalDoc)*std::log2((classTerm*totalDoc)/((classTerm+noClassTerm)*(classTerm+noTermClass)));
    double noClassTermCal = (noClassTerm/totalDoc)*std::log2((noClassTerm*totalDoc)/((classTerm+noClassTerm)*(noClassTerm+noTermNoClass)));
    double noTermClassCal = (noTermClass/totalDoc)*std::log2((noTermClass*totalDoc)/((noTermClass+classTerm)*(noTermClass+noTermNoClass)));
    double noTermNoClassCal = (noTermNoClass/totalDoc)*std::log2((noTermNoClass*totalDoc)/((noTermNoClass+noClassTerm)*(noTermNoClass+noTermClass)));
    return classTermCal + noClassTermCal + noTermClassCal + noTermNoClassCal;
}
