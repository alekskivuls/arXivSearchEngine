#define CATCH_CONFIG_MAIN  // This tells Catch to provide a main() - only do this in one cpp file
#include "catch.hpp"
#include <string>
#include "PorterStemmer.h"
#include "InvertedIndex.h"
#include "Tokenizer.h"
#include "QEngine.h"
#include "DocInfo.h"

TEST_CASE("Porter Stemming", "[stemmer]") {
	PorterStemmer stemmer;
	stemmer.stem(std::string("test"));
	REQUIRE(std::string("test").compare(std::string("test"))==0);
}