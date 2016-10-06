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
 * Adds the "term", found in the corresponding "docId" at position "int pos", 
 * into the inverted index.
 * 
 * Side note: maybe we should store all of the information as compressed bytes 
 * later? this way, we can put more on RAM
 */
void InvertedIndex::addTerm(const std::string &term, const unsigned int &docId, const int &pos) {
	if (_mIndex.find(term) == _mIndex.end()) { // TERM DOES NOT EXIST
		std::list<DocInfo> postings; // create postings list

		postings.push_back(DocInfo(docId)); // create new DocInfo
		postings.back().getPositions().push_back(pos); // insert into postings list

		_mIndex[term] = postings; // populate
	}
	else { // TERM DOES EXIST
		std::list<DocInfo> &postings = _mIndex[term]; // get postings of existing term

		if (postings.back().getDocId() == docId) { // TERM ALREADY ASSOCIATED WITH DOC
			postings.back().getPositions().push_back(pos);
		}
		else { // TERM DOES EXIST BUT DOC DOES NOT CONTAIN TERM
			postings.push_back(DocInfo(docId));

			postings.back().getPositions().push_back(pos);
		}
	}
}

void InvertedIndex::hyphenFix() const
{
	for (auto pair : _mIndex)
		if (pair.first.find('-') != std::string::npos) {
			std::cout << pair.first.substring() << std::endl;
			//addTerm();
		}
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