#include "ClassifierEngine.h"

ClassifierEngine::ClassifierEngine(DiskInvertedIndex &idx) : _idx(idx) {}

void ClassifierEngine::generateFeaturesList() {
    for(auto className : getClassNames()) {
        std::list<DocInfo> classDocList = getClassDocInfos(className);
        for(auto term : _idx.getVocabList()) {
            double weight = 0;
            std::list<DocInfo> postings = _idx.getPostings(term);

            //Calling the count classes and saving values
            double classTerm = countClassTerm(postings, classDocList);
            double justTerm = countTerm(postings, classDocList);
            double justClass = countClass(postings, classDocList);
            double neither = countTotalTrainingDocs() - (classTerm + justTerm + justClass);

            //Calling the calculator.
            //if(classTerm * justClass * justTerm * neither > 0){
                weight = featureSelect(classTerm, justTerm, justClass, neither);
            //}

            //Pushing it into priority queues.
            globalClass.push(std::pair<double, std::string>(weight, term));
        }
    }
}

//Returns the number of postings in the docs thats in the class AND the term.
uint32_t ClassifierEngine::countClassTermPostings(std::list<DocInfo> postings, std::list<DocInfo> classDocs) {
    std::list<DocInfo> result = queryEngine.AND(postings, classDocs);
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

void ClassifierEngine::generateFeatureProbability(int numFeatures) {
    featureData = std::vector<ClassifierClass>();
    auto featureList = getNumTopFeatures(numFeatures);
    for(auto className : getClassNames()) {
        ClassifierClass classClass(className);
        for(auto term : featureList) {
            double classTermCount = countClassTermPostings(_idx.getPostings(term), getClassDocInfos(className));
            classClass.addTerm(term, classTermCount);
        }
        featureData.push_back(classClass);
    }
}

void ClassifierEngine::addTrainingDoc(const std::string &className, const uint32_t docId) {
    auto foundClass = classDocs.find(className);
    if (foundClass != classDocs.end()) { //found
        foundClass->second.push_back(docId);
    } else { //not found
        std::vector<uint32_t> vec(1, docId);
        classDocs.insert(std::make_pair(className, vec));
    }
}

void ClassifierEngine::addTrainingDocList(const std::string &className, const std::vector<uint32_t> &docIds) {
     auto foundClass = classDocs.find(className);
     if (foundClass != classDocs.end()) { //found
         foundClass->second.reserve(foundClass->second.size() + docIds.size()); //preallocate memory
         foundClass->second.insert(foundClass->second.end(), docIds.begin(), docIds.end()); //merge existing docs with doc list
     } else { //not found
         classDocs.insert(std::make_pair(className, docIds));
     }
}

std::string ClassifierEngine::classifyDoc(const uint32_t numFeatures, const uint32_t docId) {
    auto featureList = getNumTopFeatures(numFeatures);
    std::string maxClass;
    double maxWeight = -INFINITY;
    for(auto classClass : featureData) {
        double totalWeight = log((double) classDocs.at(classClass.getClassName()).size() / countTotalTrainingDocs());
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

uint32_t ClassifierEngine::countClassTerm(std::list<DocInfo> postings, std::list<DocInfo> classDocs) {
    std::list<DocInfo> result = queryEngine.AND(postings, classDocs);
    return result.size();
}

uint32_t ClassifierEngine::countTerm(std::list<DocInfo> postings, std::list<DocInfo> classDocs) {
    std::list<DocInfo> result = queryEngine.ANDNOT(postings, classDocs);
    return result.size();
}

uint32_t ClassifierEngine::countClass(std::list<DocInfo> postings, std::list<DocInfo> classDocs) {
    std::list<DocInfo> result = queryEngine.ANDNOT(classDocs, postings);
    return result.size();
}

double ClassifierEngine::featureSelect(double classTerm, double noClassTerm, double noTermClass, double noTermNoClass){
    double totalDoc = classTerm + noClassTerm + noTermClass + noTermNoClass;
    double totalSum = 0;
    if(classTerm != 0)
        totalSum += (classTerm/totalDoc)*std::log2((classTerm*totalDoc)/((classTerm+noClassTerm)*(classTerm+noTermClass)));
    if(noClassTerm != 0)
        totalSum += (noClassTerm/totalDoc)*std::log2((noClassTerm*totalDoc)/((classTerm+noClassTerm)*(noClassTerm+noTermNoClass)));
    if(noTermClass != 0)
        totalSum += (noTermClass/totalDoc)*std::log2((noTermClass*totalDoc)/((noTermClass+classTerm)*(noTermClass+noTermNoClass)));
    if(noTermNoClass != 0)
        totalSum += (noTermNoClass/totalDoc)*std::log2((noTermNoClass*totalDoc)/((noTermNoClass+noClassTerm)*(noTermNoClass+noTermClass)));
    return totalSum;
}

std::vector<std::string> ClassifierEngine::getNumTopFeatures(uint32_t n) {
    //Copy of priority queue is made so that origional content is not popped off.
    std::priority_queue<std::pair<double, std::string>> tempQueue(globalClass);
    std::vector<std::string> featureTerms;
    for (uint32_t i = 0; i < n; i++) {
        featureTerms.push_back(tempQueue.top().second);
        tempQueue.pop();
    }
    return featureTerms;
}

std::vector<std::string> ClassifierEngine::getClassNames() const {
    std::vector<std::string> classNames;
    classNames.reserve(classDocs.size());
    for(auto classDoc : classDocs) {
        classNames.push_back(classDoc.first);
    }
    return classNames;
}

std::vector<uint32_t> ClassifierEngine::getClassDocs(std::string &className) const {
    auto foundClass = classDocs.find(className);
    if (foundClass != classDocs.end()) { //found
        return foundClass->second;
    }
    std::vector<uint32_t> vec;
    return vec;
}

//FIXME Method used for boxing uint32_t into DocInfos for easy merge from Query Engine in other methods
//Change in data types may be required throughout application
std::list<DocInfo> ClassifierEngine::getClassDocInfos(std::string &className) const {
    std::list<DocInfo> classDocList;
    for(auto doc : getClassDocs(className)) {
        classDocList.push_back(DocInfo(doc));
    }
    return classDocList;
}

uint32_t ClassifierEngine::countTotalTrainingDocs() const {
    uint32_t count = 0;
    for(auto classClass : classDocs) {
        count += classClass.second.size();
    }
    return count;
}
