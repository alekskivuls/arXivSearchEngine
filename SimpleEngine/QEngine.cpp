#include "QEngine.h"
#include "PorterStemmer.h"
#include <iostream>
#include <vector>
#include <boost/algorithm/string.hpp>
QEngine::QEngine(const InvertedIndex &idx) : _invIndex(idx) { } //
QEngine::~QEngine() { }

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

				if (precThis < precOther) {
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
		else 
			rpnQuery.push_back(token); // default (a.k.a. operand)
	}

	while (!opStack.empty()) {
		rpnQuery.push_back(opStack.top());
		opStack.pop();
	}

	// translate query from: 
	// "stem1 * stem2 + stem3" 
	// to: 
	// [stem1, stem2, *, stem3, +] 

	return rpnQuery;
}

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

void QEngine::split(const std::string& s, char c, std::vector<std::string>& v) {
	std::string::size_type i = 0;
	std::string::size_type j = s.find(c);

	while (j != std::string::npos) {
		v.push_back(s.substr(i, j - i));
		i = ++j;
		j = s.find(c, j);

		if (j == std::string::npos)
			v.push_back(s.substr(i, s.length()));
	}
}

/*
 * This method takes a string query and returns a list of the stemmed tokens, including operators. 
 */
std::list<std::string> QEngine::stemmify(const std::string &userQuery) {
	std::list<std::string> infix;
	std::vector<std::string> strs;
	split(userQuery, ' ', strs);
	PorterStemmer stemmer;
	bool onLiteral = false, onPlus = false;
	for (auto str : strs) {
		if (onLiteral) {
			if (str.at(str.length()-1) == '"') {
				onLiteral = false;
			}
			infix.push_back("`");
			infix.push_back(stemmer.stem(str.substr(0,str.length()-1)));
			
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
			infix.push_back(stemmer.stem(str.substr(1, std::string::npos)));
		}
		else if (str.at(0) == '-') {
			if (str.at(1) == '"') {
				onLiteral = true;
			}
			infix.push_back("~");
			infix.push_back(stemmer.stem(str.substr(1,std::string::npos)));
		}
		else if (str.at(0) == '+') {
			onPlus = true;
		}
		else {
			if (!onPlus) {
				infix.push_back("*");
			}
			else {
				infix.push_back("+");
				onPlus = false;
			}
			infix.push_back(stemmer.stem(str));
		}
	}
	return infix;
}

/*
 * Takes a stack of stemmed strings formatted in RPN and processes a postingsList. 
 * This method will be responsible for invoking getPostings, union and intersect. 
 */
std::list<DocInfo> QEngine::processQuery(std::string &userQuery, const InvertedIndex &idx) {
	//std::list<std::string> infix;
	//infix.push_back("Hello");
	//infix.push_back("`");
	//infix.push_back("World");
	//infix.push_back("`");
	std::list<std::string> rpnQuery = infixToRPN(stemmify(userQuery)); // 
	int dist = 1;
	bool prevIsPhrase;
	// insert getPostings, union and intersect as many times until all postings are merged into one. 
	std::stack<std::list<DocInfo>> result;
	std::list<DocInfo> left, right;
	for (auto token : rpnQuery) {
		if (token == "*" || token == "+" || token == "~" || token == "`") {
			prevIsPhrase = false;
			right = result.top();
			result.pop();

			left = result.top();
			result.pop();

			if (token == "*") 
				result.push(AND(left, right)); // AND
			if (token == "+") 
				result.push(OR(left, right)); // OR
			if (token == "~") // insert operator for and not
				result.push(ANDNOT(left, right)); // ANDNOT
			if (token == "`") {
				if (prevIsPhrase)
					++dist;
				else
					dist = 1;
				result.push(PHRASE(left, right, dist));
				prevIsPhrase = true;
			}
			//if (token == "^") // possibly cool future operator (assuming it makes sense)?
				//result.push(XOR(left, right));
		}
		else 
			result.push(idx.getPostings(token));
	}

	// while rpnStack is not empty, pop 2 operands and 1 operator, perform merge,
	return result.top();
}

std::list<DocInfo> QEngine::AND(const std::list<DocInfo> &left, const std::list<DocInfo> &right) {
	std::list<DocInfo> result;
	
	std::list<DocInfo> min, max;
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
		if ((*iIter).getDocName() > (*jIter).getDocName()) 
			++jIter;
		else if ((*jIter).getDocName() > (*iIter).getDocName())
			++iIter;
		else {
			result.push_back(DocInfo((*iIter).getDocName()));
			++iIter;
			++jIter;
		}
	}

	return result;
}

/*
 * Merges 2 postings list. The postingslist do not need to know the positions; therefore, a simple 
 * OR merge will only copy the document name/ id.
 */
std::list<DocInfo> QEngine::OR(const std::list<DocInfo> &left, const std::list<DocInfo> &right) {
	std::list<DocInfo> result;

	std::list<DocInfo> min, max;
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
		if ((*iIter).getDocName() > (*jIter).getDocName()) 
			result.push_back(DocInfo((*(jIter++)).getDocName()));
		else if ((*jIter).getDocName() > (*iIter).getDocName()) 
			result.push_back(DocInfo((*(iIter++)).getDocName()));
		else {
			result.push_back(DocInfo((*(iIter++)).getDocName()));
			++jIter;
		}
	}

	while (iIter != min.end()) 
		result.push_back(DocInfo((*(iIter++)).getDocName()));

	while (jIter != max.end()) 
		result.push_back(DocInfo((*(jIter++)).getDocName()));

	return result;
}

std::list<DocInfo> QEngine::ANDNOT(const std::list<DocInfo> &left, const std::list<DocInfo> &right) {
	std::list<DocInfo> result;

	auto iIter = left.begin(), jIter = right.begin(); // auto =  std::list<DocInfo>::const_iterator
	while (iIter != left.end()) {
		if ((*iIter).getDocName() > (*jIter).getDocName())
			++jIter;
		else if ((*jIter).getDocName() > (*iIter).getDocName()) {
			result.push_back(DocInfo((*iIter).getDocName()));
			++iIter;
		}
		else {
			++iIter;
			++jIter;
		}
	}

	return result;
}

std::list<DocInfo> QEngine::PHRASE(const std::list<DocInfo> &left, const std::list<DocInfo> &right, const int &dist) {
	std::list<DocInfo> result;
	std::list<DocInfo> min = left, max = right;

	auto iIter = min.begin(), jIter = max.begin();
	while (iIter != min.end() && jIter != max.end()) {
		if ((*iIter).getDocName() > (*jIter).getDocName())
			++jIter;
		else if ((*jIter).getDocName() > (*iIter).getDocName())
			++iIter;
		else { // same file/ doc name
			DocInfo merge((*iIter).getDocName());
			auto leftPos = (*iIter).getPositions(), rightPos = (*jIter).getPositions();

			auto i = leftPos.begin(), j = rightPos.begin();
			while (i != leftPos.end() && j != rightPos.end()) {
				if ((*i + dist) == *j) {
					merge.addPosition(*(i++));
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

// we can refacor this later. I still want to keep the index hidden.
InvertedIndex QEngine::getIndex() {
	return _invIndex;
}

void QEngine::printQueryTest2(InvertedIndex &idx) {
	idx.addTerm("Hello", "Article1.json", 1);
	idx.addTerm("Hello", "Article1.json", 2);

	idx.addTerm("Hello", "Article2.json", 1);
	idx.addTerm("Hello", "Article2.json", 3);
	idx.addTerm("Hello", "Article2.json", 5);

	idx.addTerm("World", "Article2.json", 2);
	idx.addTerm("World", "Article2.json", 6);

	idx.addTerm("World", "Article3.json", 1);
	idx.addTerm("World", "Article3.json", 1);

	idx.addTerm("Aleks", "Article2.json", 3);
	idx.addTerm("Aleks", "Article2.json", 7);

	auto docList = processQuery(std::string("Hello ` World"), idx);
	for (auto di : docList) {
		std::cout << di.getDocName() << ":\n";
		for (auto i : di.getPositions()) 
			std::cout << i << " ";
		std::cout << '\n';
	}
}

void QEngine::printQueryTest(InvertedIndex &idx) {
	std::list<DocInfo> left = idx.getPostings("Hello");
	std::list<DocInfo> right = idx.getPostings("World");

	std::cout << "Left = " << left.size() << '\n';
	std::cout << "Right = " << right.size() << '\n';

	std::cout << "AND Query:\n";
	std::list<DocInfo> andQuery = AND(left, right);
	for (auto doc : andQuery) {
		std::cout << doc.getDocName() << ":\n";
		for (auto i : doc.getPositions()) 
			std::cout << i << ' ';
		std::cout << '\n';
	}
	std::cout << '\n';

	std::cout << "ANDNOT Query:\n";
	std::list<DocInfo> andNotQuery = ANDNOT(left, right);
	for (auto doc : andNotQuery) {
		std::cout << doc.getDocName() << ":\n";
		for (auto i : doc.getPositions())
			std::cout << i << ' ';
		std::cout << '\n';
	}
	std::cout << '\n';

	std::cout << "OR Query:\n";
	std::list<DocInfo> orQuery = OR(left, right);
	for (auto doc : orQuery) {
		std::cout << doc.getDocName() << ":\n";
		for (auto i : doc.getPositions())
			std::cout << i << ' ';
		std::cout << '\n';
	}
	std::cout << '\n';

	// std::list<DocInfo> phraseQuery = PHRASE(left, right);
}