#include <boost\algorithm\string\predicate.hpp>
#include <boost\lambda\lambda.hpp>
#include <boost\filesystem.hpp>
#include "InvertedIndex.h"
#include <unordered_map>
#include <unordered_set>
#include "Tokenizer.h"
#include "QEngine.h"
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
	// QEngine RPN Test:
	QEngine queryEngine(idx);
	queryEngine.printInfixRpn();
	queryEngine.printInfixRpn2();
	system("pause");

	std::cout << "idx size = " << idx.getTermCount() << '\n';
	std::cout << "Adding Hello Terms...\n";
	idx.addTerm("Hello", "Article1.json", 1); // 
	idx.addTerm("Hello", "Article1.json", 2);
	idx.addTerm("Hello", "Article1.json", 3);
	idx.addTerm("Hello", "Article1.json", 10);

	idx.addTerm("Hello", "Article2.json", 4);
	idx.addTerm("Hello", "Article2.json", 6);
	idx.addTerm("Hello", "Article2.json", 8);
	idx.addTerm("Hello", "Article2.json", 30);
	std::cout << "idx size = " << idx.getTermCount() << '\n';
	
	std::cout << "idx size = " << idx.getTermCount() << '\n';
	std::cout << "Adding World Terms...\n";
	idx.addTerm("World", "Article2.json", 5);
	idx.addTerm("World", "Article2.json", 10);
	idx.addTerm("World", "Article2.json", 20);
	idx.addTerm("World", "Article2.json", 25);

	idx.addTerm("World", "Article3.json", 3);
	idx.addTerm("World", "Article3.json", 4);
	idx.addTerm("World", "Article3.json", 19);
	idx.addTerm("World", "Article3.json", 31);
	std::cout << "idx size = " << idx.getTermCount() << '\n';
	
	idx.printIndex();
	system("pause");

	std::cout << "Printing AND, OR, ANDNOT, and PHRASE Query tests:\n";
	queryEngine.printQueryTest(idx);

	// close test.txt file
	file.close();
	
	system("pause");
}