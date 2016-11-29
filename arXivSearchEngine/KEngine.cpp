#include "KEngine.h"
#include <sstream>
#include <iostream>
#include <algorithm>
//#include <bits/stdc++.h> //bad to use for programming point of view, good for program competitions.
#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/predicate.hpp>

// future design paradigm is to implement a singleton design pattern where inverted index is hidden from the main 
KEngine::KEngine() { } // future implementation will pass index into constructor: QEngine(const InvertedIndex &idx) 

/**
 * The function jaccard currently takes in three parameters, two string terms
 * you want the jaccard coefficient for and the integer kgram size.
 *
 * The jaccard coeffienct is representative of how how similar the term's kgrams are
 * taking into account the length of the terms.
 */
//change to using private _ksize?
float KEngine::jaccard(std::string &term1, std::string &term2, int kSize){
    const std::list<std::string> &grams1 = KgramIndex::getGrams(term1, kSize);
    const std::list<std::string> &grams2 = KgramIndex::getGrams(term2, kSize);
    float count = 0; //compiler auto reused did not put on stack.

	//The number of kgrams that are in both 1 and 2.
	if(grams1.size() > grams2.size()){
		for(auto grams : grams2){
            if((std::find(grams1.begin(), grams1.end(), grams)) != grams1.end()) {
                //std::cout << grams << std::endl; //nothing fell here
                count+=1.0;
            }
            //if(grams1.contains(grams)) count+=1.0;
		}
	} else {
		for(auto grams : grams1){ //versus i...
            if((std::find(grams2.begin(), grams2.end(), grams)) != grams2.end()) {
                //std::cout << grams << std::endl;
                count+=1.0;
            }
            //if(grams2.contains(grams)) count+=1.0;
		}
	}

	//count is A intersect B
    //std::cout << count << std::endl;
    //std::cout << count/((term1.size() + term2.size()) - count) << std::endl;
	return count/((term1.size() + term2.size()) - count);
}

std::list<std::string> KEngine::correctSpelling(std::string &mispelled, KgramIndex &idx) {
    const std::list<std::string> &mispgrams = KgramIndex::getGrams(mispelled, idx.getKSize());
    std::list<std::string> final;
    std::unordered_map<std::string, float> jaccarded;
    int i;
    int lowest = 20; //for edit distance
    float threshold = 1.0/4.0; //binary to cut everytime there is less.
    float jc; //storage for the jc.


    for(auto grams : mispgrams){ //grabs all the term list values from the key grams
        const std::unordered_set<std::string> loopterms = idx.getTerms(grams);
        //get the value of the grams (key) and put them in potentialterms
        //loopterms = KgramIndex::getTerms(grams); //returns the list of terms
        for(auto term : loopterms) { //loop thru that term list and get all the terms
            if (jaccarded.find(term) == jaccarded.end()) { //if you cant find the term
            //if (!(potentialterms.contains(term))) {
                //you didnt find the term...
                //get jaccardcoefficient of that term and put on jaccarded.
                jc = jaccard(mispelled, term, idx.getKSize()); //compare mispelled and terms
                if(jc >= threshold) jaccarded[term] = jc; //saving term:jc
            }
        }
    }

	//get only the ones above the threshold and then throw into editdistance.
    for(auto qualifiers : jaccarded) { //each term in jaccarded is a qual.
        if(editDistDP(mispelled, qualifiers.first) <= lowest){
            if(final.empty()){
                final.push_back(qualifiers.first);
			} else {
                //final not empty; empty if ed < lowest
                if(editDistDP(mispelled, qualifiers.first) < lowest) {
					final.clear();
                    final.push_back(qualifiers.first);
				} else {
                    //ed == lowest and its not empty, just add to final
                    final.push_back(qualifiers.first);
				}
			}
            lowest = editDistDP(mispelled, qualifiers.first);
		}
	}

	return final;
}


// Utility function to find minimum of three numbers
int KEngine::min(int x, int y, int z) {
    return std::min(std::min(x, y), z);
}
 

/** editDistDP(str1, str2)
 * Dynamic Programming to actually find the closest term.
 */
int KEngine::editDistDP(std::string term1, std::string term2) {
    // Create a table to store results of subproblems
    const size_t m = term1.length();
    const size_t n = term2.length();

	std::vector<std::vector<int>> dp;
	dp.reserve(m + 1);
	for (int i = 0; i < m + 1; ++i) {
		dp.push_back(std::vector<int>());
		dp.at(i).reserve(n + 1);
		for (int j = 0; j < n + 1; ++j) 
			dp.at(i).push_back(0);
	}
 
    // Fill d[][] in bottom up manner
    for (int i=0; i<=m; i++) {
        for (int j=0; j<=n; j++)
        {
            // If first string is empty, only option is to
            // isnert all characters of second string
            if (i==0)
                dp[i][j] = j;  // Min. operations = j

            // If second string is empty, only option is to
            // remove all characters of second string
            else if (j==0)
                dp[i][j] = i; // Min. operations = i
 
            // If last characters are same, ignore last char
            // and recur for remaining string
            else if (term1[i-1] == term2[j-1])
                dp[i][j] = dp[i-1][j-1];
 
            // If last character are different, consider all
            // possibilities and find minimum
            else
                dp[i][j] = 1 + min(dp[i][j-1],  // Insert
                                   dp[i-1][j],  // Remove
                                   dp[i-1][j-1]); // Replace
        }
    }

    return dp[m][n];
}
