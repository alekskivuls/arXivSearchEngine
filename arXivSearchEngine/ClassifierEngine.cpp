#include "ClassifierEngine.h"

ClassifierEngine::ClassifierEngine(DiskInvertedIndex &idx, std::vector<std::string> &classList) : _idx(idx), classList(classList) {
    generateFeaturesList();
    numFeatures = -1;
}

void ClassifierEngine::generateFeaturesList() {
    for(auto author : _idx.getAuthorList()) {

        //Getting all of the doc IDs that author wrote. std::list<uint32_t> auto converts to std::list<DocInfo>.
        std::list<DocInfo> authorDocs = _idx.getAuthorDocs(author);

        for(auto term : _idx.getVocabList()) {
            double weight = 0;
            std::list<DocInfo> postings = _idx.getPostings(term);

            //Calling the count classes and saving values
            double classTerm = countClassTerm(postings, authorDocs);
            double justTerm = countTerm(postings, authorDocs);
            double justClass = countClass(postings, authorDocs);
            double neither = _idx.getN() - (classTerm + justTerm + justClass);

            //Calling the calculator.
            if(classTerm * justClass * justTerm * neither > 0){
                weight = featureSelect(classTerm, justTerm, justClass, neither);
            }

            //Pushing it into priority queues.
            globalClass.push(std::pair<double, std::string>(weight, term));
        }
    }
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

void ClassifierEngine::generateFeatureProbability() {
    classListData = std::list<ClassifierClass>();
    auto featureList = getNumTopFeatures(numFeatures);
    for(auto author : _idx.getAuthorList()) {
        if(author.compare("HAMILTON OR MADISON") != 0) {
            ClassifierClass classClass(author);
            for(auto term :featureList) {
                double classTermCount = countClassTermPostings(_idx.getPostings(term), _idx.getAuthorDocs(author));
                classClass.addTerm(term, classTermCount);
            }
            classListData.push_back(classClass);
        }
    }
}

std::string ClassifierEngine::classifyDoc(const uint32_t numFeatures, const uint32_t docId) {
    if(this->numFeatures != numFeatures) {
        this->numFeatures = numFeatures;
        generateFeatureProbability();
    }

    auto featureList = getNumTopFeatures(numFeatures);
    std::string maxClass;
    double maxWeight = -INFINITY;
    for(auto classClass : classListData) {
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

uint32_t ClassifierEngine::countClassTerm(std::list<DocInfo> postings, std::list<DocInfo> authorDocs) {
    std::list<DocInfo> result = queryEngine.AND(postings, authorDocs);
    return result.size();
}

uint32_t ClassifierEngine::countTerm(std::list<DocInfo> postings, std::list<DocInfo> authorDocs) {
    std::list<DocInfo> result = queryEngine.ANDNOT(postings, authorDocs);
    return result.size();
}

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

std::vector<std::string> ClassifierEngine::getNumTopFeatures(uint32_t n) {
    //Copy of priority queue is made so that origional content is not popped off.
    std::priority_queue<std::pair<double, std::string>> tempQueue(globalClass);
    std::vector<std::string> list;
    for (uint32_t i = 0; i < n; i++) {
        list.push_back(tempQueue.top().second);
        tempQueue.pop();
    }
    return list;
}
