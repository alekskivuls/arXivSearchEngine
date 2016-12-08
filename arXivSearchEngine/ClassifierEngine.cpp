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

ClassifierEngine::ClassifierEngine(DiskInvertedIndex &idx) : _idx(idx) { }

//Populates the priority_queues.
void ClassifierEngine::driver() {
    //Driver method so I only have to process this once.
    //Nested for loop for all author and term combinations.
    for(auto author : _idx.getAuthorList()) {
//        for(auto _author : _idx.getAuthorList()){
//            std::cout << _author << " LIST OF THE AUTHORS " << std::endl;
//        }
        //Getting all of the doc IDs that author wrote.
        std::list<uint32_t> rawAuthorDocs = _idx.getAuthorDocs(author);
        std::list<DocInfo> authorDocs;
        for(auto docId : rawAuthorDocs)
            authorDocs.push_back(DocInfo(docId));

//        for(auto _term : _idx.getVocabList()) {
//            std::cout << _term << " LIST OF THE TERMS " << std::endl;
//        }

        for(auto term : _idx.getVocabList()) {
            double weight = 0;

            //Getting all of the postings for that term.
            std::list<DocInfo> postings = _idx.GetPostings(term);

            std::cout << term << " AUTHOR IS HERE " << author << std::endl;

            //Calling the count classes and saving values
            //If there's no need for it to be in a double. i.e. the counts...
            //Should I still put it as double?
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
            globalclass.push(std::pair<double, std::string>(weight, term));

            //Pushing it to the other class ones.
            if(author == "MADISON")
                madison.push(std::pair<double, std::string>(weight, term));
            else if(author == "JAY")
                jay.push(std::pair<double, std::string>(weight, term));
            else //Better be Hamilton or error.
                hamilton.push(std::pair<double, std::string>(weight, term));
        }
    }

}

//Returns the number of docs thats in the class AND the term.
double ClassifierEngine::countClassTerm(std::list<DocInfo> postings, std::list<DocInfo> authorDocs) {
    std::list<DocInfo> result = queryEngine.AND(postings, authorDocs);
    return result.size();
}

//Returns the number of docs thats in just the term AND NOT in the class.
double ClassifierEngine::countTerm(std::list<DocInfo> postings, std::list<DocInfo> authorDocs) {
    std::list<DocInfo> result = queryEngine.ANDNOT(postings, authorDocs);
    return result.size();
}

//Returns the number of docs in just the class AND NOT have the term.
double ClassifierEngine::countClass(std::list<DocInfo> postings, std::list<DocInfo> authorDocs) {
    std::list<DocInfo> result = queryEngine.AND(authorDocs, postings);
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
std::list<std::string> getTopClass(std::string author, uint32_t n) {
    //Pop and give it the second pair!
}

//Pulls from the global priority queue
std::list<std::string> getGlobalTop(uint32_t n) {

}
