#ifndef WILDENGINE_H
#define WILDENGINE_H

#include "KgramIndex.h"
#include <stack>
#include <vector>
#include <unordered_map>
#include <string>
#include <list>

class WildEngine {


    static std::string regularex(std::string wildcard);

public:

    WildEngine();

    static std::list<std::string> potentials(std::string &wildcard, KgramIndex &kindex1,
                                              KgramIndex &kindex2, KgramIndex &kindex3);

    // std::list<std::string> recommendations(std::string &wildcard);
    
};

#endif
