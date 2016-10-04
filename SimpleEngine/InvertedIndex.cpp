#include "InvertedIndex.h"
#include <algorithm>
#include <iostream>
InvertedIndex::InvertedIndex() { }

/*
 * Returns whether the postings list associated with the term exists in the inverted index. 
 * Throws and out of range exception if the key (term) is not found in the index.
 */
bool InvertedIndex::hasPostings(const std::string &term) {
	return _mIndex.find(term) != _mIndex.end();
}

/*
 * Returns the postings list from the inverted index.
 * Throws and out of range exception if the key (term) is not found in the index.
 */
std::list<DocInfo> InvertedIndex::getPostings(const std::string &term) const {
	return _mIndex.at(term);
}

/* 
 * Adds the "term", found in the corresponding "docName" at position "int pos", 
 * into the inverted index.
 * 
 * Side note: maybe we should store all of the information as compressed bytes 
 * later? this way, we can put more on RAM
 */
void InvertedIndex::addTerm(const std::string &term, const std::string &docName, const int &pos) {
	if (_mIndex.find(term) == _mIndex.end()) { // TERM DOES NOT EXIST
		std::list<DocInfo> postingsList; // create postings list

		DocInfo DI(docName); // create new DocInfo
		DI.addPosition(pos);
		
		postingsList.push_back(DI); // insert into postings list

		_mIndex[term] = postingsList; // populate
	}
	else { // TERM DOES EXIST
		std::list<DocInfo> postingsList = _mIndex[term];

		// Search list of Docs associated with "term"
		auto it = postingsList.begin();
		for (; it != postingsList.end(); ++it) {
			if ((*it).getDocName() == docName) {
				(*it).addPosition(pos); // DOCNAME DOES EXIST. UPDATE/ INSERT POS INTO EXISTING DOCLIST.
				_mIndex[term] = postingsList;
				return;
			}
			else if ((*it).getDocName() > docName) // i did not check for edge case... if docList is empty
				break;
		}
		DocInfo DI(docName);
		DI.addPosition(pos);
		postingsList.insert(it, DI);
		_mIndex[term] = postingsList;
	}
}

/*
 * This method prints all terms and DocInfos associated to each term to the console.
 */
void InvertedIndex::printIndex() {
	for (auto pair : _mIndex) {
		std::cout << "Term: " << pair.first << "\n\t";
		for (auto inner : pair.second) { // list<DocInfo>
			std::cout << inner.getDocName() << ":\n\t\t";
			for (auto pos : inner.getPositions()) {
				std::cout << pos << ' ';
			}
			std::cout << '\n';
		}
		std::cout << '\n';
	}
	std::cout << '\n';
}

/*
 * returns the number of unique terms that exist in the inverted index.
 */
unsigned int InvertedIndex::getTermCount() const {
	return _mIndex.size();
}