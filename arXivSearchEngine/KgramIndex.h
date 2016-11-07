#ifndef KGRAM_INDEX_H
#define KGRAM_INDEX_H

#include <unordered_map>
#include <string>
#include <list>

/*
 * Takes in an integer n, and that is the number kgram's number of char grams.
 * Main needs to send in an int n and that is what the kgram separates to.
 *
 * Storage for Kgrams.
 *
 */
class KgramIndex {

	int _kgramSize;
	/** The hashmap that represents the entire inverted index.*/
	std::unordered_map<std::string, std::list<std::string*>> _mIndex;
	// Where do I store this.

	/**This method is used to check if a kgram exists. Instead of catching an out of 
	 * bounds error everytime, we can check to avoid the expensive try/ catch overhead.*/
	bool hasKgram(const std::string &kgram) const;

	/**Returns the postings list for the requested kgram.
	 * i.e. getPostings('$st')
	 * will return the list of all the string terms that apply that Kgram
	 */
	void addKgram(const std::string &kgram, const std::string &term);

public:
	//Constructor
	KgramIndex(int kSize); //1, 2, 3.

	void addTerm(const std::string &kgram, const std::string &term); //add kgram to string into index

	void vocab() const;

	std::list<std::string> getGrams(const std::string &term)

	unsigned int getTermCount() const;

};

#endif
