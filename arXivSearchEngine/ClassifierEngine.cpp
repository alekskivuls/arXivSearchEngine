#define BOOST_SPIRIT_THREADSAFE
#include <boost/algorithm/string/predicate.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/filesystem.hpp>
#include "DiskInvertedIndex.h"
#include "PorterStemmer.h"
#include "InvertedIndex.h"
#include <unordered_set>
#include "Serializer.h"
#include "Tokenizer.h"
#include "DocInfo.h"
#include "ClassifierEngine.h"
#include "KSerializer.h"
#include "KDeserializer.h"

// Default constructors and destructors
//ClassifierEngine::ClassifierEngine() { }

ClassifierEngine::ClassifierEngine(DiskInvertedIndex &idx, int numFeatures) : _idx(idx), numFeatures(numFeatures) {
    generateFeaturesList();
    generateFeatureProbability();
}

//Populates the priority_queues.
void ClassifierEngine::generateFeaturesList() {
    //Nested for loop for all author and term combinations.
    for(auto author : _idx.getAuthorList()) {
        //        for(auto _author : _idx.getAuthorList()){
        //            std::cout << _author << " LIST OF THE AUTHORS " << std::endl;
        //        }
        //Getting all of the doc IDs that author wrote.
        std::list<DocInfo> authorDocs = _idx.getAuthorDocs(author);

        //        for(auto _term : _idx.getVocabList()) {
        //            std::cout << _term << " LIST OF THE TERMS " << std::endl;
        //        }

        for(auto term : _idx.getVocabList()) {
            double weight = 0;
            std::list<DocInfo> postings = _idx.getPostings(term);

            //Calling the count classes and saving values
            //If there's no need for it to be in a double. i.e. the counts...
            double classTerm = countClassTerm(postings, authorDocs);
            double justTerm = countTerm(postings, authorDocs);
            double justClass = countClass(postings, authorDocs);
            double neither = _idx.getN() - (classTerm + justTerm + justClass);

            //Calling the calculator.
            if(classTerm * justClass * justTerm * neither > 0){
                weight = featureSelect(classTerm, justTerm, justClass, neither);
            }

            //Putting it into priority queues.
            //Push it into the global std::priority_queue<std::pair<double, std::string>>
            globalClass.push(std::pair<double, std::string>(weight, term));

            //Pushing it to the other class ones.
            if(author == "MADISON")
                madison.push(std::pair<double, std::string>(weight, term));
            else if(author == "JAY")
                jay.push(std::pair<double, std::string>(weight, term));
            else if(author == "HAMILTON")
                hamilton.push(std::pair<double, std::string>(weight, term));
        }
    }

}

void ClassifierEngine::generateFeatureProbability() {
    auto featureList = getGlobalList(numFeatures);
    for(auto author : _idx.getAuthorList()) {
        if(author.compare("HAMILTON OR MADISON") != 0) {
            ClassifierClass classClass(author);
            for(auto term :featureList) {
                double classTermCount = countClassTermPostings(_idx.getPostings(term), _idx.getAuthorDocs(author));
                classClass.addTerm(term, classTermCount);
            }
            classList.push_back(classClass);
        }
    }
}

std::string ClassifierEngine::classifyDoc(const uint32_t &docId) {
    auto featureList = getGlobalList(numFeatures);
    std::string maxClass;
    double maxWeight = -INFINITY;
    for(auto classClass : classList) {
        double totalWeight = log((double)_idx.getAuthorDocs(classClass.getClassName()).size() / _idx.getN());
        for(auto term : featureList) {
            for (auto posting : _idx.getPostings(term)) {
                if (posting.getDocId() == docId) {
                    totalWeight += log(classClass.getTermProbability(term)) * posting.getPositions().size();
                    break;
                }
            }
            totalWeight += log(classClass.getTermProbability(term));
        }
        if(totalWeight > maxWeight) {
            maxWeight = totalWeight;
            maxClass = classClass.getClassName();
        }
    }
    return maxClass;
}

//Returns the number of docs thats in the class AND the term.
uint32_t ClassifierEngine::countClassTerm(std::list<DocInfo> postings, std::list<DocInfo> authorDocs) {
    std::list<DocInfo> result = queryEngine.AND(postings, authorDocs);
    return result.size();
}

//Returns the number of postings in the docs thats in the class AND the term.
uint32_t ClassifierEngine::countClassTermPostings(std::list<DocInfo> postings, std::list<DocInfo> authorDocs) {
    std::list<DocInfo> result = queryEngine.AND(postings, authorDocs);
    uint32_t postingsCount = 0;

    //FIXME Use pointers and iterators to check in O(j+k) instead of O(n^2)
    for(auto doc : result) {
        for(auto docInfo : postings) {
            if(doc.getDocId() == docInfo.getDocId()) {
                postingsCount += docInfo.getPositions().size();
            }
        }
    }
    return postingsCount;
}

//Returns the number of docs thats in just the term AND NOT in the class.
uint32_t ClassifierEngine::countTerm(std::list<DocInfo> postings, std::list<DocInfo> authorDocs) {
    std::list<DocInfo> result = queryEngine.ANDNOT(postings, authorDocs);
    return result.size();
}

//Returns the number of docs in just the class AND NOT have the term.
uint32_t ClassifierEngine::countClass(std::list<DocInfo> postings, std::list<DocInfo> authorDocs) {
    std::list<DocInfo> result = queryEngine.ANDNOT(authorDocs, postings);
    return result.size();
}

double ClassifierEngine::featureSelect(double classTerm, double noClassTerm, double noTermClass, double noTermNoClass){
    double totalDoc = classTerm + noClassTerm + noTermClass + noTermNoClass;
    double classTermCal = (classTerm/totalDoc)*std::log2((classTerm*totalDoc)/((classTerm+noClassTerm)*(classTerm+noTermClass)));
    double noClassTermCal = (noClassTerm/totalDoc)*std::log2((noClassTerm*totalDoc)/((classTerm+noClassTerm)*(noClassTerm+noTermNoClass)));
    double noTermClassCal = (noTermClass/totalDoc)*std::log2((noTermClass*totalDoc)/((noTermClass+classTerm)*(noTermClass+noTermNoClass)));
    double noTermNoClassCal = (noTermNoClass/totalDoc)*std::log2((noTermNoClass*totalDoc)/((noTermNoClass+noClassTerm)*(noTermNoClass+noTermClass)));
    return classTermCal + noClassTermCal + noTermClassCal + noTermNoClassCal;
}

//Pulls from which ever author's pq it wants. Only from madison, jay or hamilton.
//std::list<std::string> ClassifierEngine::getTopClass(std::string author, uint32_t n) {
//    //Pop and give it the second pair!
//}

//Pulls from the global priority queue
std::list<std::string> ClassifierEngine::getGlobalList(uint32_t n) {
    std::priority_queue<std::pair<double, std::string>> tempQueue(globalClass);
    std::list<std::string> list;
    for (uint32_t i = 0; i < n; i++) {
        list.push_back(tempQueue.top().second);
        tempQueue.pop();
    }
    return list;
}
