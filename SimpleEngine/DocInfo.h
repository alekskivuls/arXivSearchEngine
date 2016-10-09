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
	unsigned int _docId;

	// A list of positional occurences of the term associated with the document.
	std::list<int> _posList;

public:
	DocInfo();
	DocInfo(const unsigned int &docId);

<<<<<<< Updated upstream
	void DocInfo::addPosition(const int &pos);
=======
	void addPosition(const int &pos);
>>>>>>> Stashed changes

	// Returns the name of the document (json file name).
	unsigned int getDocId() const;

	// Returns the list of the term's occurence.
<<<<<<< Updated upstream
	std::list<int> DocInfo::getPositions() const;
};

#endif
=======
	std::list<int> getPositions() const;
};

#endif
>>>>>>> Stashed changes
