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
	engine.index(filepath);

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
			engine.index(filepath);
		}
        else if (boost::algorithm::starts_with(input, ":kgram ")) {
            std::string token = input.substr(7, std::string::npos);
            engine.correctSpelling(token);
        }
		else if(!boost::algorithm::starts_with(input, ":")) { //Query
			engine.printQuery(input);
		}
		else {
			std::cout << "Incorrect command usage" << std::endl;
		}
	}
}
