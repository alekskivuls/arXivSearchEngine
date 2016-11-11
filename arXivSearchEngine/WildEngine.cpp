#include "WildEngine.h"
#include <sstream>
#include <iostream>
#include <algorithm>
#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/predicate.hpp>

//passing by refence HELP
WildEngine::WildEngine(KgramIndex &index1,
                       KgramIndex &index2,
                       KgramIndex &index3) : kindex1(index1), kindex2(index2), kindex3(index3) { }

std::list<std::string> WildEngine::vocab(){ //need to use WildEngine:: or wont be declared in scope.
    std::cout << &kindex1 << std::endl;
    return kindex1.getVocab(); //assuming all 3 index have same words stored.
}

/*
 * Returns all potential terms in the wildcard query.
 */
std::list<std::string> WildEngine::potentials(std::string &wildcard){
    std::list<std::string> refToList = KgramIndex::getGrams(wildcard, 3); //wont let me save as &reftolist.
    std::list<std::string> potentialterms;
    //now i have starred grams.
    //std::cout << refToList.size() << std::endl;
    //std::cout << refToList.front() << std::endl;
    for(std::string gram : refToList) { //go from grams to terms in those grams.
        std::cout << gram << std::endl;
        int found = gram.find('*');
        std::cout << found << std::endl;
        std::list<std::string> reterms;
        if (found == -1) { //no *
            reterms = kindex3.getTerms(gram); //list of strings
            std::cout << reterms.size() << std::endl;
        } else {
            if (found == 1) {//split in the middle catching logic error case.
                std::string temp1 = std::string(1, gram.at(0));
                std::string temp2 = std::string(1, gram.at(2));
                reterms = kindex1.getTerms(temp1);
                reterms.merge(kindex1.getTerms(temp2));
            } else {
                gram.erase(found, 1);
                found = gram.find("*");
                if(found == -1){
                    //use 2-gram index
                    reterms = kindex2.getTerms(gram); //list of strings
                    std::cout << gram << std::endl;
                } else {
                    //use 1-gram index
                    reterms = kindex1.getTerms(gram); //list of strings
                }
            }
        }
        for(auto check : reterms) {
            //see if it exists in poterms already
            if (potentialterms.size() == 0) potentialterms.push_back(check);
            if (std::find(potentialterms.begin(), potentialterms.end(), check) == potentialterms.end()) {
                //means you couldnt find it.
                potentialterms.push_back(check);
            }
        }
        reterms.clear(); //incase
    }
    return potentialterms;
}
