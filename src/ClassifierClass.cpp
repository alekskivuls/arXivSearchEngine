#include "ClassifierClass.h"

ClassifierClass::ClassifierClass(std::string className) : className(className) {}

std::string ClassifierClass::getClassName() const {
    return className;
}

void ClassifierClass::addTerm(const std::string& term, const uint32_t &classTermCount) {
    frequency.insert(std::pair<std::string, uint32_t>(term, classTermCount));
    totalFrequency += classTermCount;
}

double ClassifierClass::getTermProbability(const std::string& term) {
    return (double)(frequency.at(term) + 1) / (totalFrequency + frequency.size() + 1);
}
