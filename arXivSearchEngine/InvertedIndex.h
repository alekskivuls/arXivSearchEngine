#ifndef INVERTED_INDEX_H
#define INVERTED_INDEX_H

#include <unordered_map>
#include "DocInfo.h"
#include <cstdint>
#include <string>
#include <vector>
#include <list>


/**
 * @brief InvertedIndex This data structure represents the inverted index. It incapsulates a set of stemmed
 * terms and a list of Objects called DocInfos. The list of DocInfos represents an 
 * entire postings list. Each DocInfo within the postings list represents the positional 
 * occurences of the term that occurs for that document.
 */
class InvertedIndex {
    /* FUTURE implementations of the QEngine will include hiding the InvertedIndex from the scope of main.
	 * To understand the purpose of listing QEngine as a friend, please refer to the constructor in QEngine.cpp*/
	friend class QEngine;
    /** @brief _mIndex The hashmap that represents the entire inverted index.*/
    std::unordered_map<std::string, std::list<DocInfo>> _mIndex;

    /** @brief vocabList The list of all terms in the vocabulary*/
	std::list<std::string> vocabList;

    /**
     * @brief addVocab Adds a term to the vocab list
     * @param term The term to add
     */
	void addVocab(const std::string &term);

public:
	InvertedIndex();

    /**
     * @brief getVocabList Gets the vocab list
     * @return The vocab list
     */
	std::list<std::string> getVocabList() const;

    /**
     * @brief addTerm Adds an instance of the stemmed term to the document.
     * A new DocInfo is created if the term is first encountered in that document.
     * @param term The vocab term to add
     * @param docId The document id which the term is associated to
     * @param pos The positional index of the term
     */
    void addTerm(const std::string &term, const uint32_t &docId, const uint32_t &pos);

    /**
     * @brief hasPostings This method is used to check if a term exists. Instead of catching an out of
     * bounds error everytime, we can check to avoid the expensive try/ catch overhead.
     * @param term The term to check
     * @return True if the term has a postings list
     */
	bool hasPostings(const std::string &term) const;

    /**
     * @brief getPostings Gets the postings list of a term
     * @param term The term to get the postings list of
     * @return The postings list
     */
	std::list<DocInfo> getPostings(const std::string &term) const;

    /**
     * @brief printVocab This method prints all terms and DocInfos associated to each term to the console.
     */
    void printVocab() const;

    /**
     * @brief getIndex Gets the mapping of terms to postings lists
     * @return The map of terms to postings lists
     */
    std::unordered_map<std::string, std::list<DocInfo>> getIndex() const;

    /**
     * @brief getTermCount Gets the number of unique terms that exist in the inverted index.
     * @return The number of unique terms
     */
	uint32_t getTermCount() const;
};

#endif
