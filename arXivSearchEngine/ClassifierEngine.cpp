#include "ClassifierEngine.h"

ClassifierEngine::ClassifierEngine(DiskInvertedIndex &idx, int numFeatures) : _idx(idx), numFeatures(numFeatures) {
    generateFeaturesList();
    generateFeatureProbability();
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

            //Putting it into priority queues.
            //Pushing it into the global std::priority_queue<std::pair<double, std::string>>
            globalClass.push(std::pair<double, std::string>(weight, term));

            //Pushing it into respecive class std::priority_queue<std::pair<double, std::string>>
            if(author == "MADISON")
                madison.push(std::pair<double, std::string>(weight, term));
            else if(author == "JAY")
                jay.push(std::pair<double, std::string>(weight, term));
            else if(author == "HAMILTON")
                hamilton.push(std::pair<double, std::string>(weight, term));
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

std::list<std::string> ClassifierEngine::getTopClass(std::string author, uint32_t n) {
    int i;
    std::pair<double, std::string> pairing;
    std::list<std::string> result;
    if (author == "MADISON") {
        for (i = 0; i < n; ++i) {
            pairing = madison.top();
            result.push_back(pairing.second);
            madison.pop();
        }
    }
    else if (author == "JAY") {
        for (i = 0; i < n; ++i) {
            pairing = jay.top();
            result.push_back(pairing.second);
            jay.pop();
        }
    }
    else { //Assumes Hamilton.
        for (i = 0; i < n; ++i) {
            pairing = hamilton.top();
            result.push_back(pairing.second);
            hamilton.pop();
        }
    }
}

std::list<std::string> ClassifierEngine::getGlobalList(uint32_t n) {
    //Copy of priority queue is made so that origional content is not popped off.
    std::priority_queue<std::pair<double, std::string>> tempQueue(globalClass);
    std::list<std::string> list;
    for (uint32_t i = 0; i < n; i++) {
        list.push_back(tempQueue.top().second);
        tempQueue.pop();
    }
    return list;
}
