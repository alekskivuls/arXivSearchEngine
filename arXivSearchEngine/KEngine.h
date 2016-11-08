#ifndef KENGINE_H
#define KENGINE_H

#include "KgramIndex.h"
#include <stack>
#include <vector>
#include <unordered_map>
#include <string>
#include <list>

/** KEngine will only take one string as the token */

class KEngine {

	std::unordered_map<std::string, float> jaccarded;

	/** This method will run the Jaccard coefficient on the two terms
	 * and then return the integer value it gets from the formula */
    float Jaccard(std::string &term1, std::string &term2, int kSize);

	int editDistDP(std::string term1, std::string term2);

	int min(int x, int y, int z);



public:
	KEngine();

	/** This will use the KgramIndex and break the mispelled word into
	 * kgrams.
	 *
	 * The returned string is the word that has the highest JC. */
	std::list<std::string> CorrectSpelling(std::string &mispelled, KgramIndex &idx);
};

#endif
