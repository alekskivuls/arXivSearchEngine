#ifndef PORTER_STEMMER_H
#define PORTER_STEMMER_H

#include <string>
#include <boost/regex.hpp>

class PorterStemmer {
public:
	std::string stem(std::string&);

private:
	const std::string c = "[^aeiou]";
	const std::string v = "[aeiouy]";
	const std::string C = c + "[^aeiouy]*";
	const std::string V = v + "[aeiou]*";

	const boost::regex mGr0{"^(" + C + ")?" + V + C };
	const boost::regex mEq1{"^(" + C + ")?(" + V + C + "){1}(" + V + ")?$" };
	const boost::regex mGr1{ "^(" + C + ")?(" + V + C + V + C + ")"};///"){2,}(" + V + ")?$" };
	const boost::regex vowel{V};
	const boost::regex mEq1cvc{ "^(" + c + v + ")(?=[^wxy])(" + c + "){1}$" };

	bool trim(std::string&, const std::string&, const std::string&, const boost::regex&);

	//String suffix patterns
	const std::string blank = "";
	const std::string sses = "sses";
	const std::string ies = "ies";
	const std::string ss = "ss";
	const std::string s = "s";
	const std::string eed = "eed";
	const std::string ee = "ee";
	const std::string ed = "ed";
	const std::string ing = "ing";
	const std::string at = "at";
	const std::string bl = "bl";
	const std::string iz = "iz";
	const std::string e = "e";
	const std::string i = "i";

	//Step 2 patterns
	const std::string ational = "ational";
	const std::string ate = "ate";
	const std::string tional = "tional";
	const std::string tion = "tion";
	const std::string enci = "enci";
	const std::string ence = "ence";
	const std::string anci = "anci";
	const std::string ance = "ance";	
	const std::string izer = "izer";
	const std::string ize = "ize";
	const std::string abli = "abli";
	const std::string al = "al";
	const std::string alli = "alli";
	const std::string entli = "entli";
	const std::string ent = "ent";
	const std::string eli = "eli";
	const std::string ousli = "ousli";
	const std::string ous = "ous";
	const std::string ization = "ization";
	const std::string ation = "ation";
	const std::string ator = "ator";
	const std::string alism = "alism";
	const std::string iveness = "iveness";
	const std::string ive = "ive";
	const std::string fulness = "fulness";
	const std::string ful = "ful";
	const std::string ousness = "ousness";
	const std::string aliti = "aliti";
	const std::string iviti = "iviti";
	const std::string biliti = "biliti";
	const std::string ble = "ble";

	//Step 3 patterns
	const std::string icate = "icate";
	const std::string ative = "ative";
	const std::string alize = "alize";
	const std::string iciti = "iciti";
	const std::string ic = "ic";
	const std::string ical = "ical";
	const std::string ness = "ness";
	
	//Step 4 patterns
	const std::string er = "er";
	const std::string able = "able";
	const std::string ible = "ible";
	const std::string ant = "ant";
	const std::string ement = "ement";
	const std::string ment = "ment";
	const std::string sion = "sion";
	const std::string ion = "ion";
	const std::string ou = "ou";
	const std::string ism = "ism";
	const std::string iti = "iti";
};

#endif
