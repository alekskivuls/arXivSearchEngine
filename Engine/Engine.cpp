#include <boost/algorithm/string/predicate.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/lambda/lambda.hpp>
#include <boost/filesystem.hpp>
#include <boost/foreach.hpp>
#include "PorterStemmer.h"
#include "InvertedIndex.h"
#include <unordered_map>
#include <unordered_set>
#include "Tokenizer.h"
#include "QEngine.h"
#include "DocInfo.h"
#include <algorithm>
#include "Engine.h"
#include <stdio.h>
#include <string>
#include <fstream>
#include <vector>
#include <list>

// Default constructors and destructors
Engine::Engine() { 
	idTable = new std::unordered_map<unsigned int, std::string>();
	idx = new InvertedIndex();
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

	void Engine::populateIndex(const boost::filesystem::path &dir, InvertedIndex *& idx, std::unordered_map<unsigned int, std::string> *idTable) {

		std::chrono::time_point<std::chrono::system_clock> totalStart, totalFinish, start, finish;
		totalStart = std::chrono::system_clock::now();
		double elapsedTime = 0.0, stemTime = 0.0, fileReadTime = 0.0, treeTime = 0.0, lowerTime = 0.0, stringTime = 0.0;

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
			(*idTable)[i] = dir.stem().string();

			//std::cout << "json to map...\n";
			// iterate through .json tree
			BOOST_FOREACH(boost::property_tree::ptree::value_type& pair, pt) {
				if (pair.first == "body") { // if author... get json array and process the authors as well. || pair.first == "title"
					std::string input = pair.second.get_value<std::string>();
					std::transform(input.begin(), input.end(), input.begin(), ::tolower);


					Tokenizer tkzr(input);
					std::string token;
					token.reserve(200);
					int posIndex = 0;
					bool hyphen = false;
					while (tkzr.nextToken(token, hyphen)) {
						// while not end of file.
						// Get stem the token or retrieve the value from a cache
						//start = std::chrono::system_clock::now();
						token.reserve(200);
						if (!hyphen) {
							std::string stemmedToken = (cache.find(token) != cache.end())
								? cache[token] : PorterStemmer::stem(token);
							idx->addTerm(stemmedToken, i, posIndex); // stemmedToken
						}
						else {
							std::string total = "";
							for (auto s : split(token)) {
								idx->addTerm(PorterStemmer::stem(s), i, posIndex);
								total += s;
							}
							idx->addTerm(PorterStemmer::stem(total), i, posIndex);
						}

						posIndex++;
					}
				}
			}
			//system("pause");
		}

		totalFinish = std::chrono::system_clock::now();
		std::chrono::duration<double> elapsed_seconds = totalFinish-totalStart;

		//std::cout << "Total elapsed time for string time: " << stringTime << " ms." << std::endl;
		//std::cout << "Total elapsed time for File Read Time: " << fileReadTime << " ms." << std::endl;
		//std::cout << "Total elapsed time to fill Json Tree: " << treeTime << " ms." << std::endl;
		//std::cout << "Total elapsed time for Porter Stemmer: " << stemTime << " ms." << std::endl;
		std::cout << "Total elapsed time for Populate Index: " << elapsed_seconds.count() << "s." << std::endl;
	}

void Engine::index(const std::string &filepath) {
	boost::filesystem::path dir(filepath);
	boost::filesystem::directory_iterator it(dir), eod;

	delete idx;
	delete idTable;
	idTable = new std::unordered_map<unsigned int, std::string>();
	idx = new InvertedIndex();
	Engine::populateIndex(dir, idx, idTable);
	std::cout << "idx size = " << idx->getTermCount() << '\n';
}

void Engine::printVocab() {
	idx->vocab();
}

std::string Engine::stem(std::string &token) {
	return PorterStemmer::stem(token);
}

void Engine::printQuery(std::string &query) {
std::list<DocInfo> output = queryEngine.processQuery(query, idx);
			for (auto di : output)
				std::cout << idTable->at(di.getDocId()) << '\t';
			std::cout << std::endl << output.size() << std::endl;
			std::cout << std::endl;
}
