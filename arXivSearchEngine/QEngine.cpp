#include "QEngine.h"
#include "PorterStemmer.h"
#include "WildEngine.h"
#include <sstream>
#include <iterator>
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/predicate.hpp>

// future design paradigm is to implement a singleton design pattern where inverted index is hidden from the main 
QEngine::QEngine() { } // future implementation will pass index into constructor: QEngine(const InvertedIndex &idx) 

std::vector<uint32_t> QEngine::rankedQuery(std::string userQuery, DiskInvertedIndex &dIdx) {
	std::istringstream iss(userQuery);
	std::vector<std::string> tokens{ std::istream_iterator<std::string>{iss},
		std::istream_iterator<std::string>{} };

	uint32_t size = dIdx.getN();
    const std::vector<double_t> &weights = dIdx.ReadWeights();

	std::vector<pair> scores;
	scores.reserve(size);
	uint32_t i;
	for (i = 0; i < size; ++i) 
		scores.push_back(pair(i, 0.0)); // <-- Acc is score value

	i = 0;
	// I DID NOT SUM ACCUMULATOR YET
	for (std::string token : tokens) {
		std::string stemmedToken = PorterStemmer::stem(token);
        const std::list<DocInfo> &docList = dIdx.GetPostings(stemmedToken);

		double_t df = (double_t)docList.size();
		double_t wqt = (df == 0.0) ? 0 : log(1.0 + (size / df)); // WQT


		// FOR EACH TERM... CALC SCORES FOR ALL DOCS
		for (const DocInfo &doc : docList) {
			double_t tf =  doc.getPositions().size();
			double_t wdt = (tf == 0.0) ? 0 : 1.0 + log(tf); // WDT

			double_t Ad = wqt * wdt;
			if (Ad != 0) scores[doc.getDocId()-1].score += (Ad / weights[doc.getDocId()-1]);
		}
	}

	// SORT AND THEN RETURN TOP 10
	std::vector<uint32_t> result = heapify(scores);

	return result;
}

std::vector<uint32_t> QEngine::heapify(std::vector<pair> scores) {
	std::make_heap(scores.begin(), scores.end(), descending());

	std::vector<uint32_t> result;
    result.reserve(10);
    uint32_t i;
    for (i = 0; i < 10; ++i) {
		if (scores.front().score == 0) // max score is 0
			break;

		result.push_back(scores.front().docid);
		std::cout << "MAX: docid(" << scores.front().docid <<") score(" << scores.front().score << ")" << std::endl; // simple print debugger statement for: fire in yosemite (1.7)
		std::pop_heap(scores.begin(), scores.end(), descending()); scores.pop_back(); // gets top and pops from heap
	}

	return result;
}

/*
 * Takes a stemmed string query in inverse notation and parses it into a list formatted in RPN. 
 * It uses a shallow version of Dijkstra's Shunting Yard Algorithm. 
 */
std::list<std::string> QEngine::infixToRPN(std::list<std::string> &invQuery) {
	std::list<std::string> rpnQuery;
	std::stack<std::string> opStack;

	std::string token;

	char precThis, precOther;

	for (auto token : invQuery) { 
		if (token == "*" || token == "~" || token == "`" || token == "+" ) {
			if (token == "`") 
				precThis = 2;
			else if (token == "*" || token == "~") 
				precThis = 1;
			else
				precThis = 0;

			while (true) {
				if (opStack.empty()) {
					opStack.push(token);
					break;
				}

				if (opStack.top() == "`")
					precOther = 2;
				else if (opStack.top() == "*" || opStack.top() == "~")
					precOther = 1;
				else
					precOther = 0;

				if (precThis <= precOther) {
					rpnQuery.push_back(opStack.top());
					opStack.pop();
				}
				else {
					opStack.push(token);
					break;
				}
			}
		}
		//else if () { // THIS SHOULD BE FOR PHRASE QUERY
		//
		//}
		else { // default (a.k.a. operand)
			rpnQuery.push_back(token);
			if (!opStack.empty() && opStack.top() == "`") {
				rpnQuery.push_back(opStack.top());
				opStack.pop();
			}
		}
	}

	while (!opStack.empty()) {
		rpnQuery.push_back(opStack.top());
		opStack.pop();
	}

	// translate query from: 
	// "stem1 * stem2 + stem3" 
	// to: 
	// [stem1, stem2, *, stem3, +] 

	for (auto i = rpnQuery.begin(); i != rpnQuery.end(); ++i) {
		if (i !=rpnQuery.begin()) 
			std::cout << ", ";
		std::cout << *i;
	}
	std::cout << std::endl;

	return rpnQuery;
}

/*
 * Splits a std::string (query) into a vector of std::strings (tokens) based on all white space
 */
std::vector<std::string> QEngine::split(std::string const &input) {
	std::istringstream buffer(input);
	std::vector<std::string> ret((std::istream_iterator<std::string>(buffer)),
		std::istream_iterator<std::string>());
	return ret;
}

/*
 * This method takes a string query and returns a list of the stemmed tokens, including operators. 
 */
std::list<std::string> QEngine::stemmify(std::string &userQuery) {
	std::transform(userQuery.begin(), userQuery.end(), userQuery.begin(), ::tolower);
	std::vector<std::string> strs = split(userQuery);
	std::list<std::string> infix;
	
	PorterStemmer stemmer;
	bool onLiteral = false, onPlus = false, first = true;
	for (auto str : strs) {
		std::string procStr = str;
		if (str.find("-") && !boost::algorithm::starts_with(str,"-")) {
			boost::erase_all(procStr, "-");
		}
		else if (boost::algorithm::starts_with(str, "-")) {
			procStr = procStr.substr(1, std::string::npos);
			boost::erase_all(procStr, "-");
		}

		if (onLiteral) {
			if (str.at(str.length()-1) == '"') 
				onLiteral = false;
			infix.push_back("`");
			std::string substr = procStr.substr(0,str.length()-1);
			infix.push_back(stemmer.stem(substr));
		}
		else if (str.at(0) == '"') {
			onLiteral = true;
			if (onPlus) {
				infix.push_back("+");
				onPlus = false;
			}
			else {
				infix.push_back("*");
			}
			std::string substr = procStr.substr(1, std::string::npos);
			infix.push_back(stemmer.stem(substr));
		}
		else if (str.at(0) == '-') {
			if (str.at(1) == '"') 
				onLiteral = true;
			infix.push_back("~");
			std::string substr = procStr.substr(0,std::string::npos);
			infix.push_back(stemmer.stem(substr));
		}
		else if (str.at(0) == '+') {
			onPlus = true;
		}
		else {
			if (!onPlus) 
				infix.push_back("*");
			else {
				infix.push_back("+");
				onPlus = false;
			}
			infix.push_back(stemmer.stem(procStr));
		}
		if (first) {
			infix.pop_front();
			first = false;
		}
	}
	return infix;
}

/*
 * Takes a stack of stemmed strings formatted in RPN and processes a postingsList. 
 * This method will be responsible for invoking getPostings, AND, OR, ANDNOT and PHRASE. 
 */
std::list<DocInfo> QEngine::processQuery(std::string &userQuery, DiskInvertedIndex 
	&dIdx, KgramIndex &kIdx1, KgramIndex &kIdx2, KgramIndex &kIdx3) {
	std::list<std::string> infix = stemmify(userQuery);

	if (infix.size() == 0) {
        return dIdx.GetPostings("");
	}

	if (infix.size() == 1) {
        for (auto d : dIdx.GetPostings(infix.front())) {
			std::cout << d.getDocId() << ":\n";
			for (auto i : d.getPositions()) {
				std::cout << i << " ";
			}
			std::cout << "\n";
		}
		std::cout << "\n";
        return dIdx.GetPostings(infix.front());
	}

	std::list<std::string> rpnQuery = infixToRPN(infix);
	int dist;
	bool prevIsPhrase = false;
	std::stack<std::list<DocInfo>> result;
	std::list<DocInfo> left, right;
	for (auto token : rpnQuery) { // if size is not 0 AND the string CONTAINS *, the token is a wildcard query
		if (token.size() != 0 && token.find("*") != std::string::npos) {
			std::list<std::string> expansion = WildEngine::potentials(token, kIdx1, kIdx2, kIdx3);

            std::list<DocInfo> ans;
			std::list<std::string>::iterator itr = expansion.begin();
			for (; itr != expansion.end(); ++itr) {
				if (itr == expansion.begin()) 
					ans = dIdx.GetPostings(*itr);

                std::list<DocInfo> curr = dIdx.GetPostings(*itr);
				ans = OR(ans, curr);
			}

			result.push(ans);
		}
		else if (token == "*" || token == "+" || token == "~" || token == "`") {
			right = result.top();
			result.pop();

			left = result.top();
			result.pop();

			if (token == "`") {
				dist = (prevIsPhrase) ? dist + 1 : 1;
				result.push(PHRASE(left, right, dist));
				prevIsPhrase = true;
			}
			else {
				prevIsPhrase = false;
				if (token == "*")
					result.push(AND(left, right)); // AND
				if (token == "+")
					result.push(OR(left, right)); // OR
				if (token == "~") // insert operator for and not
					result.push(ANDNOT(left, right)); // ANDNOT
			}
			//if (token == "^") // possibly cool future operator (assuming it makes sense)?
				//result.push(XOR(left, right));
		}
		else {
			if (dIdx.GetPostings(token).size() == 0) { // check spelling correction
				std::list<std::string> &candidates = KEngine::correctSpelling(token, kIdx3);
				if (candidates.size() >= 1 && candidates.front() != token) { // mispelled
					token = candidates.front();
					std::cout << "Did you mean: " << token << std::endl; // REPLACE LOGIC LATER (FOR ALEKS)
				}
				else {
					std::cout << "There are no spelling corrections available for token(" << token << ")." << std::endl;
				}
			}
			result.push(dIdx.GetPostings(token)); // must be a token... check for spelling correction?
		}
	}
	return result.top();
}

/*
 * Merges 2 postings list and returns the resulting postings list. The postingslist do not need to
 * know the positions; therefore, a simple AND merge will only copy the document name/ id.
 */
std::list<DocInfo> QEngine::AND(const std::list<DocInfo> &left, const std::list<DocInfo> &right) {
	std::list<DocInfo> result, min, max;
	if (left.size() < right.size()) {
		min = left;
		max = right;
	}
	else {
		min = right;
		max = left;
	}
	
	auto iIter = min.begin(), jIter = max.begin();
	while (iIter != min.end() && jIter != max.end()) {
		if ((*iIter).getDocId() > (*jIter).getDocId()) 
			++jIter;
		else if ((*jIter).getDocId() > (*iIter).getDocId())
			++iIter;
		else {
			result.push_back(DocInfo((*iIter).getDocId()));
			++iIter;
			++jIter;
		}
	}

	return result;
}

/*
 * Merges 2 postings list and returns the resulting postings list. The postingslist do not need to 
 * know the positions; therefore, a simple OR merge will only copy the document name/ id.
 */
std::list<DocInfo> QEngine::OR(const std::list<DocInfo> &left, const std::list<DocInfo> &right) {
	std::list<DocInfo> result, min, max;
	if (left.size() < right.size()) {
		min = left;
		max = right;
	}
	else {
		min = right;
		max = left;
	}

	auto iIter = min.begin(), jIter = max.begin();
	while (iIter != min.end() && jIter != max.end()) {
		if ((*iIter).getDocId() > (*jIter).getDocId()) 
			result.push_back(DocInfo((*(jIter++)).getDocId()));
		else if ((*jIter).getDocId() > (*iIter).getDocId()) 
			result.push_back(DocInfo((*(iIter++)).getDocId()));
		else {
			result.push_back(DocInfo((*(iIter++)).getDocId()));
			++jIter;
		}
	}

	while (iIter != min.end()) 
		result.push_back(DocInfo((*(iIter++)).getDocId()));

	while (jIter != max.end()) 
		result.push_back(DocInfo((*(jIter++)).getDocId()));

	return result;
}

/*
 * Merges 2 postings list and returns the resulting postings list. The postingslist do not need to
 * know the positions; therefore, a simple ANDNOT merge will only copy the document name/ id.
 */
std::list<DocInfo> QEngine::ANDNOT(const std::list<DocInfo> &left, const std::list<DocInfo> &right) {
	std::list<DocInfo> result;
	if (right.size() == 0)
		return left;

	auto iIter = left.begin(), jIter = right.begin(); // auto =  std::list<DocInfo>::const_iterator
	while (iIter != left.end()) {
		if ((*iIter).getDocId() > (*jIter).getDocId()) 
			++jIter;
		else if ((*jIter).getDocId() > (*iIter).getDocId()) {
			result.push_back(DocInfo((*iIter).getDocId()));
			++iIter;
		}
		else {
			++iIter;
			++jIter;
		}
	}

	return result;
}

/*
 * Merges 2 postings list and returns the resulting postings list. The postingslist DO need to
 * know the positions; therefore, this algorithm performs 2 walks: 
 *		1)	one walk along term names like the AND query.
 *		2)	one walk along the positions for each matching docId
 */
std::list<DocInfo> QEngine::PHRASE(std::list<DocInfo> &left, std::list<DocInfo> &right, const int &dist) {
	std::list<DocInfo> result;
	auto iIter = left.begin(), jIter = right.begin();
	while (iIter != left.end() && jIter != right.end()) {
		if ((*iIter).getDocId() > (*jIter).getDocId())
			++jIter;
		else if ((*jIter).getDocId() > (*iIter).getDocId())
			++iIter;
		else {
			DocInfo merge((*iIter).getDocId());
			auto leftPos = (*iIter).getPositions(), rightPos = (*jIter).getPositions();

			auto i = leftPos.begin(), j = rightPos.begin();
			while (i != leftPos.end() && j != rightPos.end()) {
				if ((*i + dist) == *j) {
					merge.addPosition(*(i++)); // add position
					++j;
				}
				else if ((*i + dist) > *j)
					++j;
				else
					++i;
			}
			result.push_back(merge);
			++iIter;
			++jIter;
		}
	}
	
	return result;
}

// Infix, rpn test 1
void QEngine::printInfixRpn() {
	std::list<std::string> infix, rpn;
	infix.push_back("Hello");
	infix.push_back("+");
	infix.push_back("World");
	infix.push_back("+");
	infix.push_back("Hello");
	infix.push_back("*");
	infix.push_back("World");
	infix.push_back("+");
	infix.push_back("Query");
	infix.push_back("*");
	infix.push_back("This");
	infix.push_back("*");
	infix.push_back("String");

	rpn = infixToRPN(infix);

	std::cout << "Size of infix = " << infix.size() << "\n";
	for (auto s : infix)
		std::cout << s << " ";
	std::cout << "\n";

	std::cout << "Size of rpn = " << rpn.size() << "\n";
	for (auto s : rpn)
		std::cout << s << " ";
	std::cout << "\n";
}

// Infix, rpn test 2
void QEngine::printInfixRpn2() {
	std::list<std::string> infix, rpn;
	infix.push_back("A");
	infix.push_back("+");
	infix.push_back("B");
	infix.push_back("*");
	infix.push_back("C");
	infix.push_back("+");
	infix.push_back("D");

	rpn = infixToRPN(infix);

	std::cout << "Size of infix = " << infix.size() << "\n";
	for (auto s : infix)
		std::cout << s << " ";
	std::cout << "\n";

	std::cout << "Size of rpn = " << rpn.size() << "\n";
	for (auto s : rpn)
		std::cout << s << " ";
	std::cout << "\n";
}
