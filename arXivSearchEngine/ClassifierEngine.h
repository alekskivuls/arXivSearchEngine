#ifndef CENGINE_H
#define CENGINE_H

#include "DiskInvertedIndex.h"
#include "ClassifierClass.h"
#include "QEngine.h"
#include "DocInfo.h"
#include <iostream>
#include <string>
#include <vector>
#include <queue>
#include <list>

/**
 * @brief ClassifierEngine::ClassifierEngine Generating priority queue of top n terms in classes using
 * naive bayes equation for feature selection.
 * Insert a populated DiskInvertedIndex and call driver() to start.
 */
class ClassifierEngine {

    /**
     * @brief _idx The DiskInvertedIndex containing terms user wishes to classify using naive bayes.
     * Stored vocabulary of stemed and unstemed.
     */
    DiskInvertedIndex &_idx;

    /**
     * @brief queryEngine QEngine for AND and ANDNOT merges to find quadrant intersections of class and term.
     */
    QEngine queryEngine;

    std::list<ClassifierClass> classList;

    /**
     * @brief numFeatures an int value of the number of features ClassifierEngine contains.
     */
    int numFeatures;

public:

    /**
     * @brief Priority queues representing different classes that put the highest ranking terms first.
     */
    std::priority_queue<std::pair<double, std::string>> madison, jay, hamilton, globalClass;

    //Constructors
    ClassifierEngine();
    ClassifierEngine(DiskInvertedIndex &idx, int numFeatures);

    //Runs naive bayes function with given input.
    double featureSelect(double classTerm, double noClassTerm, double noTermClass, double noTermNoClass);

    //Extract the number of documents for each and put into method for the score.
    uint32_t countClassTermPostings(std::list<DocInfo> postings, std::list<DocInfo> authorDocs);
    uint32_t countClassTerm(std::list<DocInfo> postings, std::list<DocInfo> authorDocs);
    uint32_t countClass(std::list<DocInfo> postings, std::list<DocInfo> authorDocs);
    uint32_t countTerm(std::list<DocInfo> postings, std::list<DocInfo> authorDocs);

    void generateFeaturesList();
    void generateFeatureProbability();
    std::string classifyDoc(const uint32_t &docId);

    std::list<std::string> getGlobalList(uint32_t n);

    //Access the Hamilton, Jay or Madison Classes
    std::list<std::string> getTopClass(std::string author, uint32_t n);
};

#endif
