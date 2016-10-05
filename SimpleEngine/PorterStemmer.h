#ifndef PORTER_STEMMER_H
#define PORTER_STEMMER_H

#include <string>
#include <boost\regex.hpp>

class PorterStemmer {
public:
	std::string stem(std::string&);

private:
	std::string c = "[^aeiou]";
	std::string v = "[aeiouy]";
	std::string C = c + "[^aeiouy]*";
	std::string V = v + "[aeiou]*";

	boost::regex mGr0{"^(" + C + ")?" + V + C };
	boost::regex mEq1{"^(" + C + ")?(" + V + C + "){1}(" + V + ")?$" };
	boost::regex mGr1{"^(" + C + ")?(" + V + C + "){2,}(" + V + ")?$" };
	boost::regex vowel{".*" + V + ".*"};
	boost::regex mEq1cvc{ "^(" + C + V + ")(?=[^wxy])(" + C + "){1}$" };

	bool trim(std::string&, std::string&, std::string&, boost::regex&);
};

#endif