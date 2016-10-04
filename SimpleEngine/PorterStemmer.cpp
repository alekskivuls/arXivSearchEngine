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
		token = token.substr(0, token.length() - 2);
	else if (boost::algorithm::ends_with(token, "ies"))
		token = token.substr(0, token.length() - 2);
	else if (!boost::algorithm::ends_with(token, "ss") && boost::algorithm::ends_with(token, "s"))
		token = token.substr(0, token.length() - 1);

	//Step 1b
	boost::smatch list;
	bool extra1b = false;
	if (boost::algorithm::ends_with(token, "eed")) {
		if (boost::regex_search(token.substr(0, token.length() - 3), list, mGr0)) {
			token = token.substr(0, token.length() - 2);
		}
	}
	else if (boost::algorithm::ends_with(token, "ed")) {
		if (boost::regex_search(token.substr(0, token.length() - 2), list, vowel)) {
			token = token.substr(0, token.length() - 2);
			extra1b = true;
		}
	}
	else if (boost::algorithm::ends_with(token, "ing")) {
		if (boost::regex_search(token.substr(0, token.length() - 3), list, vowel)) {
		token = token.substr(0, token.length() - 3);
		extra1b = true;
		}
	}

	if (extra1b) {
		if (boost::algorithm::ends_with(token, "at") || boost::algorithm::ends_with(token, "bl") || boost::algorithm::ends_with(token, "iz")) {
			token = token + "e";
		}
		else if ((token.at(token.length() - 2)) == (token.at(token.length() - 1)) && (token.at(token.length() - 1) != 'l' && (token.at(token.length() - 1) != 's') && (token.at(token.length() - 1) != 'z'))) {
			token = token.substr(0, token.length() - 1);
		}
		else if (boost::regex_search(token, list, mEq1cvc)) {
			token = token + "e";
		}
	}

	//Step 1c
	if (boost::regex_search(token.substr(0, token.length() - 1), vowel) && (token.at(token.length() - 1) == 'y'))
		token = token.substr(0, token.length() - 1) + 'i';

	//Step 2
	if (boost::algorithm::ends_with(token, "ational")) {
		if (boost::regex_search(token.substr(0, token.length() - 7), mGr0)) {
			token = token.substr(0, token.length() - 7) + "ate";
		}
	} else if (boost::algorithm::ends_with(token, "tional")) {
		if (boost::regex_search(token.substr(0, token.length() - 6), mGr0)) {
			token = token.substr(0, token.length() - 6) + "tion";
		}
	}
	else if(boost::algorithm::ends_with(token, "enci")) {
		if (boost::regex_search(token.substr(0, token.length() - 4), mGr0)) {
			token = token.substr(0, token.length() - 4) + "ence";
		}
	}
	else if (boost::algorithm::ends_with(token, "anci")) {
		if (boost::regex_search(token.substr(0, token.length() - 4), mGr0)) {
			token = token.substr(0, token.length() - 4) + "ance";
		}
	}
	else if (boost::algorithm::ends_with(token, "izer")) {
		if (boost::regex_search(token.substr(0, token.length() - 4), mGr0)) {
			token = token.substr(0, token.length() - 4) + "ize";
		}
	}
	else if (boost::algorithm::ends_with(token, "abli")) {
		if (boost::regex_search(token.substr(0, token.length() - 4), mGr0)) {
			token = token.substr(0, token.length() - 4) + "able";
		}
	}
	else if (boost::algorithm::ends_with(token, "alli")) {
		if (boost::regex_search(token.substr(0, token.length() - 4), mGr0)) {
			token = token.substr(0, token.length() - 4) + "al";
		}
	} 
	else if (boost::algorithm::ends_with(token, "entli")) {
		if (boost::regex_search(token.substr(0, token.length() - 5), mGr0)) {
			token = token.substr(0, token.length() - 5) + "ent";
		}
	}
	else if (boost::algorithm::ends_with(token, "eli")) {
		if (boost::regex_search(token.substr(0, token.length() - 3), mGr0)) {
			token = token.substr(0, token.length() - 3) + "e";
		}
	}
	else if (boost::algorithm::ends_with(token, "ousli")) {
		if (boost::regex_search(token.substr(0, token.length() - 5), mGr0)) {
			token = token.substr(0, token.length() - 5) + "ous";
		}
	}
	else if (boost::algorithm::ends_with(token, "ization")) {
		if (boost::regex_search(token.substr(0, token.length() - 7), mGr0)) {
			token = token.substr(0, token.length() - 7) + "ize";
		}
	}
	else if (boost::algorithm::ends_with(token, "ation")) {
		if (boost::regex_search(token.substr(0, token.length() - 5), mGr0)) {
			token = token.substr(0, token.length() - 5) + "ate";
		}
	}
	else if (boost::algorithm::ends_with(token, "ator")) {
		if (boost::regex_search(token.substr(0, token.length() - 4), mGr0)) {
			token = token.substr(0, token.length() - 4) + "ate";
		}
	}
	else if (boost::algorithm::ends_with(token, "alism")) {
		if (boost::regex_search(token.substr(0, token.length() - 5), mGr0)) {
			token = token.substr(0, token.length() - 5) + "al";
		}
	}
	else if (boost::algorithm::ends_with(token, "iveness")) {
		if (boost::regex_search(token.substr(0, token.length() - 7), mGr0)) {
			token = token.substr(0, token.length() - 7) + "ive";
		}
	}
	else if (boost::algorithm::ends_with(token, "fulness")) {
		if (boost::regex_search(token.substr(0, token.length() - 7), mGr0)) {
			token = token.substr(0, token.length() - 7) + "ful";
		}
	}
	else if (boost::algorithm::ends_with(token, "ousness")) {
		if (boost::regex_search(token.substr(0, token.length() - 7), mGr0)) {
			token = token.substr(0, token.length() - 7) + "ous";
		}
	}
	else if (boost::algorithm::ends_with(token, "aliti")) {
		if (boost::regex_search(token.substr(0, token.length() - 5), mGr0)) {
			token = token.substr(0, token.length() - 5) + "al";
		}
	}
	else if (boost::algorithm::ends_with(token, "iviti")) {
		if (boost::regex_search(token.substr(0, token.length() - 5), mGr0)) {
			token = token.substr(0, token.length() - 5) + "ive";
		}
	}
	else if (boost::algorithm::ends_with(token, "biliti")) {
		if (boost::regex_search(token.substr(0, token.length() - 6), mGr0)) {
			token = token.substr(0, token.length() - 6) + "ble";
		}
	}

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
	else if (trim(token, std::string("encee"), std::string(""), mGr1)) {}
	else if (trim(token, std::string("er"), std::string(""), mGr1)) {}
	else if (trim(token, std::string("ic"), std::string(""), mGr1)) {}
	else if (trim(token, std::string("able"), std::string(""), mGr1)) {}
	else if (trim(token, std::string("ible"), std::string(""), mGr1)) {}
	else if (trim(token, std::string("ant"), std::string(""), mGr1)) {}
	else if (trim(token, std::string("ement"), std::string(""), mGr1)) {}
	else if (trim(token, std::string("ment"), std::string(""), mGr1)) {}
	else if (trim(token, std::string("ent"), std::string(""), mGr1)) {}
	else if (trim(token, std::string("sion"), std::string(""), mGr1)) {}
	else if (trim(token, std::string("tion"), std::string(""), mGr1)) {}
	else if (trim(token, std::string("ou"), std::string(""), mGr1)) {}
	else if (trim(token, std::string("ism"), std::string(""), mGr1)) {}
	else if (trim(token, std::string("ate"), std::string(""), mGr1)) {}
	else if (trim(token, std::string("iti"), std::string(""), mGr1)) {}
	else if (trim(token, std::string("ous"), std::string(""), mGr1)) {}
	else if (trim(token, std::string("ive"), std::string(""), mGr1)) {}
	else if (trim(token, std::string("ize"), std::string(""), mEq1Ncvc)) {}
	
	//Step 5a
	if (trim(token, std::string("e"), std::string(""), mGr1)) {}
	else if (trim(token, std::string("ent"), std::string(""), mEq1)) {}
	
	//Step 5b
	if (trim(token, std::string("ll"), std::string(""), mGr1)) {} 
	else if (trim(token, std::string("dd"), std::string(""), mGr1)) {}

	return token;
}

bool PorterStemmer::trim(std::string &token, std::string &suffix, std::string &replacement, boost::regex &reg) {
	if (boost::algorithm::ends_with(token, suffix)) {
		if (boost::regex_search(token.substr(0, (token.length() - suffix.length())), reg)) {
			token = token.substr(0, token.length() - suffix.length()) + replacement;
			return true;
		}
	}
	return false;
}