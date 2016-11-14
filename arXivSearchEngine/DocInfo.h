#ifndef DOC_POS_H
#define DOC_POS_H

#include <cstdint>
#include <string>
#include <list>

/**
 * The document info represents a single document in a single postings list. 
 * This Object contains all positional instances/ occurences of "term" that 
 * this "document" is associated with.
 */
class DocInfo {
	/** The document name that is associated with the stemmed term in the index.*/
	uint32_t _docId;

	/** A list of positional occurences of the term associated with the document.*/
	std::list<uint32_t> _posList;

public:
	DocInfo();
	DocInfo(const uint32_t &docId);

	void addPosition(const uint32_t &pos);

	/**Returns the name of the document (json file name).*/
	uint32_t getDocId() const;

	/**Returns the list of the term's occurence.*/
	std::list<uint32_t> getPositions() const;

	void addPosition(int &value);
};

#endif
