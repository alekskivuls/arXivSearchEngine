#ifndef WILDENGINE_H
#define WILDENGINE_H

#include "KgramIndex.h"
#include <stack>
#include <vector>
#include <unordered_map>
#include <string>
#include <list>

/**
* @brief WildEngine : Constructor. Made to load in 1-gram, 2-gram, and 3-gram.
* @param index1 : pass your reference and these will point to it.
* @param index2 : pass your reference and these will point to it.
* @param index3 : pass your reference and these will point to it.
* The order must specifically be index1, 2, and 3 to function properly.
*/
class WildEngine {

    /**
     * @brief kindex1 : User can load in their indexes.
     */
    // KgramIndex& kindex1; //need the & after to do by reference
    // KgramIndex& kindex2;
    // KgramIndex& kindex3;

    static std::string regularex(std::string wildcard);

public:

    static std::list<std::string> potentials(std::string &wildcard, KgramIndex &kindex1,
                                              KgramIndex &kindex2, KgramIndex &kindex3);

    // std::list<std::string> recommendations(std::string &wildcard);

    
};

#endif
