#define BOOST_SPIRIT_THREADSAFE
#include <boost/algorithm/string/predicate.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/filesystem.hpp>
#include <unordered_set>
#include "DiskInvertedIndex.h"
#include "PorterStemmer.h"
#include "InvertedIndex.h"
#include "Serializer.h"
#include "Tokenizer.h"
#include "DocInfo.h"
#include "Engine.h"
#include "KSerializer.h"
#include "KDeserializer.h"
#include "ClassifierEngine.h"

// Default constructors and destructors
Engine::Engine() {}


void Engine::getPathNames(const boost::filesystem::path &directory, std::vector<std::string> &mPathList) { // normal json parser
    boost::filesystem::directory_iterator end_itr;
    std::unordered_set<std::string> fileSet;

    for (boost::filesystem::directory_iterator itr(directory); itr != end_itr; ++itr) {
        if (is_regular_file(itr->path())) {
            std::string s = itr->path().string();
            if (boost::algorithm::ends_with(s, ".json")) {
                std::replace(s.begin(), s.end(), '\\', '/');
                //std::cout << s << '\n';
                fileSet.insert(s);
            }
        }
    }

    mPathList.resize(fileSet.size());
    int i = 0;
    for (auto s : fileSet)
        mPathList[i++] = s;
}

std::vector<std::string> split(std::string token) {
    std::vector<std::string> vect;
    std::stringstream ss(token);

    char c;
    std::string str = "";
    while (ss >> c)
    {
        if (c != '-') // if NOT hyphen
            str += c;
        else {
            vect.push_back(str);
            str = "";
        }
    }
    vect.push_back(str);

    return vect;
}

void Engine::updateTf(std::unordered_map<std::string, uint32_t> &wdt, const std::string &term) {
    if (wdt.find(term) == wdt.end())
        wdt[term] = 1;
    else
        wdt[term] = wdt.at(term) + 1;
}

double_t Engine::calcEucDist(std::unordered_map<std::string, uint32_t> &wdt) { // check for query: fire in yosemite... top rank should be 1.7
    double_t Ld = 0.0;
    for (const std::pair<std::string, uint32_t> &pr : wdt) {
        double_t tf = (double_t)pr.second;
        double_t wgt = 1.0 + log((double_t)tf);
        Ld += (wgt * wgt);
    }

    return sqrt(Ld);
}


void Engine::rocchio() {
	std::unordered_map<std::string, std::list<DocInfo>> classes;
	std::unordered_map<std::string, std::vector<double_t>> centroid;

	std::list<DocInfo> hamAndMad;
	std::list<DocInfo> input;
	
	auto temp = dIdx.getAuthorList();
	for (std::string s : temp) {
		//std::transform(s.begin(), s.end(), s.begin(), ::tolower);
		std::cout << "String value = " << s << std::endl;
		if (s == "HAMILTON" || s == "MADISON") {
			std::cout << "Size = " << dIdx.getAuthorDocs(s).size() << std::endl;
			classes.insert(std::pair<std::string, std::list<DocInfo>>(s, dIdx.getAuthorDocs(s)));
			centroid.insert(std::pair<std::string, std::vector<double_t>>(s, std::vector<double_t>()));
		}
		else if (s == "HAMILTON OR MADISON") 
			input = dIdx.getAuthorDocs(s);
		else if (s == "HAMILTON AND MADISON") 
			hamAndMad = dIdx.getAuthorDocs(s);
	}

    const std::list<std::string> &vocab = dIdx.getVocabList();
	std::vector<double_t> idf;
	idf.reserve(vocab.size());

	//  FOREVERY VECTOR I NEED T COMPONENTS (where t denotes the total # terms)
	std::vector<std::vector<double_t>> vSpace;
	vSpace.reserve(dIdx.getN()); // need 85 vectors	   
	uint32_t i = 0, j = 0;
	for (i = 0; i < dIdx.getN(); ++i) {
		vSpace.push_back(std::vector<double_t>());
		vSpace.at(i).reserve(vocab.size());
		for (j = 0; j < vocab.size(); ++j)
			vSpace.at(i).push_back(0.0);
	}

	std::vector<double_t> ld;
	ld.reserve(dIdx.getN());
	for (i = 0; i < dIdx.getN(); ++i)
		ld.push_back(0.0);


	
	//IDF-t
	double_t N = (double_t)dIdx.getN(); // THIS METHOD IS FINE
    for (const std::string &term : vocab) {
		uint32_t dft = dIdx.getPostings(term).size();
		double_t idft = (dft == 0) ? 0.0 : 1.0 + log10(N / (double_t)dft); // 0.0
		//printf("N = %.1f, dft = %d, idft = %.3f\n", N, dft, idft);
		idf.push_back(idft);
	}


	
	//TF-td
	uint32_t col = 0;
    for (const std::string &term : vocab) { // READING LEFT TO RIGHT
        const std::list<DocInfo> &postings = dIdx.getPostings(term);
		
        for (const DocInfo &doc : postings) {
			const double_t &tftd = (double_t)doc.getPositions().size();
			double_t wdt = 1.0 + log(tftd);

			ld[doc.getDocId()] = ld.at(doc.getDocId()) + (wdt * wdt); // (tftd * tftd)
			vSpace.at(doc.getDocId()).at(col) = idf.at(col) * tftd; // tf-t,d
		}
		++col;
	}

	for (i = 0; i < ld.size(); ++i) 
		ld[i] = sqrt(ld.at(i));

	for (i = 0; i < vSpace.size(); ++i) {
		for (j = 0; j < vSpace.at(i).size(); ++j) 
			vSpace.at(i).at(j) = vSpace.at(i).at(j) / ld.at(i);
	}

	// initialize centroid vectors
	for (std::pair<const std::string, std::vector<double_t>> &pr : centroid) {
		for (i = 0; i < vocab.size(); ++i) 
			pr.second.push_back(0.0);
	}

	for (auto pr : classes) {
		std::vector<double_t> &C = centroid.at(pr.first);

		// for every document in the class
		for (DocInfo doc : pr.second) {

			for (j = 0; j < vocab.size(); ++j) {
				// NORMALIZE THE VECTOR COMPONENT
				double_t tfidf = vSpace.at(doc.getDocId()).at(j);
				// UPDATE CENTROID VECTOR COMPONENT
				C[j] = C.at(j) + tfidf;
			}
		}

		
		// for every document in the class
		for (DocInfo &doc : hamAndMad) { // HAMILTON AND MADISON

			for (j = 0; j < vocab.size(); ++j) {
				// NORMALIZE THE VECTOR COMPONENT
				double_t tfidf = vSpace.at(doc.getDocId()).at(j);
				// UPDATE CENTROID VECTOR COMPONENT
				C[j] = C.at(j) + tfidf;
			}
		}

		std::cout << "pr.second (CLASSES) = " << pr.second.size() << std::endl;
		std::cout << "C.SIZE() = " << C.size() << std::endl;
		for (j = 0; j < vocab.size(); ++j) 
			C[j] = C.at(j) / (pr.second.size() + hamAndMad.size()); // C[j] = C.at(j) / C.size(); // not this one... (pr.second.size() + hamAndMad.size()) pr.second.size()
	}

	std::unordered_map<uint32_t, std::string> output;
	for (DocInfo &in : input) {
		std::string ans = "";
		double_t min = 0.0; // change later.

		for (auto pr : centroid) {
			double_t curr;

			for (j = 0; j < pr.second.size(); ++j) {
				double_t diff = pr.second.at(j) - (vSpace.at(in.getDocId()).at(j));

				curr += (diff * diff); // square the components aka square the differences
			}
			curr = sqrt(curr);

			pr.second;


			if (ans == "") {
				ans = pr.first;
				min = curr;
			}
			else if (min > curr) {
				ans = pr.first;
				min = curr;
			}
		}


		printf("in = %d, min = %.3f\n", in.getDocId(), min);
		output.insert(std::pair<uint32_t, std::string>(in.getDocId(), ans));
	}


	for (auto pr : output) {
		std::cout << "Document id = " << pr.first << " ";
		std::cout << "(" << idTable.at(pr.first) << ")" << std::endl;
		std::cout << "is MOST RELEVANT to author: " << pr.second << std::endl << std::endl;
	}
}


void Engine::populateIndex(const boost::filesystem::path &inDir, const boost::filesystem::path &outDir) {
    std::chrono::time_point<std::chrono::system_clock> totalStart, totalEnd;
    totalStart = std::chrono::system_clock::now();

    idTable = std::unordered_map<uint32_t, std::string>();
    auto idx = InvertedIndex();
    std::unordered_map<std::string, std::string> cache;
    std::vector<std::string> mPathList;
    getPathNames(inDir, mPathList);

    std::vector<double_t> ld = std::vector<double_t>(); // VOCAB POSITION, SCORE
    ld.reserve(mPathList.size());

    std::sort(mPathList.begin(), mPathList.end());

    uint32_t i = 0;
    for (auto p : mPathList) {
        std::cout << "Processing Article (" << (i) << "): " << boost::filesystem::path(p).stem() << ".json" << std::endl;
		//ld.push_back(0.0);
        std::unordered_map<std::string, uint32_t> wdt;

        // reads json file into stringstream and populates a json tree
        std::ifstream file(p);
        std::stringstream ss;
        ss << file.rdbuf();
        file.close();

        boost::property_tree::ptree pt;
        boost::property_tree::read_json(ss, pt);
        boost::filesystem::path dir(p);
        (idTable)[i] = dir.stem().string();

        //std::cout << "json to map...\n";
        // iterate through .json tree
        for (auto& pair : pt) {
            if (pair.first == "body") { // if author... get json array and process the authors as well. || pair.first == "title"
                std::string input = pair.second.get_value<std::string>();
                std::transform(input.begin(), input.end(), input.begin(), ::tolower);

                Tokenizer tkzr(input);
                std::string token;
                token.reserve(200);
                uint32_t posIndex = 0;
                bool hyphen = false;
                while (tkzr.nextToken(token, hyphen)) {
                    // while not end of file.
                    // Get stem the token or retrieve the value from a cache
                    if (!hyphen) {
                        kIdx1.addTerm(token);
                        kIdx2.addTerm(token);
                        kIdx3.addTerm(token);

                        std::string stemmedToken = (cache.find(token) != cache.end())
                                ? cache[token] : PorterStemmer::stem(token);
                        idx.addTerm(stemmedToken, i, posIndex); // stemmedToken
                        updateTf(wdt, stemmedToken);
                    }
                    else {
                        std::string total = "";
                        for (auto s : split(token)) {
                            kIdx1.addTerm(s);
                            kIdx2.addTerm(s);
                            kIdx3.addTerm(s);
							total += s;

                            std::string str = std::string(s);
                            idx.addTerm(PorterStemmer::stem(str), i, posIndex);
                            updateTf(wdt, str);
                        }
                        std::string &totalToken = total;
                        //std::string &totalToken = PorterStemmer::stem(total);
                        idx.addTerm(PorterStemmer::stem(totalToken), i, posIndex);
                        updateTf(wdt, totalToken);

                        kIdx1.addTerm(totalToken);
                        kIdx2.addTerm(totalToken);
                        kIdx3.addTerm(totalToken);
                    }


                    posIndex++;
                }
            } else if (pair.first == "title") { // we don't care about this...

            } else if(pair.first == "author") {
                /*
                std::string author = pair.second.get_value<std::string>();
                std::transform(author.begin(), author.end(), author.begin(), ::tolower);
                idx.addAuthorDoc(author, i);

                std::cout << "fed paper: " << idTable.at(i) << "; class: " << author << std::endl;
                */
                std::string authorStr = pair.second.get_value<std::string>();
                //std::transform(author.begin(), author.end(), author.begin(), ::tolower);

                std::string delimiter = " AND ";
                size_t pos = 0;
                std::string token;
                while ((pos = authorStr.find(delimiter)) != std::string::npos) {
                    token = authorStr.substr(0, pos);
                    idx.addAuthorDoc(token, i);
                    authorStr.erase(0, pos + delimiter.length());
                }
                idx.addAuthorDoc(authorStr, i);
//            } else if(pair.first == "identifier") { //FIXME Use own class, currently author overwrite
//                std::string identifier = pair.second.get_value<std::string>();
//                idx.addAuthorDoc(identifier.substr(0, identifier.find("/")), i);
            }
        }

        /*std::cout << "SIZE OF MAP = " << wdt.size() << std::endl;
        for (auto pr : wdt) {
            std::cout << "first = " << pr.first << " second = " << pr.second << std::endl;
        }*/
		ld.push_back(calcEucDist(wdt));
		wdt = std::unordered_map<std::string, uint32_t>();
        ++i;
    }
    // test write print
    /*std::cout << "TEST PRINT FOR WRITING EUCLIDEAN DISTANCE." << std::endl;
    for (double_t &d : ld) {
        std::cout << d << std::endl;
    }*/
    totalEnd = std::chrono::system_clock::now();
    std::chrono::duration<double> elapsed_seconds = totalEnd-totalStart;
    std::cout << "Total elapsed time for Populate Index: " << elapsed_seconds.count() << "s." << std::endl;

    Serializer::buildIndex(outDir, idx, idTable, ld); // populates all .bin files
    KSerializer::buildIndex(outDir, kIdx1, kIdx2, kIdx3);
    dIdx = DiskInvertedIndex(boost::filesystem::path(outDir));
}

void Engine::printRank(std::string &query) {
    auto list = getRank(query);
    for(auto element : list)
        std::cout << "doc name: " << getArticleName(element.first)  << " ->\tscore: " << element.second << std::endl;
}

std::vector<std::pair<uint32_t, double_t>> Engine::getRank(std::string &query) {
    return queryEngine.rankedQuery(query, dIdx, kIdx3);
}

void Engine::createIndex(const boost::filesystem::path &filepath) {
    auto cwd = boost::filesystem::current_path();
    //std::cout << "Current path is : " << cwd << std::endl;
    createIndex(filepath, cwd);
}

void Engine::createIndex(const boost::filesystem::path &inDir, const boost::filesystem::path &outDir) {
    populateIndex(inDir, outDir);
}

void Engine::loadIndex(const boost::filesystem::path &filepath) {
    dIdx = DiskInvertedIndex(boost::filesystem::path(filepath));
    KDeserializer kds(filepath);
    kds.toKgramIndex(kIdx1, kIdx2, kIdx3);
	idTable = DiskInvertedIndex::ReadIdTableFromFile(boost::filesystem::path(filepath));
}

std::string Engine::getArticleName(const uint32_t &docid) {
	return idTable.at(docid);
}

/*
void Engine::printIndex() {
    typedef std::pair<std::string, std::list<DocInfo>> pair;
    for (const pair &p : dIdx.getIndex()) {
        std::cout << "Term (" << p.first << ") found in the following documents:" << std::endl;
        for (DocInfo doc : p.second) { // list of positions
            std::cout << "Document id " << doc.getDocId() << " positions(" << doc.getPositions().size() << "): " << std::endl;
            for (uint32_t &pos : doc.getPositions())
                std::cout << pos << " ";
            std::cout << std::endl;
        }
        std::cout << std::endl;
    }
}*/

void Engine::printVocab() {
    auto vocab = getVocab();
    for(auto term : vocab)
        std::cout << term << std::endl;
    std::cout << vocab.size() << std::endl;
}

std::list<std::string> Engine::getVocab() {
    return dIdx.getVocabList();
}

std::string Engine::stem(std::string &token) {
    return PorterStemmer::stem(token);
}

void Engine::printQuery(std::string &query) {
    auto output = getQuery(query);
    for (auto di : output)
        std::cout << di << '\t';
    std::cout << std::endl << output.size() << std::endl << std::endl;
}

std::vector<std::string> Engine::getQuery(std::string &query) {
    std::list<DocInfo> output = queryEngine.processQuery(query, dIdx, kIdx1, kIdx2, kIdx3);
    std::vector<std::string> results;
    results.reserve(output.size());
    for (auto di : output)
        results.push_back(getArticleName(di.getDocId()));

    return results;
}

std::list<std::string> Engine::getAuthors() {
    return dIdx.getAuthorList();
}

void Engine::printAuthors() {
    auto authors = getAuthors();
    for(auto author : authors) {
        std::cout << author << std::endl;
    }
    std::cout << authors.size() << std::endl;
}

std::list<DocInfo> Engine::getAuthorDocs(const std::string &author) {
    return dIdx.getAuthorDocs(author);
}

void Engine::printAuthorDocs(const std::string &author) {
    auto authorDocs = getAuthorDocs(author);
    for(auto docId : authorDocs) {
        std::cout << getArticleName(docId.getDocId()) << std::endl;
    }
    std::cout << authorDocs.size() << std::endl;
}

void Engine::classifyDocuments(const uint32_t numFeatures) {
//    ClassifierEngine cEngine(dIdx);
//    for(auto author : dIdx.getAuthorList()) {
//        auto authorDocs = dIdx.getAuthorDocs(author);
//        std::random_shuffle(authorDocs.begin(), authorDocs.end());
//        auto currDoc = authorDocs.begin();
//        for(int i = 0; i < authorDocs.size()*0.8; i++) {
//            cEngine.addTrainingDoc(author, currDoc->getDocId());
//            currDoc++;
//        }
//    }
//    cEngine.generateFeaturesList();
//    cEngine.generateFeatureProbability(numFeatures);
    //for(int i = authorDocs.size()*0.8; i < authorDocs.size(); i++) {
    //    std::cout << getArticleName(docId.getDocId()) << ": " << cEngine.classifyDoc(numFeatures, docId.getDocId()) << std::endl;
    //}
}

void Engine::classifyFederalistDocuments(const uint32_t numFeatures) {
    std::string classifyClassName = "HAMILTON OR MADISON";

    ClassifierEngine cEngine(dIdx);
    for(auto author : dIdx.getAuthorList()) {
        if(author != classifyClassName) {
            for(auto doc : dIdx.getAuthorDocs(author)) {
                cEngine.addTrainingDoc(author, doc.getDocId());
            }
        }
    }
    cEngine.generateFeaturesList();
    cEngine.generateFeatureProbability(numFeatures);

    for(auto docId : dIdx.getAuthorDocs(classifyClassName)) {
        std::cout << getArticleName(docId.getDocId()) << ": " << cEngine.classifyDoc(numFeatures, docId.getDocId()) << std::endl;
    }
}
