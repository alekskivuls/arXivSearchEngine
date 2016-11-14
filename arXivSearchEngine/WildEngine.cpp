#include "WildEngine.h"
#include <sstream>
#include <iostream>
#include <algorithm>
#include <boost/regex.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/predicate.hpp>

/**
 * @brief WildEngine::WildEngine : The constructor of the WildEngine; requires reference
 * of 3 different size KgramIndex objects. They are expected to contain the same vocabulary.
 * @param index1 : Passing in the reference of the KgramIndex object of size 1.
 * @param index2 : Passing in the reference of the KgramIndex object of size 2.
 * @param index3 : Passing in the reference of the KgramIndex object of size 3.
 */
WildEngine::WildEngine(KgramIndex &index1,
                       KgramIndex &index2,
                       KgramIndex &index3) : kindex1(index1), kindex2(index2), kindex3(index3) { }

std::list<std::string> WildEngine::vocab(){ //need to use WildEngine:: or wont be declared in scope.
    std::cout << &kindex1 << std::endl;
    return kindex1.getVocab(); //assuming all 3 index have same words stored.
}

/**
 * @brief WildEngine::potentials : Returns all potential terms in the wildcard query, filtered
 * via boost::regex. This is FALSE wildcard, put more (*) for more wildcard characters.
 * @param wildcard : the literal wildcard term. (i.e. "h*llo")potentialterms
 * @return The return value is a list of strings that fit the wildcard.
 */
std::list<std::string> WildEngine::potentials(std::string &wildcard){
    std::list<std::string> refToList = KgramIndex::getGrams(wildcard, 3);
    std::list<std::string> potentialterms, filtered;
    //std::cout << refToList.size() << std::endl;
    //std::cout << refToList.front() << std::endl;
    for(std::string gram : refToList) { //go from grams to terms in those grams.
        //std::cout << gram << std::endl;
        int found = gram.find('*');
        //std::cout << found << std::endl;
        std::list<std::string> reterms;
        if (found == -1) { //no *
            reterms = kindex3.getTerms(gram);
            //std::cout << reterms.size() << std::endl;
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
                    reterms = kindex2.getTerms(gram);
                    //std::cout << gram << std::endl;
                } else {
                    //use 1-gram index
                    reterms = kindex1.getTerms(gram);
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
        reterms.clear();
    }

    //Post-proccessing step
    boost::regex reg{regularex(wildcard)};

    for(auto term : potentialterms) {
          if(boost::regex_search(term, reg)) filtered.push_back(term);
    }
    return filtered;
}

/**
 * @brief WildEngine::regularex : This method takes in a string wildcard and turns the wildcard
 * into a regular expression pattern string.
 *
 * The intention of this method was for it to create a regular expression string to be sent to
 * the caller to be turned into a boost::regex mattern to be matched.
 * @param wildcard : the std::string of the wildcard token. (i.e. "h*llo")
 * @return The return value is a std::string of the regular expression of the inputed std::string.
 * (i.e. "(^h\\wllo$)"; the '\w' is escaped for when it is taken in, otherwise literal regular
 * expression is "(^h\wllo$)".
 */
std::string WildEngine::regularex(std::string wildcard){
    std::string pattern;
    int i;
    pattern += "(^";
    for(i = 0; i < wildcard.size(); ++i) {
        if (wildcard.at(i) == '*') pattern += "\\w";
        else pattern += wildcard.at(i);
    }
    pattern += "$)";
    //std::cout << preparation << std::endl;
    return pattern;
}
