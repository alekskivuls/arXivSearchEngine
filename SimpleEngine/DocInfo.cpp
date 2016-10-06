#include "DocInfo.h"

// Default constructors and destructors
DocInfo::DocInfo() { }
DocInfo::DocInfo(const std::string &docName) : _docName(docName) { }

// Adds a positional occurence of the "term" to this document.
void DocInfo::addPosition(const int &pos) {
	/*for (auto it = _posList.begin(); it != _posList.end(); ++it) {
		if (pos < *it) {
			_posList.insert(it, pos);
			return;
		}
	}*/
	_posList.push_back(pos);
}

// Getter method for Document Name (a.k.a. id)
std::string DocInfo::getDocName() const {
	return _docName;
}

// Getter method for the list of all positional occurences of the term 
// related to this DocInfo. 
std::list<int> DocInfo::getPositions() const {
	return _posList;
}