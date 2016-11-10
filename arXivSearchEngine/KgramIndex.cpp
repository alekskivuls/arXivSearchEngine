#include "KgramIndex.h"
#include <algorithm>
#include <iostream>

//probably right.
KgramIndex::KgramIndex(int kSize) : _kgramSize(kSize) { }
//why is this returning errors?

//returns true or false if the kgram is in the index.
//private method; used in getTerms (dependency).
//
bool KgramIndex::hasKgram(std::string &kgram) const { 
	return _mIndex.find(kgram) != _mIndex.end();
}

/** This (public) method, getTerms will, given a string kgram (key), return the list<string> (value) of
 * terms associates with that kgram from the _mIndex
 */

std::list<std::string> KgramIndex::getTerms(std::string &kgram) {
	std::list<std::string> empty;
	if (hasKgram(kgram))
		return _mIndex.at(kgram); //at returns pointer to where that thing is at.
	else
		return empty;
}

/**
 * This (private) method, addKgram will, given a string kgram and a string term,
 * push_back that string term into the list (value) and map it to the provided 
 * kgram (key), and save that into the _mIndex.
 * 
 * This method is utilized by (dependency) addTerm.
 */

// TODO: Potential optimation if we change to unordered set type.
void KgramIndex::addKgram(std::string &kgram, std::string &term) {
	if (_mIndex.find(kgram) == _mIndex.end()) { //if kgram is new or not in index already
        std::list<std::string> terms; //list of kgrams
		//map kgram to list of string term
        //std::string ptr_term = &term;
        terms.push_back(term); // kgrams is the array of words put term into array
		//i just want to put the list with just one string into the kgram
		_mIndex[kgram] = terms;
	}
	else { //kgram is already made
        std::list<std::string> &terms = _mIndex[kgram]; //index[kg] is the array of ptrs to the words that apply

        if (!((std::find(terms.begin(), terms.end(), term)) != terms.end())){
        //if (!(terms.contains(term))){ //if term already associated with the kgram, dont put again.
			//if the term is not in the kgram key 
            _mIndex[kgram].push_back(term);
		}
	}
}

int KgramIndex::getKSize(){
    return _kgramSize;
}

/** This (public) method will separate the term into kgrams (of the size passed in constructor)
 * and use the addKgram (private) to add that kgram key to term pair into the _mIndex.
*/
void KgramIndex::addTerm(std::string &term) { //TODO: fix addTerm logic
	int i;
    if(_kgramSize > 1) { //add $ for more than 1
        std::string kterm;
        kterm += '$';
        kterm += term;
        kterm += '$';
        for (i = 0; i < term.size(); ++i) { //do you not get the numer of kgram for letter
            std::string gram;
            int j;
            for(j = 0; j < _kgramSize; ++j) { // append character term[j]
                gram += kterm[i + j];
            }
            //gram is n letters here.
            addKgram(gram, term); //add the gram
        }
    } else {
        for (char letter : term) {
            std::string strl = std::string(1,letter);
            addKgram(strl, term);
		}
	}
}

/**
 * How to use this (public) method: 
 * std::string term = "hello";
 * std::list<std:string> &refToList = getGrams(hello);
 */
std::list<std::string> KgramIndex::getGrams(std::string &term, int kSize) {
	// inside the function... does blah, returns a list of all kgrams of term
	int i;
	std::list<std::string> grams;
    if(kSize > 1) {
        std::string kterm;
        kterm += '$';
        kterm += term;
        kterm += '$';
        for (i = 0; i < term.size(); i++) { //the term.size is the same as # of kgrams
            std::string gram;
            int j;
            for(j = 0; j < kSize; ++j) {
                gram += kterm[i + j];
                //std::cout << i+j << term.at(i + j) << std::endl;
            }
            grams.push_back(gram); //adding our gram onto grams list
            //std::cout << gram << std::endl;
        }
    } else { //1-gram
        for (char letter : term) {
            std::string strl = std::string(1,letter);
            grams.push_back(strl);
		}
	}
	return grams;
}

/*
 * This method prints all kgrams and terms associated to each kgram to the console.
 */

void KgramIndex::vocab() const {
	for (auto pair : _mIndex) std::cout << pair.first << '\n';
	std::cout << getGramCount() << '\n';
}

/*
 * returns the number of kgrams that exist in the inverted index.
 */
unsigned int KgramIndex::getGramCount() const{
	return _mIndex.size();
}


//string builder and regex and big int.
