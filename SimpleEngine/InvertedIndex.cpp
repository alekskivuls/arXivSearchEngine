#include "InvertedIndex.h"
#include <algorithm>
#include <iostream>
InvertedIndex::InvertedIndex() { }

/*
 * Returns whether the postings list associated with the term exists in the inverted index. 
 * Throws and out of range exception if the key (term) is not found in the index.
 */
bool InvertedIndex::hasPostings(const std::string &term) const {
	return _mIndex.find(term) != _mIndex.end();
}

/*
 * Returns the postings list from the inverted index.
 * Throws and out of range exception if the key (term) is not found in the index.
 */
std::list<DocInfo> InvertedIndex::getPostings(const std::string &term) const {
	std::list<DocInfo> empty;
	if (hasPostings(term))
		return _mIndex.at(term);
	else
		return empty;
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
		DI.getPositions().push_back(pos);
		//DI.addPosition(pos);
		
		postingsList.push_back(DI); // insert into postings list

		_mIndex[term] = postingsList; // populate
	}
	else { // TERM DOES EXIST DOC ALREADY HAD TERM
		std::list<DocInfo> &postingsList = _mIndex[term];
		if (postingsList.back().getDocName() == docName) {
			//postingsList.back().addPosition(pos);
			postingsList.back().getPositions().push_back(pos);
		}
		else { // TERM DOES EXIST BUT DOC DOES NOT CONTAIN TERM
			DocInfo DI(docName);
			DI.addPosition(pos);
			postingsList.push_back(DI);
			//_mIndex[term] = postingsList;
		}
	}
}

void InvertedIndex::getVocabList() const
{

}

/*
 * This method prints all terms and DocInfos associated to each term to the console.
 */
void InvertedIndex::vocab() const {
	for (auto pair : _mIndex) 
		std::cout << pair.first << '\n';
	std::cout << getTermCount() << '\n';
}

/*
 * returns the number of unique terms that exist in the inverted index.
 */
unsigned int InvertedIndex::getTermCount() const {
	return _mIndex.size();
}