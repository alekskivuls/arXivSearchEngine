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

std::list<std::string> KgramIndex::getTerms(std::string &kgram) const {
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
		std::list<std::string*> terms; //list of kgrams
		//map kgram to list of string term
		std::string* ptr_term = &term;
		terms.push_back(ptr_term); // kgrams is the array of words put term into array
		//i just want to put the list with just one string into the kgram
		_mIndex[kgram] = terms;
	}
	else { //kgram is already made
		std::list<std::string*> &terms = _mIndex[kgram]; //index[kg] is the array of ptrs to the words that apply

		if (!(terms.contains(term))){ //if term already associated with the kgram, dont put again.
			//if the term is not in the kgram key 
			_mIndex[kgram].back().push_back(term);
		}
	}
}

/** This (public) method will separate the term into kgrams (of the size passed in constructor)
 * and use the addKgram (private) to add that kgram key to term pair into the _mIndex.
*/
void KgramIndex::addTerm(std::string &term) {
	int i;
	if(_kgramSize != 1) { //if you want 2 or 3 kgrams you can add $
		bool first, last;
		int end = term.size() + _kgramSize; //why?
		for (i = 0; i < end; ++i) { //do you not get the numer of kgram for letter
			std::string gram;
			first = i == 0;
			last = i == end;

			if (first) gram += '$';
			int j, remainingK;
			remainingK = (first || last) ? _kgramSize - 1 : _kgramSize;
			for(j = 0; j < remainingK; ++j) { // append character term[j]
				gram += term[i + j]; //gram is your gram.
				//in the last and first case this will only cut 2.
				//read into gram so 
			}
			if(last) gram += '$'; 
			//gram is n letters here. 
			addKgram(gram, term); //add the gram
		}
	} else { //1-gram would be stupid...
		for (i = 0; i < term.size(); i++) {
			std::string gram = (std::string)term[i]; //there is likely a better way to do this.
			addKgram(gram, term);
		}
	}
}

/**
 * How to use this (public) method: 
 * std::string term = "hello";
 * std::list<std:string> &refToList = getGrams(hello);
 */
std::list<std::string> KgramIndex::getGrams(std::string &term) {
	// inside the function... does blah, returns a list of all kgrams of term
	int i;
	std::list<std::string> grams;
	if(_kgramSize != 1) {
		bool first, last;
		int end = term.size();
		for (i = 0; i < end; i++) {
			first = i == 0;
			last = i == end;
			std::string gram;
			
			if(i == 0) gram += '$';
			int j, remainingK; //if it was first or last you will have kSize - 1 remaining letters
			remainingK = (first || last) ? _kgramSize - 1 : _kgramSize;
			
			for(j = 0; j < remainingK; ++j) gram += term[i + j];
	
			if(last) gram += '$';
			grams.push_back(gram); //adding our gram onto grams list
		}
	} else { //1-gram
		for (i = 0; i < term.size(); i++) {
			std::string gram = term[i];
			grams.push_back(gram);
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