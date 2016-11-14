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

	static int min(int x, int y, int z);


public:
    // In the future we would like to make a list on indicies. void addIndex(int kgramSize);

	KEngine();

	/** This will use the KgramIndex and break the mispelled word into
	 * kgrams.
	 *
	 * The returned string is the word that has the highest JC. */
    static std::list<std::string> correctSpelling(std::string &mispelled, KgramIndex &idx);

    /** This method will run the Jaccard coefficient on the two terms
     * and then return the integer value it gets from the formula.
     * Public for unit testing.
     */
    static float jaccard(std::string &term1, std::string &term2, int kSize);

    static int editDistDP(std::string term1, std::string term2);
};

#endif
