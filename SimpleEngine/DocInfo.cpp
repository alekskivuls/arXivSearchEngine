#include "DocInfo.h"

DocInfo::DocInfo() { }

DocInfo::DocInfo(const std::string &docName) : _docName(docName) { }

void DocInfo::addPosition(const int &pos) {
	for (auto it = _posList.begin(); it != _posList.end(); ++it) {
		if (pos < *it) {
			_posList.insert(it, pos);
			return;
		}
	}
	_posList.push_back(pos);
}

std::string DocInfo::getDocName() const {
	return _docName;
}

std::list<int> DocInfo::getPositions() {
	return _posList;
}