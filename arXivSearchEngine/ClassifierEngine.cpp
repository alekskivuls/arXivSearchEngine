#include "ClassifierEngine.h"

ClassifierEngine::ClassifierEngine(DiskInvertedIndex &idx) : _idx(idx) { }

/**
 * @brief ClassifierEngine::driver Populates the priority queues with calculated
 * naive bayes function. Which may be retrieved with getTopClass() or getGlobalTop().
 */
void ClassifierEngine::driver() {
    for(auto author : _idx.getAuthorList()) {

        //Getting all of the doc IDs that author wrote.
        std::list<uint32_t> rawAuthorDocs = _idx.getAuthorDocs(author);
        std::list<DocInfo> authorDocs;

        for(auto docId : rawAuthorDocs)
            authorDocs.push_back(DocInfo(docId));

        for(auto term : _idx.getVocabList()) {
            double weight = 0;

            //Getting all of the postings for that term.
            std::list<DocInfo> postings = _idx.GetPostings(term);

            std::cout << term << " AUTHOR IS HERE " << author << std::endl;

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
            globalclass.push(std::pair<double, std::string>(weight, term));

            //Pushing it into respecive class std::priority_queue<std::pair<double, std::string>>
            if(author == "MADISON")
                madison.push(std::pair<double, std::string>(weight, term));
            else if(author == "JAY")
                jay.push(std::pair<double, std::string>(weight, term));
            else //Assumed Hamilton Class.
                hamilton.push(std::pair<double, std::string>(weight, term));
        }
    }
}

/**
 * @brief ClassifierEngine::countClassTerm Returns the number of docs that is in the class AND the term.
 * The top left quadrant of (1,1)<term, class>.
 * @param postings The std::list<DocInfo> of the document IDs that the term is contained in.
 * @param authorDocs The std::list<DocInfo> of the document IDs that the authors wrote.
 * @return A double value that is the number of docs that is in the class AND the term.
 */
double ClassifierEngine::countClassTerm(std::list<DocInfo> postings, std::list<DocInfo> authorDocs) {
    std::list<DocInfo> result = queryEngine.AND(postings, authorDocs);
    return result.size();
}

/**
 * @brief ClassifierEngine::countTerm Returns the number of docs that only falls in the term (not the class).
 * The top right quadrant of (1,0)<term, class>.
 * @param postings The std::list<DocInfo> of the document IDs that the term is contained in.
 * @param authorDocs The std::list<DocInfo> of the document IDs that the authors wrote.
 * @return A double value that is the number of docs that is in the term AND NOT the class.
 */
double ClassifierEngine::countTerm(std::list<DocInfo> postings, std::list<DocInfo> authorDocs) {
    std::list<DocInfo> result = queryEngine.ANDNOT(postings, authorDocs);
    return result.size();
}

/**
 * @brief ClassifierEngine::countClass Returns the number of docs that only falls in the class (not the term).
 * The bottom left quadrant of (0,1)<term, class>.
 * @param postings The std::list<DocInfo> of the document IDs that the term is contained in.
 * @param authorDocs The std::list<DocInfo> of the document IDs that the authors wrote.
 * @return A double value that is the number of docs that is in the class AND NOT the term.
 */
double ClassifierEngine::countClass(std::list<DocInfo> postings, std::list<DocInfo> authorDocs) {
    std::list<DocInfo> result = queryEngine.AND(authorDocs, postings);
    return result.size();
}

/**
 * @brief ClassifierEngine::featureSelect Calculates the result using naieve bayes function.
 * @param classTerm The top left quadrant of (1,1)<term, class>.
 * @param noClassTerm The top right quadrant of (1,0)<term, class>.
 * @param noTermClass The bottom left quadrant of (0,1)<term, class>.
 * @param noTermNoClass The bottom right quadrant of (0,0)<term, class>.
 * @return A double value that is the result of naieve bayes function calculation.
 */
double ClassifierEngine::featureSelect(double classTerm, double noClassTerm, double noTermClass, double noTermNoClass){
    double totalDoc = classTerm + noClassTerm + noTermClass + noTermNoClass;
    double classTermCal = (classTerm/totalDoc)*std::log2((classTerm*totalDoc)/((classTerm+noClassTerm)*(classTerm+noTermClass)));
    double noClassTermCal = (noClassTerm/totalDoc)*std::log2((noClassTerm*totalDoc)/((classTerm+noClassTerm)*(noClassTerm+noTermNoClass)));
    double noTermClassCal = (noTermClass/totalDoc)*std::log2((noTermClass*totalDoc)/((noTermClass+classTerm)*(noTermClass+noTermNoClass)));
    double noTermNoClassCal = (noTermNoClass/totalDoc)*std::log2((noTermNoClass*totalDoc)/((noTermNoClass+noClassTerm)*(noTermNoClass+noTermClass)));
    return classTermCal + noClassTermCal + noTermClassCal + noTermNoClassCal;
}

/**
 * @brief getTopClass After driver() is called you can use this method to get the top n scored terms for
 * the specified author's priority queue.
 * It returns a std::list<std::string> of terms where you can use the front() method from the list to
 * get a reference and navigate from there.
 * @param author The author class you want the top terms for, limited to hamilton, jay, and madison.
 * @param n The number of top term you want to get.
 * @return A std::list<std::string> of the top n terms with the highest in the front of the list.
 */
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

/**
 * @brief getGlobalTop After driver() is called you can use this method to get the top n scored terms.
 * It returns a std::list<std::string> of terms where you can use the front() method from the list to
 * get a reference and navigate from there.
 * @param n The number of top term you want to get.
 * @return A std::list<std::string> of the top n terms with the highest in the front of the list.
 */
std::list<std::string> ClassifierEngine::getGlobalTop(uint32_t n) {
    int i;
    std::pair<double, std::string> pairing;
    std::list<std::string> result;
    for (i = 0; i < n; ++i) {
        pairing = globalclass.top();
        result.push_back(pairing.second);
        globalclass.pop();
    }
}
