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
 * @brief Generating priority queue of top n terms in classes using
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

    std::vector<std::string> &classList;

    std::list<ClassifierClass> classListData;

    /**
     * @brief numFeatures an int value of the number of features ClassifierEngine contains.
     */
    int numFeatures;

public:

    /**
     * @brief Priority queues representing different classes that put the highest ranking terms first.
     */
    std::priority_queue<std::pair<double, std::string>> globalClass;

    //Constructors
    //ClassifierEngine();
    ClassifierEngine(DiskInvertedIndex &idx, std::vector<std::string> &classList);

    /**
     * @brief ClassifierEngine::featureSelect Calculates the result using naieve bayes function.
     * @param classTerm The top left quadrant of (1,1)<term, class>.
     * @param noClassTerm The top right quadrant of (1,0)<term, class>.
     * @param noTermClass The bottom left quadrant of (0,1)<term, class>.
     * @param noTermNoClass The bottom right quadrant of (0,0)<term, class>.
     * @return A double value that is the result of naieve bayes function calculation.
     */
    double featureSelect(double classTerm, double noClassTerm, double noTermClass, double noTermNoClass);

    //Extract the number of documents for each and put into method for the score.
    uint32_t countClassTermPostings(std::list<DocInfo> postings, std::list<DocInfo> authorDocs);

    /**
     * @brief ClassifierEngine::countClassTerm Returns the number of docs that is in the class AND the term.
     * The top left quadrant of (1,1)<term, class>.
     * @param postings The std::list<DocInfo> of the document IDs that the term is contained in.
     * @param authorDocs The std::list<DocInfo> of the document IDs that the authors wrote.
     * @return A uint32_t value that is the number of docs that is in the class AND the term.
     */
    uint32_t countClassTerm(std::list<DocInfo> postings, std::list<DocInfo> authorDocs);

    /**
     * @brief ClassifierEngine::countClass Returns the number of docs that only falls in the class (not the term).
     * The bottom left quadrant of (0,1)<term, class>.
     * @param postings The std::list<DocInfo> of the document IDs that the term is contained in.
     * @param authorDocs The std::list<DocInfo> of the document IDs that the authors wrote.
     * @return A uint32_t value that is the number of docs that is in the class AND NOT the term.
     */
    uint32_t countClass(std::list<DocInfo> postings, std::list<DocInfo> authorDocs);

    /**
     * @brief ClassifierEngine::countTerm Returns the number of docs that only falls in the term (not the class).
     * The top right quadrant of (1,0)<term, class>.
     * @param postings The std::list<DocInfo> of the document IDs that the term is contained in.
     * @param authorDocs The std::list<DocInfo> of the document IDs that the authors wrote.
     * @return A uint32_t value that is the number of docs that is in the term AND NOT the class.
     */
    uint32_t countTerm(std::list<DocInfo> postings, std::list<DocInfo> authorDocs);

    /**
     * @brief ClassifierEngine::generateFeaturesList Populates the priority queues with calculated
     * naive bayes function. Which may be retrieved with getTopClass() or getGlobalTop().
     */
    void generateFeaturesList();
    void generateFeatureProbability();
    std::string classifyDoc(const uint32_t numFeatures, const uint32_t docId);

    /**
     * @brief getGlobalList After driver() is called you can use this method to get the top n scored terms.
     * It returns a std::list<std::string> of terms where you can use the front() method from the list to
     * get a reference and navigate from there.
     * @param n The number of top term you want to get.
     * @return A std::list<std::string> of the top n terms with the highest in the front of the list.
     */
    std::vector<std::string> getNumTopFeatures(uint32_t n);

};

#endif
