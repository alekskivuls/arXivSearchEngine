#include "InvertedIndex.h"
#include <algorithm>
#include <iterator>
#include <iostream>
InvertedIndex::InvertedIndex() : vocabList(std::list<std::string>()) { }

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
 *
 */
void InvertedIndex::addVocab(const std::string &term) {
	if (vocabList.size() == 0 || vocabList.back() < term) {
		vocabList.push_back(term);
		return;
	}

	if (vocabList.front() > term) {
		vocabList.push_front(term);
		return;
	}

	auto itr = vocabList.begin(), next = itr;
	while (itr != vocabList.end()) {
		next = std::next(itr, 1);

		if (*itr < term && term < *next) {
			vocabList.insert(next, term);
			break;
		}

		++itr; // check next term
	}
}

std::list<std::string> InvertedIndex::getVocabList() const {
	return vocabList;
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
void InvertedIndex::addTerm(const std::string &term, const uint32_t &docId, const uint32_t &pos) {
	if (_mIndex.find(term) == _mIndex.end()) { // TERM DOES NOT EXIST
		addVocab(term);
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

std::unordered_map<std::string, std::list<DocInfo>> InvertedIndex::getIndex() const {
	return _mIndex;
}

/*
 * returns the number of unique terms that exist in the inverted index.
 */
uint32_t InvertedIndex::getTermCount() const {
	return _mIndex.size();
}
