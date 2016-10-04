#include <boost\algorithm\string\predicate.hpp>
#include <boost\lambda\lambda.hpp>
#include <boost\filesystem.hpp>
#include "InvertedIndex.h"
#include <unordered_map>
#include <unordered_set>
#include "Tokenizer.h"
#include "DocInfo.h"
#include <algorithm>
#include <iostream>
#include <stdio.h>
#include <fstream>
#include <vector>
#include <list>

/*
 * This method has a fully functional and tested inverted index. We only need to change the 
 * tokenizer methods for suitable parsing (single terms, k-grams and splitting hyphenated 
 * words into 3 terms). The following main has sample uses of the functions.
 * 
 * At the end of the day, the terms must be properly processed (stemmed) BEFORE they are 
 * inserted into the inverted index.
 *
 * IMPORTANT NOTE: the following functions need to be removed AND the getPostings method 
 * must also be changed from public to private. This is to ensure data integrity. There is 
 * absolutely no need for the main to be public. There should be higher level funciton calls 
 * that call getPostings.
 */
int main() {
	// opens sample test.txt file
	std::fstream file;
	file.open("test.txt", std::fstream::in | std::fstream::out | std::fstream::app);

	// tokenizes sample test.txt file
	std::string input;
	while (std::getline(file, input)) {
		Tokenizer tkzr(input);

		std::string token;
		while (tkzr.nextToken(token)) // while not end of file.
			std::cout << token << '\n';
	}

	// creates an inverted index
	InvertedIndex idx;

	//initializes dummy values for term insertion
	std::string term = "Hello";
	std::string docName = "Article1.json";
	int pos = 123;
	
	// Sanity check for proper inverted index insertion
	std::cout << "idx size = " << idx.getTermCount() << '\n';
	idx.addTerm(term, docName, pos);
	std::cout << "idx size = " << idx.getTermCount() << '\n';
	idx.printIndex();


	system("pause");


	//initializes dummy values for term insertion
	term = "Random Term";
	docName = "Article2.json";
	pos = 345;

	// Sanity check for proper inverted index insertion
	std::cout << "idx size = " << idx.getTermCount() << '\n';
	idx.addTerm(term, docName, pos);
	std::cout << "idx size = " << idx.getTermCount() << '\n';
	idx.printIndex();

	// close test.txt file
	file.close();
	
	system("pause");
}