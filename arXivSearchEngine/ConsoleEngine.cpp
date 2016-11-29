/**
 *	@Authors: Paul Kim, Aleks Kivuls, Cherie Woo
 *	@Course: CECS 429 - Search Engine Technology
 */

#include <boost/algorithm/string/predicate.hpp>
#include "Engine.h"

/*void pktest() {
    // Paul's test code. just ignore it.
    //Initialize Engine
    Engine engine;

    // Set file directory
    //std::string filepath;
    std::cout << "Writing to Disk: " << std::endl;
    //std::getline(std::cin, filepath);
    engine.diskWriteTest("hello"); // should create 3 files
}*/

int main() {
    //Initialize Engine
    Engine engine;


    // Set file directory
    std::string filepath;
    std::cout << "Enter directory of corpus" << std::endl;
    std::getline(std::cin, filepath);
	//engine.createIndex(filepath);
    engine.loadIndex(filepath);

	//engine.createIndex(R"(C:\Users\pkim7\Documents\Visual Studio 2015\Projects\arXivSearchEngine\test\documents\testCorpus)");

	//system("pause");

	//engine.loadIndex(R"(C:\Users\pkim7\Documents\Visual Studio 2015\Projects\arXivSearchEngine\arXivSearchEngine)");
	//return 0;
	
    /*std::unordered_map<uint32_t, std::string> idTable = DiskInvertedIndex::ReadIdTableFromFile(boost::filesystem::path(filepath));
	for (auto pair : idTable) {
		std::cout << "pair.first = " << pair.first << " ";
		std::cout << "pair.second = " << pair.second << std::endl;
    }*/

    // Main loop
    std::string input;
    while (true) {
        std::cout << "Enter a query:" << std::endl;
        std::getline(std::cin, input);

        if (input.compare(":q") == 0) {
            break;
        }
        else if (boost::algorithm::starts_with(input, ":stem ")) {
            std::string substr = input.substr(6, std::string::npos);
            std::cout << engine.stem(substr) << std::endl;
        }
        else if (input.compare(":vocab") == 0) {
            engine.printVocab();
        }
        else if (boost::algorithm::starts_with(input, ":index ")) {
            filepath = input.substr(7, std::string::npos);
            engine.createIndex(filepath);
        }
        else if (boost::algorithm::starts_with(input, ":load ")) {
            filepath = input.substr(6, std::string::npos);
            engine.loadIndex(filepath);
        }
		else if (boost::algorithm::starts_with(input, ":rank ")) { // REMOVE THIS LATER
            std::string query = input.substr(6, std::string::npos);
            engine.printRank(query);
		}
        else if(!boost::algorithm::starts_with(input, ":")) { //Query
            engine.printQuery(input);
        }
        else {
            std::cout << "Incorrect command usage" << std::endl;
        }
    }
}
