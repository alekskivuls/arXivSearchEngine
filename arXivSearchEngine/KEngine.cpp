#include "KEngine.h"
#include <sstream>
#include <iostream>
#include <vector>
#include <algorithm>
#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/predicate.hpp>

// future design paradigm is to implement a singleton design pattern where inverted index is hidden from the main 
KEngine::KEngine() { } // future implementation will pass index into constructor: QEngine(const InvertedIndex &idx) 

