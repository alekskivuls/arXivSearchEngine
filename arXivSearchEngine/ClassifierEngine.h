#ifndef CENGINE_H
#define CENGINE_H

#include "PorterStemmer.h"
#include "InvertedIndex.h"

#include <boost/filesystem.hpp>
#include "PorterStemmer.h"
#include "InvertedIndex.h"
#include <unordered_map>
#include "WildEngine.h"
#include "Tokenizer.h"
#include "QEngine.h"
#include "DocInfo.h"
#include <algorithm>
#include <iostream>
#include <string>
#include <vector>
#include <list>
#include <queue>

#include "KSerializer.h"
#include "KEngine.h"
#include "KgramIndex.h"
#include "DiskInvertedIndex.h"
#include "ClassifierClass.h"

class ClassifierEngine {
    boost::filesystem::path dir;

    DiskInvertedIndex &_idx; //Store the vocabulary, stem and unstem.
    QEngine queryEngine; //maybe use to classify and merges.
    std::list<ClassifierClass> classList;
    int numFeatures;

    //Need something to count number of term in class, not in...
    //class is author.classTerm
public:
    //Greatest double first.
    std::priority_queue<std::pair<double, std::string>> madison, jay, hamilton, globalClass;

    ClassifierEngine();
    ClassifierEngine(DiskInvertedIndex &idx, int numFeatures);
    //Extract the nuber of documents for each and put into method for the score.
    double featureSelect(double classTerm, double noClassTerm, double noTermClass, double noTermNoClass);

    uint32_t countClassTerm(std::list<DocInfo> postings, std::list<DocInfo> authorDocs);
    uint32_t countClassTermPostings(std::list<DocInfo> postings, std::list<DocInfo> authorDocs);
    uint32_t countClass(std::list<DocInfo> postings, std::list<DocInfo> authorDocs);
    uint32_t countTerm(std::list<DocInfo> postings, std::list<DocInfo> authorDocs);

    void generateFeaturesList();
    void generateFeatureProbability();
    std::string classifyDoc(const uint32_t &docId);

    //Get the all time top rank.
    std::list<std::string> getGlobalList(uint32_t n);
    //Access the Hamilton, Jay or Madison Classes
    std::list<std::string> getTopClass(std::string author, uint32_t n);

    //Set the DiskInvertedIndex
    //void setIndex(DiskInvertedIndex &idx);
};

#endif
