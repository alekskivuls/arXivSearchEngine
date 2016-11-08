#include "KEngine.h"
#include <sstream>
#include <iostream>
#include <algorithm>
//#include <bits/stdc++.h> //bad to use for programming point of view, good for program competitions.
#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/predicate.hpp>

// future design paradigm is to implement a singleton design pattern where inverted index is hidden from the main 
KEngine::KEngine() { } // future implementation will pass index into constructor: QEngine(const InvertedIndex &idx) 

//inefficient code.
float KEngine::Jaccard(std::string &term1, std::string &term2, int kSize){
    const std::list<std::string> &grams1 = KgramIndex::getGrams(term1, kSize);
    const std::list<std::string> &grams2 = KgramIndex::getGrams(term2, kSize);
    float count;

	//The number of kgrams that are in both 1 and 2.
	if(grams1.size() > grams2.size()){
		for(auto grams : grams2){
            if((std::find(grams1.begin(), grams1.end(), grams)) != grams1.end()) count+=1.0;
            //if(grams1.contains(grams)) count+=1.0;
		}
	} else {
		for(auto grams : grams1){ //versus i...
            if((std::find(grams2.begin(), grams2.end(), grams)) != grams2.end()) count+=1.0;
            //if(grams2.contains(grams)) count+=1.0;
		}
	}

	//count is A intersect B
	return count/((term1.size() + term2.size()) - count);
}

std::list<std::string> KEngine::CorrectSpelling(std::string &mispelled, KgramIndex &idx) {
    const std::list<std::string> &mispgrams = KgramIndex::getGrams(mispelled, idx.getKSize());
	std::list<std::string> potentialterms;
	std::list<std::string> final;
	int i;
	int highest = 0;
	float threshold = 3.0/7.0;

	for(auto grams : mispgrams){ //grabs all the term list values from the key grams
        const std::list<std::string> loopterms = idx.getTerms(grams);
		//get the value of the grams (key) and put them in potentialterms
        //loopterms = KgramIndex::getTerms(grams); //returns the list of terms
		for(auto term : loopterms) { //loop thru that term list and get all the terms
            if (!((std::find(potentialterms.begin(), potentialterms.end(), term)) != potentialterms.end())) {
            //if (!(potentialterms.contains(term))) {
                potentialterms.push_back(term); //put in one list
			}
		}
	}

	//potentialterms should have all of the term
    for(auto pterms : potentialterms) {
		//compare potential terms to the mispelled word.
        if(Jaccard(mispelled, pterms, idx.getKSize()) >= threshold)
            jaccarded[pterms] = Jaccard(mispelled, pterms, idx.getKSize());
	}

	//get only the ones above the threshold and then throw into editdistance.
	for(auto qualifiers : jaccarded) {
        if(editDistDP(mispelled, qualifiers.first) >= highest){

            if(final.empty()){
                final.push_back(qualifiers.first);
			} else {
				//final not empty; empty if ed > highest
                if(editDistDP(mispelled, qualifiers.first) > highest) {
					final.clear();
                    final.push_back(qualifiers.first);
				} else {
					//ed == highest and its not empty, just add to final
                    final.push_back(qualifiers.first);
				}
			}
            highest = editDistDP(mispelled, qualifiers.first);
		}
	}

	return final;
}


// Utility function to find minimum of three numbers
int KEngine::min(int x, int y, int z) {
    return std::min(std::min(x, y), z);
}
 

/** editDistDP(str1, str2)
 * Dynamic Programming. 
 */

int KEngine::editDistDP(std::string term1, std::string term2) {
    // Create a table to store results of subproblems
    int m = term1.length();
    int n = term2.length();
    int dp[m+1][n+1];
 
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
