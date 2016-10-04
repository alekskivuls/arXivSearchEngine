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

	std::list<DocInfo> AND(const std::list<DocInfo> &left, const std::list<DocInfo> &right);

	std::list<DocInfo> OR(const std::list<DocInfo> &left, const std::list<DocInfo> &right);

	std::list<DocInfo> ANDNOT(const std::list<DocInfo> &left, const std::list<DocInfo> &right);

	std::list<DocInfo> QEngine::PHRASE(const std::list<DocInfo> &left, const std::list<DocInfo> &right, const int &dist);

public:
	QEngine(const InvertedIndex &idx);
	~QEngine();

	std::list<DocInfo> processQuery(std::string &userQuery, const InvertedIndex &idx);

	InvertedIndex getIndex();

	void printInfixRpn();
	void printInfixRpn2();
	void split(const std::string & s, char c, std::vector<std::string>& v);
	void printQueryTest(InvertedIndex &idx);
};

#endif