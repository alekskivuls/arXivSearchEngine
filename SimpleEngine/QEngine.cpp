#include "QEngine.h"

QEngine::QEngine(const InvertedIndex &idx) : _invIndex(idx) { }
QEngine::~QEngine() { }

/*
 * Takes a stemmed string query in inverse notation and parses it into a stack formatted in RPN.
 */
std::list<std::string> QEngine::infixToRPN(std::list<std::string> &invQuery) {
	std::list<std::string> rpnQuery;

	// translate query from: 
	// "stem1 * stem2 + stem3" 
	// to: 
	// [stem1, stem2, *, stem3, +] 

	return rpnQuery;
}

/*
 * This method takes a string query and returns a list of the stemmed tokens, including operators. 
 */
std::list<std::string> QEngine::stemmify(const std::string &userQuery) {
	// @TODO: Aleks Kivul. Please implement the stemiify operation (implement it however you would like).
	std::list<std::string> infix;

	// This funciton will CALL the Utility Class (Porter Stemmer) and individually stem each token. 
	// We will probably have to #include "PorterStemmer" -OR- we can make a private class inside QEngine. 
	// It will look like this: 

	/* QEngine header file:
	class QEngine {

		// The class should be a static class with static functions (utility class) 
		// The class should ONLY be visible to QEngine because... there should be no other reason to accesss 
		// The Porter Stemmer from outside...
		static class PorterStemmer { 
			//define private static methods here
		};

	public:
		//blah...
	};

	*/

	return infix;
}

/*
 * Takes a stack of stemmed strings formatted in RPN and processes a postingsList. 
 * This method will be responsible for invoking getPostings, union and intersect. 
 */
std::list<DocInfo> QEngine::processQuery(std::string &userQuery) {
	std::list<DocInfo> mergedPostings;
	std::list<std::string> rpnQuery;

	rpnQuery = infixToRPN(stemmify(userQuery));
	
	// insert getPostings, union and intersect as many times until all postings are merged into one. 
	std::stack<std::string> rpnStack;
	// while rpnStack is not empty, pop 2 operands and 1 operator, perform merge, 

	return mergedPostings;
}