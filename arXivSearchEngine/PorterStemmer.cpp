#include "PorterStemmer.h"
#include <boost/regex.hpp>
#include <string>

	//Regex patterns
	const std::string PorterStemmer::c = "[^aeiou]";
	const std::string PorterStemmer::v = "[aeiouy]";
	const std::string PorterStemmer::C = PorterStemmer::c + "[^aeiouy]*";
	const std::string PorterStemmer::V = PorterStemmer::v + "[aeiou]*";

	const boost::regex PorterStemmer::mGr0{"^(" + C + ")?" + V + C };
	const boost::regex PorterStemmer::mEq1{"^(" + C + ")?(" + V + C + "){1}(" + V + ")?$" };
	const boost::regex PorterStemmer::mGr1{ "^(" + C + ")?(" + V + C + V + C + ")"};///"){2,}(" + V + ")?$" };
	const boost::regex PorterStemmer::vowel{V};
	const boost::regex PorterStemmer::mEq1cvc{ "^(" + c + v + ")(?=[^wxy])(" + c + "){1}$" };

	//Suffix patterns
	//Step 1 patters
	const std::string PorterStemmer::blank = "";
	const std::string PorterStemmer::sses = "sses";
	const std::string PorterStemmer::ies = "ies";
	const std::string PorterStemmer::ss = "ss";
	const std::string PorterStemmer::s = "s";
	const std::string PorterStemmer::eed = "eed";
	const std::string PorterStemmer::ee = "ee";
	const std::string PorterStemmer::ed = "ed";
	const std::string PorterStemmer::ing = "ing";
	const std::string PorterStemmer::at = "at";
	const std::string PorterStemmer::bl = "bl";
	const std::string PorterStemmer::iz = "iz";
	const std::string PorterStemmer::e = "e";
	const std::string PorterStemmer::i = "i";

	//Step 2 patterns
	const std::string PorterStemmer::ational = "ational";
	const std::string PorterStemmer::ate = "ate";
	const std::string PorterStemmer::tional = "tional";
	const std::string PorterStemmer::tion = "tion";
	const std::string PorterStemmer::enci = "enci";
	const std::string PorterStemmer::ence = "ence";
	const std::string PorterStemmer::anci = "anci";
	const std::string PorterStemmer::ance = "ance";	
	const std::string PorterStemmer::izer = "izer";
	const std::string PorterStemmer::ize = "ize";
	const std::string PorterStemmer::abli = "abli";
	const std::string PorterStemmer::al = "al";
	const std::string PorterStemmer::alli = "alli";
	const std::string PorterStemmer::entli = "entli";
	const std::string PorterStemmer::ent = "ent";
	const std::string PorterStemmer::eli = "eli";
	const std::string PorterStemmer::ousli = "ousli";
	const std::string PorterStemmer::ous = "ous";
	const std::string PorterStemmer::ization = "ization";
	const std::string PorterStemmer::ation = "ation";
	const std::string PorterStemmer::ator = "ator";
	const std::string PorterStemmer::alism = "alism";
	const std::string PorterStemmer::iveness = "iveness";
	const std::string PorterStemmer::ive = "ive";
	const std::string PorterStemmer::fulness = "fulness";
	const std::string PorterStemmer::ful = "ful";
	const std::string PorterStemmer::ousness = "ousness";
	const std::string PorterStemmer::aliti = "aliti";
	const std::string PorterStemmer::iviti = "iviti";
	const std::string PorterStemmer::biliti = "biliti";
	const std::string PorterStemmer::ble = "ble";

	//Step 3 patterns
	const std::string PorterStemmer::icate = "icate";
	const std::string PorterStemmer::ative = "ative";
	const std::string PorterStemmer::alize = "alize";
	const std::string PorterStemmer::iciti = "iciti";
	const std::string PorterStemmer::ic = "ic";
	const std::string PorterStemmer::ical = "ical";
	const std::string PorterStemmer::ness = "ness";
	
	//Step 4 patterns
	const std::string PorterStemmer::er = "er";
	const std::string PorterStemmer::able = "able";
	const std::string PorterStemmer::ible = "ible";
	const std::string PorterStemmer::ant = "ant";
	const std::string PorterStemmer::ement = "ement";
	const std::string PorterStemmer::ment = "ment";
	const std::string PorterStemmer::sion = "sion";
	const std::string PorterStemmer::ion = "ion";
	const std::string PorterStemmer::ou = "ou";
	const std::string PorterStemmer::ism = "ism";
	const std::string PorterStemmer::iti = "iti";

std::string PorterStemmer::stem(std::string &token)
{
	//Token must be at least 3 chars
	if (token.length() < 3)
		return token;

	//Step 1a
	if (ends_with(token, sses))
		token.erase(token.end() - 2, token.end());
		//token = token.substr(0, token.length() - 2);
	else if (ends_with(token, ies))
		token.erase(token.end() - 2, token.end());
	else if (!ends_with(token, ss) && ends_with(token, s))
		token.erase(token.end() - 1, token.end());

	//Step 1b
	bool extra1b = false;
	if (trim(token, eed, ee, mGr0)) {}
	else if (trim(token, ed, blank, vowel)) {
		if(!ends_with(token, ed))
			extra1b = true;
	}
	else if (trim(token, ing, blank, vowel)) {
		if (!ends_with(token, ing))
			extra1b = true;
	}

	if (extra1b) {
		if (ends_with(token, at) || ends_with(token, bl) || ends_with(token, iz)) {
			token.append(e);
		}
		else if (token.length() >= 2 && (token.at(token.length() - 2)) == (token.at(token.length() - 1))) {
			if ((token.at(token.length() - 1) != 'l' && (token.at(token.length() - 1) != 's') && (token.at(token.length() - 1) != 'z'))) {
				token.erase(token.end() - 1, token.end());
			}
		}
		else {
        	boost::smatch list;
            if (boost::regex_search(token, list, mEq1cvc)) {
    			token.append(e);// = token + "e";
    		}
        }
	}

	//Step 1c
	if (boost::regex_search(token.substr(0, token.length() - 1), vowel) && (token.at(token.length() - 1) == 'y')) {
		token.erase(token.end() - 1, token.end());
		token.append(i);
	}

	//Step 2
    if(token.length() > 5) {
        switch(token.at(token.length()-2)) {
            case 'a':
                if (trim(token, ational, ate, mGr0)) {}
	            else if(trim(token, tional, tion, mGr0)) {}
                break;
            case 'c':
                if (trim(token, enci, ence, mGr0)) {}
	            else if (trim(token, anci, ance, mGr0)) {}
                break;
            case 'e':
                if (trim(token, izer, ize, mGr0)) {}
                break;
            case 'l':
                if (trim(token, abli, al, mGr0)) {}
	            else if (trim(token, alli, al, mGr0)) {}
	            else if (trim(token, entli, ent, mGr0)) {}
	            else if (trim(token, eli, e, mGr0)) {}
	            else if (trim(token, ousli, ous, mGr0)) {}
                break;
            case 'o':
                if (trim(token, ization, ize, mGr0)) {}
	            else if (trim(token, ation, ate, mGr0)) {}
	            else if (trim(token, ator, ate, mGr0)) {}
                break;
            case 's':
                if (trim(token, alism, al, mGr0)) {}
	            else if (trim(token, iveness, ive, mGr0)) {}
	            else if (trim(token, fulness, ful, mGr0)) {}
	            else if (trim(token, ousness, ous, mGr0)) {}
                break;
            case 't':
                if (trim(token, aliti, al, mGr0)) {}
	            else if (trim(token, iviti, ive, mGr0)) {}
	            else if (trim(token, biliti, ble, mGr0)) {}
                break;
        }
    }

	//Step 3
    if(token.length() > 5) {
        switch(token.at(token.length()-1)) {
	        case 'e':
                if (trim(token, icate, ic, mGr0)) {}
	            else if (trim(token, ative, blank, mGr0)) {}
	            else if (trim(token, alize, al, mGr0)) {}
                break;
            case 'i':
                if (trim(token, iciti, ic, mGr0)) {}
                break;
            case 'l':
                if (trim(token, ical, ic, mGr0)) {}
	            else if (trim(token, ful, blank, mGr0)) {}
                break;
            case 's':
                 if (trim(token, ness, blank, mGr0)) {}
                 break;
        }
    }

	//Step 4
    if(token.length() > 6) {
        switch(token.at(token.length()-2)) {
            case 'a':
                if (trim(token, al, blank, mGr1)) {}
                break;
            case 'c':
                if (trim(token, ance, blank, mGr1)) {}
	            else if (trim(token, ence, blank, mGr1)) {}
                break;
            case 'r':
                if (trim(token, er, blank, mGr1)) {}
                break;
            case 'i':
                if (trim(token, ic, blank, mGr1)) {}
                break;
            case 'l':
                if (trim(token, able, blank, mGr1)) {}
	            else if (trim(token, ible, blank, mGr1)) {}
                break;
            case 'n':
                if (trim(token, ant, blank, mGr1)) {}
                else if (trim(token, ement, blank, mGr1)) {}
	            else if (trim(token, ment, blank, mGr1)) {}
	            else if (trim(token, ent, blank, mGr1)) {}
                break;
            case 'o':
                if (ends_with(token, sion) || ends_with(token, tion)) { 
		            trim(token, ion, "", mGr1);
	            }
	            else if (trim(token, ou, blank, mGr1)) {}
                break;
            case 's':
                if (trim(token, ism, blank, mGr1)) {}
                break;
            case 't':
                if (trim(token, ate, blank, mGr1)) {}
	            else if (trim(token, iti, blank, mGr1)) {}
                break;
            case 'u':
                if (trim(token, ous, blank, mGr1)) {}
                break;
            case 'v':
                if (trim(token, ive, blank, mGr1)) {}
                break;
            case 'z':
                if (trim(token, ize, blank, mGr1)) {}
                break;
        }	 
    }	

	//Step 5a
	if (trim(token, "e", "", mGr1)) {}
	if (trim(token, "e", "e", mEq1)) {
		if (!boost::regex_search(token.substr(0, (token.length() - 1)), mEq1cvc)) {
			trim(token, "e", "", mEq1);
		}
	}
	
	//Step 5b
	if (boost::regex_search(token.substr(0, (token.length() - 1)), mGr1)) {
		if (ends_with(token, "ll")) {
			token.erase(token.end() - 1, token.end());
		}
	}
	return token;
}

/*
 * This method will trim the token.
 */
bool PorterStemmer::trim(std::string &token, const std::string &suffix, const std::string &replacement, const boost::regex &reg) {
	if (ends_with(token, suffix)) {
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

/*
 * This method will check if the string ends with the given ending.
 */
inline bool PorterStemmer::ends_with(std::string const & value, std::string const & ending)
{
    if (ending.size() > value.size()) return false;
    return std::equal(ending.rbegin(), ending.rend(), value.rbegin());
}
