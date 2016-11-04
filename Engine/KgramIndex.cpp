#include "KgramIndex.h"

//probably right.
KgramIndex::KgramIndex(int kSize) : _kgramSize(kSize) { }
//why is this returning errors?

//variable
std::unordered_map<std::string, std::list<std::string>> _mIndex;

//returns true or false if the kgram is in the index.
//private method.
bool KgramIndex::hasKgram(const std::string &kgram) const { 
	return _mIndex.find(kgram) != _mIndex.end();
}

std::list<std::string> KgramIndex::getTerms(const std::string &kgram) const {
	std::list<std::string*> empty;
	if (hasKgram(kgram))
		return _mIndex.at(kgram);
	else
		return empty;
}

/*
you give it a string term  and kgram and it will break the term into the n-gram and save the kgrams
into the index and the word into each of thse kgram indexes.
//fills the index
*/

// TODO: gunna change to unordered set.
void KgramIndex::addKgram(const std::string &kgram, const std::string &term) {
	if (_mIndex.find(kgram) == _mIndex.end()) { //if kgram is new or not in index already
		std::list<std::string> terms; //list of kgrams
		//map kgram to list of string term
		terms.push_back(term); // kgrams is the array of words put term into array
		//i just want to put the list with just one string into the kgram
		_mIndex[kgram] = terms;
	}
	else { //kgram is already made
		std::list<std::string*> &terms = _mIndex[kgram]; //index[kg] is the array of ptrs to the words that apply

		if (!(terms.back().contains(term))){ //if term already associated with the kgram, dont put again.
			//if the term is not in the kgram key 
			_mIndex[kgram].back().push_back(term);
		}
	}
}

/*
this will use the addKgram method and separate the term into kgrams and use the
addKgram to add that kgram key to term pair. 
*/
void KgramIndex::addTerm(const std::string &term) {
	int i;
	if(_kgramSize != 1) {
		//if you want 2 or 3 kgrams you can add $
		bool first, last;
		std::string gram;
		int end = term.size() + _kgramSize;
		for (i = 0; i < end; ++i) {
			first = i == 0;
			last = i == end;

			if (first) gram += '$';
			int j, whatev;
			// if(first || last) {
			// 	whatev = _kgramSize - 1;
			// }
			// else {
			// 	whatev = _kgramSize;
			// }
			whatev = (first || last) ? _kgramSize - 1 : _kgramSize;
			for(j = 0; j < whatev; ++j) { // append character term[j]
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
			addKgram(term[i], term);
		}
	}
}

/**
 * std::string term = "hello";
 * std::list<std:string> &refToList = getGrams(hello);
 */
std::list<std::string> getGrams(const std::string &term) {
	// inside the function... does blah, returns a list of all kgrams of term
	int i;
	std::string gram;
	for (i = 0; i < term.size(); i++) {
		if(i == 0) gram += '$';

		

		if(i == term.size()-1) gram += '$';

	}
}

/*
 * This method prints all kgrams and terms associated to each kgram to the console.
 */
void KgramIndex::vocab() const {
	for (auto pair : _mIndex) 
		std::cout << pair.first << '\n';
	std::cout << getTermCount() << '\n';
}

/*
 * returns the number of kgrams that exist in the inverted index.
 */
unsigned int getTermCount() const{
	return _mIndex.size();
}


//string builder and regex and big int.