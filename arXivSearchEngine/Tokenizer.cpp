#include "Tokenizer.h"
#include "ctype.h"

Tokenizer::Tokenizer(const boost::filesystem::path &p) : isFileTokenizer(true) {
	_file.open(p.c_str(), std::fstream::in | std::fstream::out | std::fstream::app);
}

Tokenizer::Tokenizer(const std::string &s) 
	: isFileTokenizer(false), _buffer(s), pos(0) { }

Tokenizer::~Tokenizer() {
	_file.close();
}

////Method needs to be refactored and bug fixed
// should pass in an empty string to populate the tokenizer variable 
bool Tokenizer::nextToken(std::string &token, bool &hyphen) {
	bool start = false, tempHyphen = false, alphaNum;
	std::string tempBuf = "";
	hyphen = false;
	token = "";
	char c;
	while (pos < _buffer.size()) {
		// get next char in sequence
		c = _buffer[pos++];
		if (c != '\'') {
			alphaNum = (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9');

			if (alphaNum) {
				if (start) {
					if (tempHyphen) hyphen = true;
					token += tempBuf;
				}
				else start = true;

				if (tempBuf.size() != 0)
					tempBuf = "";

				token += c;
			}
			else {
				if (c > 0 && isspace(c)) 
					break; // DONE
				else {
					if (c == '-') tempHyphen = true;
					tempBuf += c; // truly some dank, random-ass char
				}
			}
		}
	}

    //return token.size() != 0 || pos < _buffer.size();
    return pos < _buffer.size() || token.size() != 0; // no more tokens to process from
}

// ignore the following block of comment. I may end up creating a new library for k-grams, 
// compressing and uncompressing.

/* ... I cannot complete this part until we compress the json files
// logic for file tokenizer
if (_file.is_open()) {
char c;
while (true) {
if (_file.eof()) // return false if no token left (end of file)
return false;
c = (char)_file.get();
std::cout << c << '\n';
// valid
if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z')
|| (c >= '0' && c <= '9') || c == '-')
token += c;
else {
// if... token is NOT empty AND we've come across a non alpha-numeric || non-hyphen char,
// we know that we've reached the end of a valid token

// I need to eventually break to denote the end of a valid token.
if (token != "")
return true;
}
}
}
else {

}
*/

// FUTURE IMPLEMENTATION: Cannot complete this method until compression is complete.
std::string Tokenizer::nextToken(std::string token, const int &gram) {
	return "";
}
