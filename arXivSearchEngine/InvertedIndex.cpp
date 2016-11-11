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
 *
 * _mIndex.find may be redundant....replace later with _mIndex[term]
 */
void InvertedIndex::addTerm(const std::string &term, const unsigned int &docId, const unsigned int &pos) {
	if (_mIndex.find(term) == _mIndex.end()) { // TERM DOES NOT EXIST
		_mIndex.insert(std::pair<std::string, std::list<DocInfo>>(term, std::list<DocInfo>())); // create postings list
		std::list<DocInfo> &postings = _mIndex.at(term);
		//_mIndex[term] = postings; // populate
		postings.push_back(DocInfo(docId));


		DocInfo &doc = postings.back();
		doc.addPosition(pos);
	}
	else { // TERM DOES EXIST
		std::list<DocInfo> &postings = _mIndex.at(term); // get postings of existing term

		// TERM ALREADY ASSOCIATED WITH DOC, DO NOTHING
		if (postings.back().getDocId() != docId) // TERM DOES EXIST BUT DOC DOES NOT CONTAIN TERM
			postings.push_back(DocInfo(docId));

		DocInfo &doc = postings.back();
		doc.addPosition(pos);
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
 * This method returns the strings of all terms and DocInfos associated to each
 * term to the console.
 */
std::vector<std::string> InvertedIndex::getVocab() const {
    std::vector<std::string> keys;
    keys.reserve(_mIndex.size());
    for (auto kv : _mIndex)
        keys.push_back(kv.first);
    return keys;
}


std::unordered_map<std::string, std::list<DocInfo>> InvertedIndex::getIndex() const {
	return _mIndex;
}

/*
 * returns the number of unique terms that exist in the inverted index.
 */
unsigned int InvertedIndex::getTermCount() const {
	return _mIndex.size();
}
