#ifndef DOC_POS_H
#define DOC_POS_H

#include <cstdint>
#include <string>
#include <list>

/**
 * @brief The DocInfo class : The document info represents a single document in a single postings list.
 * This Object contains all positional instances/occurences of "term" that
 * this "document" is associated with.
 */
class DocInfo {
    /** @brief _docId : The document name that is associated with the stemmed term in the index.*/
    uint32_t _docId;

    /** @brief _posList : A list of positional occurences of the term associated with the document.*/
    std::list<uint32_t> _posList;

public:
    /** @brief DocInfo : Create an empty postings list for a document
     *  @param docId : The document id to create an empty postings list for
     */
    DocInfo(const uint32_t &docId);

    /**
     * @brief addPosition : Adds a position to the end of the document's posting list (not inserted)
     * @param pos : The position to add
     */
    void addPosition(const uint32_t &pos);

    /**
     * @brief getDocId : Gets the name of the document (json file name).
     * @return The document id number.
     */
	uint32_t getDocId() const;

    /**
     * @brief getPositions : Gets the list of the term's occurence.
     * @return The list of occurences.
     */
    std::list<uint32_t> getPositions() const;
};

#endif
