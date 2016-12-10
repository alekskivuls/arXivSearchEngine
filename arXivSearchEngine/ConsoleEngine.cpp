/**
 *	@Authors: Paul Kim, Aleks Kivuls, Cherie Woo
 *	@Course: CECS 429 - Search Engine Technology
 */

#include <boost/algorithm/string/predicate.hpp>
#include "Engine.h"

int main() {
    //Initialize Engine
    Engine engine;

    // Set file directory
    std::string filepath;
    std::cout << "Enter directory of corpus" << std::endl;
    std::getline(std::cin, filepath);

    //Check if index needs to be loaded
    std::string isLoadingIndex;
    std::cout << "Load index? (y/n)" << std::endl;
    std::getline(std::cin, isLoadingIndex);

    boost::filesystem::path dir(filepath);
    if(isLoadingIndex.front() == 'y') {
        engine.loadIndex(dir);
    } else {
        engine.createIndex(dir);
    }

    // Main loop
    std::string input;
    while (true) {
        std::cout << "Enter a query:" << std::endl;
        std::getline(std::cin, input);

        if (input.compare(":q") == 0) {
            break;
        }
		else if (input.compare(":rocchio") == 0) { // perform rocchio classification
			engine.rocchio();
		}
        else if (boost::algorithm::starts_with(input, ":stem ")) {
            std::string substr = input.substr(6, std::string::npos);
            std::cout << engine.stem(substr) << std::endl;
        }
        else if (input.compare(":vocab") == 0) {
            engine.printVocab();
        }
        else if (input.compare(":authors") == 0) {
            engine.printAuthors();
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
        else if (boost::algorithm::starts_with(input, ":authored ")) {
            std::string author = input.substr(10, std::string::npos);
            engine.printAuthorDocs(author);
        }
        else if(!boost::algorithm::starts_with(input, ":")) { //Query
            engine.printQuery(input);
        }
        else {
            std::cout << "Incorrect command usage" << std::endl;
        }
    }
}
