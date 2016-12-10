#ifndef CLASSIFIER_CLASS_H
#define CLASSIFIER_CLASS_H

#include <unordered_map>
#include <string>

class ClassifierClass
{
    std::string className;

    std::unordered_map<std::string, uint32_t> frequency;

    uint32_t totalFrequency = 0;

public:
    ClassifierClass(std::string className);

    std::string getClassName() const;

    void addTerm(const std::string& term, const uint32_t &classTermCount);

    double getTermProbability(const std::string& term);
};

#endif
