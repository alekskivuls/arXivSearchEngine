#include "PorterStemmer.h"
#include <boost/regex.hpp>
#include <boost/algorithm/string/predicate.hpp>
#include <string>
#include <iostream>

std::string PorterStemmer::stem(std::string &token)
{
	//Token must be at least 3 chars
	if (token.length() < 3)
		return token;

	//Step 1a
	if (boost::algorithm::ends_with(token, sses))
		token.erase(token.end() - 2, token.end());
		//token = token.substr(0, token.length() - 2);
	else if (boost::algorithm::ends_with(token, ies))
		token.erase(token.end() - 2, token.end());
	else if (!boost::algorithm::ends_with(token, ss) && boost::algorithm::ends_with(token, s))
		token.erase(token.end() - 1, token.end());

	//Step 1b
	boost::smatch list;
	bool extra1b = false;
	if (trim(token, eed, ee, mGr0)) {}
	else if (trim(token, ed, blank, vowel)) {
		if(!boost::algorithm::ends_with(token, ed))
			extra1b = true;
	}
	else if (trim(token, ing, blank, vowel)) {
		if (!boost::algorithm::ends_with(token, ing))
			extra1b = true;
	}

	if (extra1b) {
		if (boost::algorithm::ends_with(token, at) || boost::algorithm::ends_with(token, bl) || boost::algorithm::ends_with(token, iz)) {
			token.append(e);
		}
		else if (token.length() >= 2 && (token.at(token.length() - 2)) == (token.at(token.length() - 1))) {
			if ((token.at(token.length() - 1) != 'l' && (token.at(token.length() - 1) != 's') && (token.at(token.length() - 1) != 'z'))) {
				token.erase(token.end() - 1, token.end());
			}
		}
		else if (boost::regex_search(token, list, mEq1cvc)) {
			token.append(e);// = token + "e";
		}
	}

	//Step 1c
	if (boost::regex_search(token.substr(0, token.length() - 1), vowel) && (token.at(token.length() - 1) == 'y')) {
		token.erase(token.end() - 1, token.end());
		token.append(i);
	}

	//Step 2
	if (trim(token, ational, ate, mGr0)) {}
	else if(trim(token, tional, tion, mGr0)) {}
	else if (trim(token, enci, ence, mGr0)) {}
	else if (trim(token, anci, ance, mGr0)) {}
	else if (trim(token, izer, ize, mGr0)) {}
	else if (trim(token, abli, al, mGr0)) {}
	else if (trim(token, alli, al, mGr0)) {}
	else if (trim(token, entli, ent, mGr0)) {}
	else if (trim(token, eli, e, mGr0)) {}
	else if (trim(token, ousli, ous, mGr0)) {}
	else if (trim(token, ization, ize, mGr0)) {}
	else if (trim(token, ation, ate, mGr0)) {}
	else if (trim(token, ator, ate, mGr0)) {}
	else if (trim(token, alism, al, mGr0)) {}
	else if (trim(token, iveness, ive, mGr0)) {}
	else if (trim(token, fulness, ful, mGr0)) {}
	else if (trim(token, ousness, ous, mGr0)) {}
	else if (trim(token, aliti, al, mGr0)) {}
	else if (trim(token, iviti, ive, mGr0)) {}
	else if (trim(token, biliti, ble, mGr0)) {}

	//Step 3
	if (trim(token, icate, ic, mGr0)) {}
	else if (trim(token, ative, blank, mGr0)) {}
	else if (trim(token, alize, al, mGr0)) {}
	else if (trim(token, iciti, ic, mGr0)) {}
	else if (trim(token, ical, ic, mGr0)) {}
	else if (trim(token, ful, blank, mGr0)) {}
	else if (trim(token, ness, blank, mGr0)) {}

	//Step 4
	if (trim(token, al, blank, mGr1)) {}
	else if (trim(token, ance, blank, mGr1)) {}
	else if (trim(token, ence, blank, mGr1)) {}
	else if (trim(token, er, blank, mGr1)) {}
	else if (trim(token, ic, blank, mGr1)) {}
	else if (trim(token, able, blank, mGr1)) {}
	else if (trim(token, ible, blank, mGr1)) {}
	else if (trim(token, ant, blank, mGr1)) {}
	else if (trim(token, ement, blank, mGr1)) {}
	else if (trim(token, ment, blank, mGr1)) {}
	else if (trim(token, ent, blank, mGr1)) {}
	else if (boost::algorithm::ends_with(token, sion) || boost::algorithm::ends_with(token, tion)) { 
		trim(token, ion, "", mGr1);
	}
	else if (trim(token, ou, blank, mGr1)) {}
	else if (trim(token, ism, blank, mGr1)) {}
	else if (trim(token, ate, blank, mGr1)) {}
	else if (trim(token, iti, blank, mGr1)) {}
	else if (trim(token, ous, blank, mGr1)) {}
	else if (trim(token, ive, blank, mGr1)) {}
	else if (trim(token, ize, blank, mGr1)) {}
	
	//Step 5a
	if (trim(token, "e", "", mGr1)) {}
	if (trim(token, "e", "e", mEq1)) {
		if (!boost::regex_search(token.substr(0, (token.length() - 1)), mEq1cvc)) {
			trim(token, "e", "", mEq1);
		}
	}
	
	//Step 5b
	if (boost::regex_search(token.substr(0, (token.length() - 1)), mGr1)) {
		if (boost::algorithm::ends_with(token, "ll")) {
			token.erase(token.end() - 1, token.end());
		}
	}
	return token;
}

bool PorterStemmer::trim(std::string &token, const std::string &suffix, const std::string &replacement, const boost::regex &reg) {
	if (boost::algorithm::ends_with(token, suffix)) {
		std::string prefix = token.substr(0, (token.length() - suffix.length()));
		if (boost::regex_search(prefix, reg)) {
			//token = token.substr(0, token.length() - suffix.length()) + replacement;
			token.erase(token.end() - suffix.length(), token.end());
			token.append(replacement);
		}
		return true;
	}
	return false;
}
