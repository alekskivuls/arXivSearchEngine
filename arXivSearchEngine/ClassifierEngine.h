#ifndef CENGINE_H
#define CENGINE_H

#include "DiskInvertedIndex.h"
#include "QEngine.h"
#include "DocInfo.h"
#include <iostream>
#include <cstdint>
#include <string>
#include <queue>
#include <list>

/**
 * @brief ClassifierEngine::ClassifierEngine Generating priority queue of top n terms in classes using
 * naive bayes equation for feature selection.
 * Insert a populated DiskInvertedIndex and call driver() to start.
 * @param idx A populated DiskInvertedIndex of terms user wishes to classify.
 */
class ClassifierEngine {

    /**
     * @brief _idx The DiskInvertedIndex containing terms user wishes to classify using naive bayes.
     */
    DiskInvertedIndex &_idx;

    /**
     * @brief queryEngine QEngine for AND and ANDNOT merges to find quadrant intersections of class and term.
     */
    QEngine queryEngine;

public:

    /**
     * @brief Priority queues representing different classes that put the highest ranking terms first.
     */
    std::priority_queue<std::pair<double, std::string>> madison, jay, hamilton, globalclass;

    //Constructors
    ClassifierEngine();
    ClassifierEngine(DiskInvertedIndex &idx);

    //Runs naive bayes function with given input.
    double featureSelect(double classTerm, double noClassTerm, double noTermClass, double noTermNoClass);

    //Extract the nuber of documents for each and put into method for the score.
    double countClassTerm(std::list<DocInfo> postings, std::list<DocInfo> authorDocs);
    double countClass(std::list<DocInfo> postings, std::list<DocInfo> authorDocs);
    double countTerm(std::list<DocInfo> postings, std::list<DocInfo> authorDocs);
    double countNone(std::list<DocInfo> postings, std::list<DocInfo> authorDocs);

    void driver();

    std::list<std::string> getGlobalTop(uint32_t n);
    std::list<std::string> getTopClass(std::string author, uint32_t n);
};

#endif
