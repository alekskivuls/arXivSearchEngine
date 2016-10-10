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
	if (boost::algorithm::ends_with(token, "sses"))
		token.erase(token.end() - 2, token.end());
		//token = token.substr(0, token.length() - 2);
	else if (boost::algorithm::ends_with(token, "ies"))
		token.erase(token.end() - 2, token.end());
	else if (!boost::algorithm::ends_with(token, "ss") && boost::algorithm::ends_with(token, "s"))
		token.erase(token.end() - 1, token.end());

	//Step 1b
	boost::smatch list;
	bool extra1b = false;
	if (trim(token, std::string("eed"), std::string("ee"), mGr0)) {}
	else if (trim(token, std::string("ed"), std::string(""), vowel)) {
		if(!boost::algorithm::ends_with(token, std::string("ed")))
			extra1b = true;
	}
	else if (trim(token, std::string("ing"), std::string(""), vowel)) {
		if (!boost::algorithm::ends_with(token, std::string("ing")))
			extra1b = true;
	}

	if (extra1b) {
		if (boost::algorithm::ends_with(token, "at") || boost::algorithm::ends_with(token, "bl") || boost::algorithm::ends_with(token, "iz")) {
			token.append("e");
		}
		else if (token.length() >= 2 && (token.at(token.length() - 2)) == (token.at(token.length() - 1))) {
			if ((token.at(token.length() - 1) != 'l' && (token.at(token.length() - 1) != 's') && (token.at(token.length() - 1) != 'z'))) {
				token.erase(token.end() - 1, token.end());
			}
		}
		else if (boost::regex_search(token, list, mEq1cvc)) {
			token.append("e");// = token + "e";
		}
	}

	//Step 1c
	if (boost::regex_search(token.substr(0, token.length() - 1), vowel) && (token.at(token.length() - 1) == 'y')) {
		token.erase(token.end() - 1, token.end());
		token.append("i");
	}

	//Step 2
	if (trim(token, std::string("ational"), std::string("ate"), mGr0)) {}
	else if(trim(token, std::string("tional"), std::string("tion"), mGr0)) {}
	else if (trim(token, std::string("enci"), std::string("ence"), mGr0)) {}
	else if (trim(token, std::string("anci"), std::string("ance"), mGr0)) {}
	else if (trim(token, std::string("izer"), std::string("ize"), mGr0)) {}
	else if (trim(token, std::string("abli"), std::string("al"), mGr0)) {}
	else if (trim(token, std::string("alli"), std::string("al"), mGr0)) {}
	else if (trim(token, std::string("entli"), std::string("ent"), mGr0)) {}
	else if (trim(token, std::string("eli"), std::string("e"), mGr0)) {}
	else if (trim(token, std::string("ousli"), std::string("ous"), mGr0)) {}
	else if (trim(token, std::string("ization"), std::string("ize"), mGr0)) {}
	else if (trim(token, std::string("ation"), std::string("ate"), mGr0)) {}
	else if (trim(token, std::string("ator"), std::string("ate"), mGr0)) {}
	else if (trim(token, std::string("alism"), std::string("al"), mGr0)) {}
	else if (trim(token, std::string("iveness"), std::string("ive"), mGr0)) {}
	else if (trim(token, std::string("fulness"), std::string("ful"), mGr0)) {}
	else if (trim(token, std::string("ousness"), std::string("ous"), mGr0)) {}
	else if (trim(token, std::string("aliti"), std::string("al"), mGr0)) {}
	else if (trim(token, std::string("iviti"), std::string("ive"), mGr0)) {}
	else if (trim(token, std::string("biliti"), std::string("ble"), mGr0)) {}

	//Step 3
	if (trim(token, std::string("icate"), std::string("ic"), mGr0)) {}
	else if (trim(token, std::string("ative"), std::string(""), mGr0)) {}
	else if (trim(token, std::string("alize"), std::string("al"), mGr0)) {}
	else if (trim(token, std::string("iciti"), std::string("ic"), mGr0)) {}
	else if (trim(token, std::string("ical"), std::string("ic"), mGr0)) {}
	else if (trim(token, std::string("ful"), std::string(""), mGr0)) {}
	else if (trim(token, std::string("ness"), std::string(""), mGr0)) {}

	//Step 4
	if (trim(token, std::string("al"), std::string(""), mGr1)) {}
	else if (trim(token, std::string("ance"), std::string(""), mGr1)) {}
	else if (trim(token, std::string("ence"), std::string(""), mGr1)) {}
	else if (trim(token, std::string("er"), std::string(""), mGr1)) {}
	else if (trim(token, std::string("ic"), std::string(""), mGr1)) {}
	else if (trim(token, std::string("able"), std::string(""), mGr1)) {}
	else if (trim(token, std::string("ible"), std::string(""), mGr1)) {}
	else if (trim(token, std::string("ant"), std::string(""), mGr1)) {}
	else if (trim(token, std::string("ement"), std::string(""), mGr1)) {}
	else if (trim(token, std::string("ment"), std::string(""), mGr1)) {}
	else if (trim(token, std::string("ent"), std::string(""), mGr1)) {}
	else if (boost::algorithm::ends_with(token, "sion") || boost::algorithm::ends_with(token, "tion")) { 
		trim(token, std::string("ion"), std::string(""), mGr1);
	}
	else if (trim(token, std::string("ou"), std::string(""), mGr1)) {}
	else if (trim(token, std::string("ism"), std::string(""), mGr1)) {}
	else if (trim(token, std::string("ate"), std::string(""), mGr1)) {}
	else if (trim(token, std::string("iti"), std::string(""), mGr1)) {}
	else if (trim(token, std::string("ous"), std::string(""), mGr1)) {}
	else if (trim(token, std::string("ive"), std::string(""), mGr1)) {}
	else if (trim(token, std::string("ize"), std::string(""), mGr1)) {}
	
	//Step 5a
	if (trim(token, std::string("e"), std::string(""), mGr1)) {}
	if (trim(token, std::string("e"), std::string("e"), mEq1)) {
		if (!boost::regex_search(token.substr(0, (token.length() - std::string("e").length())), mEq1cvc)) {
			trim(token, std::string("e"), std::string(""), mEq1);
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

bool PorterStemmer::trim(std::string &token, std::string &suffix, std::string &replacement, boost::regex &reg) {
	if (boost::algorithm::ends_with(token, suffix)) {
		if (boost::regex_search(token.substr(0, (token.length() - suffix.length())), reg)) {
			//token = token.substr(0, token.length() - suffix.length()) + replacement;
			token.erase(token.end() - suffix.length(), token.end());
			token.append(replacement);
		}
		return true;
	}
	return false;
}
