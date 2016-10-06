#ifndef DOC_POS_H
#define DOC_POS_H

#include <string>
#include <list>

/*
 * The document info represents a single document in a single postings list. 
 * This Object contains all positional instances/ occurences of "term" that 
 * this "document" is associated with.
 */
class DocInfo {
	// The document name that is associated with the stemmed term in the index.
	std::string _docName;

	// A list of positional occurences of the term associated with the document.
	std::list<int> _posList;

public:
	DocInfo();
	DocInfo(const std::string &docName);

	void DocInfo::addPosition(const int &pos);

	// Returns the name of the document (json file name).
	std::string getDocName() const;

	// Returns the list of the term's occurence.
	std::list<int> DocInfo::getPositions() const;
};

#endif