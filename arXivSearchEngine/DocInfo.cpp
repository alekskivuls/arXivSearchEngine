#include "DocInfo.h"

// Default constructors and destructors
DocInfo::DocInfo() : _posList(std::list<uint32_t>()) { }
DocInfo::DocInfo(const uint32_t &docId) : _docId(docId), _posList(std::list<uint32_t>()) { }

void DocInfo::addPosition(const uint32_t &pos) {
	for (auto it = _posList.begin(); it != _posList.end(); ++it) {
		if (pos < *it) {
			_posList.insert(it, pos);
			return;
		}
	}
	_posList.push_back(pos);
}

// Getter method for Document Name (a.k.a. id)
uint32_t DocInfo::getDocId() const {
	return _docId;
}

// Getter method for the list of all positional occurences of the term 
// related to this DocInfo. 
std::list<uint32_t> DocInfo::getPositions() const {
	return _posList;
}

void DocInfo::addPosition(int &value) {
	_posList.push_back(value);
}