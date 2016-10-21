#ifndef TOKEN_STREAM_H
#define TOKEN_STREAM_H

#include <string>

/*
 * The interface
 */
class TokenStream {
public:
	/** Returns false if the end of the -string OR file- has been reached 
	 * and true otherwise. This method IS subject to change according to 
	 * our group requirements and definitions of a tokenized word. This 
	 * method will also change when we implement a tokenizer for file reading.*/
	virtual bool nextToken(std::string &token, bool &hyphen) = 0;



	/** Returns false if the end of the -string OR file- has been reached 
	 * and true otherwise. This method is responsible for tokenizing words 
	 * into k grams.
	 * 
	 * It MAY be better to just create a static class solely for compressing, 
	 * uncompressing, stemming and processing to k-grams. It doesn't make sense 
	 * to have a tokenizer for k-gramming.*/
	virtual std::string nextToken(std::string token, const int &gram) = 0;
};

#endif
