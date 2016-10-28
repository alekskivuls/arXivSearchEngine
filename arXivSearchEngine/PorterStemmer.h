#ifndef PORTER_STEMMER_H
#define PORTER_STEMMER_H

#include <string>
#include <boost/regex.hpp>

class PorterStemmer {
public:
	/**Apply the Porter stemmer to the token and return the result */
	static std::string stem(std::string&);

private:
	//Regex patterns
	static const std::string c;
	static const std::string v;
	static const std::string C;
	static const std::string V;

	static const boost::regex mGr0;
	static const boost::regex mEq1;
	static const boost::regex mGr1;
	static const boost::regex vowel;
	static const boost::regex mEq1cvc;

	//String suffix patterns
	static const std::string blank;
	static const std::string sses;
	static const std::string ies;
	static const std::string ss;
	static const std::string s;
	static const std::string eed;
	static const std::string ee;
	static const std::string ed;
	static const std::string ing;
	static const std::string at;
	static const std::string bl;
	static const std::string iz;
	static const std::string e;
	static const std::string i;

	//Step 2 patterns
	static const std::string ational;
	static const std::string ate;
	static const std::string tional;
	static const std::string tion;
	static const std::string enci;
	static const std::string ence;
	static const std::string anci;
	static const std::string ance;	
	static const std::string izer;
	static const std::string ize;
	static const std::string abli;
	static const std::string al;
	static const std::string alli;
	static const std::string entli;
	static const std::string ent;
	static const std::string eli;
	static const std::string ousli;
	static const std::string ous;
	static const std::string ization;
	static const std::string ation;
	static const std::string ator;
	static const std::string alism;
	static const std::string iveness;
	static const std::string ive;
	static const std::string fulness;
	static const std::string ful;
	static const std::string ousness;
	static const std::string aliti;
	static const std::string iviti;
	static const std::string biliti;
	static const std::string ble;

	//Step 3 patterns
	static const std::string icate;
	static const std::string ative;
	static const std::string alize;
	static const std::string iciti;
	static const std::string ic;
	static const std::string ical;
	static const std::string ness;
	
	//Step 4 patterns
	static const std::string er;
	static const std::string able;
	static const std::string ible;
	static const std::string ant;
	static const std::string ement;
	static const std::string ment;
	static const std::string sion;
	static const std::string ion;
	static const std::string ou;
	static const std::string ism;
	static const std::string iti;

	static bool trim(std::string&, const std::string&, const std::string&, const boost::regex&);
};

#endif
