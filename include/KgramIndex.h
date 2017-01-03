#ifndef KGRAM_INDEX_H
#define KGRAM_INDEX_H

#include <unordered_map>
#include <unordered_set>
#include <string>
#include <list>

/**
 * @brief The KgramIndex class takes in an integer n, and that is the number kgram's number of char grams.
 * Main needs to send in an int n and that is what the kgram separates to.
 *
 * Index for Kgrams to map from kgram to list of terms; has add and get (public)
 * functionality for the index; essentially Index object, and a few independent methods
 * that allow you to use kgram logic.
 */
class KgramIndex {

	int _kgramSize;
	std::unordered_set<std::string> mCache;
	
	/** The hashmap that represents the entire inverted index.
	 * Possible future optimization is to do <std::string*> */
    std::unordered_map<std::string, std::unordered_set<std::string>> _mIndex;

    /**
     * @brief hasKgram This method is used to check if a kgram exists. Instead of catching an out of
     * bounds error everytime, we can check to avoid the expensive try/ catch overhead.
     * @param kgram in std::string that you wish to check if exists in this index.
     * @return bool value if the kgram is in the index.
     */
    bool hasKgram(std::string &kgram) const;

    /**
     * @brief addKgram method used to add Kgram to the KgramIndex.
     * @param kgram is the std::string value of the kgram you want to add to the index.
     * @param term is the std::string value of the term that you want to add to the index.
     */
    void addKgram(std::string &kgram, std::string &term);

public:

    int getKSize();

    /**
     * @brief KgramIndex constructor where the kSize is the k in kgram. Designed to support k = 1 - 3; however may support more than 3.
     * @param kSize is the integer value of the size of your desired kgram.
     */
    KgramIndex(int kSize);

    /**
     * @brief getTerms returns the term list for the requested kgram. i.e. getTerms('$st')
     * @param kgram is the kgram std::string value you want the std::unordered_set<std::string> of terms for.
     * @return std::unordered_set<std::string> of terms that are associated with given kgram.
     */
	std::unordered_set<std::string> getTerms(std::string &kgram);

    /**
     * @brief addTerm adds the term to the KgramIndex. Breaks the term into kgrams of ksize and
     * adds into the index.
     * @param term std::string of term that you wish to add to the index.
     */
    void addTerm(std::string &term);

    /**
     * @brief vocab prints the list of terms in this KgramIndex.
     */
	void vocab() const;

    /**
     * @brief getKgramList returns the list of kgrams in this KgramIndex.
     * @return std::list<std::string> of kgrams in KgramIndex.
     */
    std::list<std::string> getKgramList();


    /** This (public) method is independent of the Index.
     * It is intended to be used for query terms, so the user can get the kgrams of the term.*/
    static std::list<std::string> getGrams(std::string &term, int kSize); //should this all be <std::string*> or no *

    unsigned int getGramCount();

};

#endif
