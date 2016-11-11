#ifndef WILDENGINE_H
#define WILDENGINE_H

#include "KgramIndex.h"
#include <stack>
#include <vector>
#include <unordered_map>
#include <string>
#include <list>

class WildEngine {

    /**
     * @brief kindex1 : User can load in their indexes.
     */
    KgramIndex& kindex1; //need the & after to do by reference
    KgramIndex& kindex2;
    KgramIndex& kindex3;

public:

    /**
     * @brief WildEngine : Constructor. Made to load in 1-gram, 2-gram, and 3-gram.
     * @param index1 : pass your reference and these will point to it.
     * @param index2 : pass your reference and these will point to it.
     * @param index3 : pass your reference and these will point to it.
     * The order must specifically be index1, 2, and 3 to function properly.
     */
    WildEngine(KgramIndex &index1, KgramIndex &index2, KgramIndex &index3);

    std::list<std::string> potentials(std::string &wildcard);

    // std::list<std::string> recommendations(std::string &wildcard);

    std::list<std::string> vocab();
};

#endif
