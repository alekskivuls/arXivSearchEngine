#ifndef INVERTED_INDEX_H
#define INVERTED_INDEX_H

#include <unordered_map>
#include "DocInfo.h"
#include <cstdint>
#include <string>
#include <vector>
#include <list>


/**
 * This data structure represents the inverted index. It incapsulates a set of stemmed 
 * terms and a list of Objects called DocInfos. The list of DocInfos represents an 
 * entire postings list. Each DocInfo within the postings list represents the positional 
 * occurences of the term that occurs for that document.
 */
class InvertedIndex {
	/** FUTURE implementations of the QEngine will include hiding the InvertedIndex from the scope of main.
	 * To understand the purpose of listing QEngine as a friend, please refer to the constructor in QEngine.cpp*/
	friend class QEngine;
	/** The hashmap that represents the entire inverted index.*/
	std::unordered_map<std::string, std::list<DocInfo>> _mIndex;

	std::list<std::string> vocabList;

	void addVocab(const std::string &term);

public:
	InvertedIndex();

	std::list<std::string> getVocabList() const;

	/** Adds an instance of the stemmed term to the document.
	 * A new DocInfo is created if the term is first encountered in that document.*/
	void addTerm(const std::string &term, const uint32_t &docId, const uint32_t &pos); // maybe create a remove term?

	/**This method is used to check if a term exists. Instead of catching an out of
	* bounds error everytime, we can check to avoid the expensive try/ catch overhead.*/
	bool hasPostings(const std::string &term) const;

	/**Returns the postings list for the requested term.*/
	std::list<DocInfo> getPostings(const std::string &term) const;

	void vocab() const;

    std::unordered_map<std::string, std::list<DocInfo>> getIndex() const;

	uint32_t getTermCount() const;
};

#endif
