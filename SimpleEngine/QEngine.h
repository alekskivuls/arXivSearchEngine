#ifndef QUERY_LANGUAGE_H
#define QUERY_LANGUAGE_H

#include "InvertedIndex.h"
#include <stack>

class QEngine {
	InvertedIndex _invIndex;

	// A member function tha takes an existing, stemmed inverse notation query of 
	// types std::list<std::string> and returns the reverse polish notation query 
	std::list<std::string> infixToRPN(std::list<std::string> &invQuery);

	// Takes a std::string query, stems each token in the query, and returns a list 
	// of stemmed tokens and operators in inverse notation. 
	std::list<std::string> stemmify(const std::string &userQuery);

public:
	QEngine(const InvertedIndex &idx);
	~QEngine();

	std::list<DocInfo> processQuery(std::string &stemmed);
};

#endif