#ifndef QUERY_LANGUAGE_H
#define QUERY_LANGUAGE_H

#include "DiskInvertedIndex.h"
#include "PorterStemmer.h"
#include "KEngine.h"
#include <cmath>
#include <stack>
#include <vector>

/**
 * @brief The QEngine class is used to process any kind of merge. Included are AND, ANDNOT, OR merges.
 */
class QEngine {
	/** private method used to split queries into tokens for processing*/
	std::vector<std::string> split(std::string const &input);

	/** A member function tha takes an existing, stemmed inverse notation query of 
	 * types std::list<std::string> and returns the reverse polish notation query*/ 
	std::list<std::string> infixToRPN(std::list<std::string> &invQuery);

	/** Takes a std::string query, stems each token in the query, and returns a list 
	 * of stemmed tokens and operators in inverse notation. 
	 * std::list<std::string> stemmify(std::string &userQuery);*/

	void correctSpelling(DiskInvertedIndex &dIdx, KgramIndex &kIdx3, std::string &token);


public:

	struct pair {
		pair() {}
		explicit pair(const uint32_t &first, const double_t &second)
			: docid(first), score(second) { }

		/*int operator()(const pair& other) const { 
			return score > other.score;
		}*/

		uint32_t docid;
		double_t score;
	};

	struct descending {
		bool operator()(const pair& l, const pair& r) const {
			return l.score < r.score;
		}
	};


	QEngine();

	std::list<DocInfo> processQuery(std::string &userQuery, DiskInvertedIndex
		&dIdx, KgramIndex &kIdx1, KgramIndex &kIdx2, KgramIndex &kIdx3);

	std::vector<std::pair<uint32_t, double_t>> rankedQuery(std::string userQuery, DiskInvertedIndex &dIdx, KgramIndex &kIdx3);

	// The methods below are used for TestDrivenDevelopment debugging.
	void printInfixRpn();
	void printInfixRpn2();
	
    std::list<DocInfo> AND(const std::list<DocInfo> &left, const std::list<DocInfo> &right);

    std::list<DocInfo> OR(const std::list<DocInfo> &left, const std::list<DocInfo> &right);

    std::list<DocInfo> ANDNOT(const std::list<DocInfo> &left, const std::list<DocInfo> &right);

    std::list<DocInfo> PHRASE(std::list<DocInfo> &left, std::list<DocInfo> &right, const int &dist);
private:

    std::vector<std::pair<uint32_t, double_t>> heapify(std::vector<pair> scores);

    /** Takes a std::string query, stems each token in the query, and returns a list
     * of stemmed tokens and operators in inverse notation. */
    std::list<std::string> stemmify(std::string &userQuery);
};

#endif
