#define CATCH_CONFIG_MAIN  // This tells Catch to provide a main() - only do this in one cpp file
#include "catch.hpp"
#include <string>
#include <boost\foreach.hpp>
#include <boost\filesystem.hpp>
#include "PorterStemmer.h"
#include "InvertedIndex.h"
#include "Tokenizer.h"
#include "QEngine.h"
#include "DocInfo.h"

TEST_CASE("Porter Stemming", "[stemmer]") {
	PorterStemmer stemmer;

	std::cout << stemmer.stem(std::string("radicalli")) << std::endl;

	//1a
	REQUIRE(stemmer.stem(std::string("caresses")).compare(std::string("caress"))==0);
	REQUIRE(stemmer.stem(std::string("ponies")).compare(std::string("poni")) == 0);
	REQUIRE(stemmer.stem(std::string("ties")).compare(std::string("ti")) == 0);
	REQUIRE(stemmer.stem(std::string("caress")).compare(std::string("caress")) == 0);
	REQUIRE(stemmer.stem(std::string("cats")).compare(std::string("cat")) == 0);
	
	//1b
	REQUIRE(stemmer.stem(std::string("feed")).compare(std::string("feed")) == 0);
	REQUIRE(stemmer.stem(std::string("agreed")).compare(std::string("agree")) == 0);
	REQUIRE(stemmer.stem(std::string("plastered")).compare(std::string("plaster")) == 0);
	REQUIRE(stemmer.stem(std::string("bled")).compare(std::string("bled")) == 0);
	REQUIRE(stemmer.stem(std::string("motoring")).compare(std::string("motor")) == 0);
	REQUIRE(stemmer.stem(std::string("sing")).compare(std::string("sing")) == 0);

	//1bextra
	REQUIRE(stemmer.stem(std::string("conflated")).compare(std::string("conflat")) == 0);//conflate
	REQUIRE(stemmer.stem(std::string("troubled")).compare(std::string("trouble")) == 0);
	REQUIRE(stemmer.stem(std::string("sized")).compare(std::string("size")) == 0);
	REQUIRE(stemmer.stem(std::string("hopping")).compare(std::string("hop")) == 0);
	REQUIRE(stemmer.stem(std::string("tanned")).compare(std::string("tan")) == 0);
	REQUIRE(stemmer.stem(std::string("falling")).compare(std::string("fall")) == 0);
	REQUIRE(stemmer.stem(std::string("hissing")).compare(std::string("hiss")) == 0);
	REQUIRE(stemmer.stem(std::string("fizzed")).compare(std::string("fizz")) == 0);
	REQUIRE(stemmer.stem(std::string("failing")).compare(std::string("faile")) == 0);//fail
	REQUIRE(stemmer.stem(std::string("filing")).compare(std::string("file")) == 0);

	//step1c
	REQUIRE(stemmer.stem(std::string("happy")).compare(std::string("happi")) == 0);
	REQUIRE(stemmer.stem(std::string("sky")).compare(std::string("sky")) == 0);

	//step2
	REQUIRE(stemmer.stem(std::string("relational")).compare(std::string("relat")) == 0);//relate
	REQUIRE(stemmer.stem(std::string("conditional")).compare(std::string("condition")) == 0);
	REQUIRE(stemmer.stem(std::string("rational")).compare(std::string("ration")) == 0);//rational
	REQUIRE(stemmer.stem(std::string("valenci")).compare(std::string("valenc")) == 0);//valence
	REQUIRE(stemmer.stem(std::string("hesitanci")).compare(std::string("hesit")) == 0);//hesitance
	REQUIRE(stemmer.stem(std::string("digitizer")).compare(std::string("digit")) == 0);//digitize
	REQUIRE(stemmer.stem(std::string("conformabli")).compare(std::string("conform")) == 0);//conformable
	REQUIRE(stemmer.stem(std::string("radicalli")).compare(std::string("radic")) == 0);//radical
	REQUIRE(stemmer.stem(std::string("differentli")).compare(std::string("different")) == 0);
	REQUIRE(stemmer.stem(std::string("vileli")).compare(std::string("vile")) == 0);
	REQUIRE(stemmer.stem(std::string("analogousli")).compare(std::string("analogous")) == 0);
	REQUIRE(stemmer.stem(std::string("vietnamization")).compare(std::string("vietnamize")) == 0);
	REQUIRE(stemmer.stem(std::string("predication")).compare(std::string("predicate")) == 0);
	REQUIRE(stemmer.stem(std::string("operator")).compare(std::string("operate")) == 0);
	REQUIRE(stemmer.stem(std::string("feudalism")).compare(std::string("feudal")) == 0);
	REQUIRE(stemmer.stem(std::string("decisiveness")).compare(std::string("decisive")) == 0);
	REQUIRE(stemmer.stem(std::string("hopefulness")).compare(std::string("hopeful")) == 0);
	REQUIRE(stemmer.stem(std::string("callousness")).compare(std::string("callous")) == 0);
	REQUIRE(stemmer.stem(std::string("formaliti")).compare(std::string("formal")) == 0);
	REQUIRE(stemmer.stem(std::string("sensitiviti")).compare(std::string("sensitive")) == 0);
	REQUIRE(stemmer.stem(std::string("sensibiliti")).compare(std::string("sensible")) == 0);

	//step3
	REQUIRE(stemmer.stem(std::string("triplicate")).compare(std::string("triplic")) == 0);
	REQUIRE(stemmer.stem(std::string("formative")).compare(std::string("form")) == 0);
	REQUIRE(stemmer.stem(std::string("formalize")).compare(std::string("formal")) == 0);
	REQUIRE(stemmer.stem(std::string("electriciti")).compare(std::string("electric")) == 0);
	REQUIRE(stemmer.stem(std::string("electrical")).compare(std::string("electric")) == 0);
	REQUIRE(stemmer.stem(std::string("hopeful")).compare(std::string("hope")) == 0);
	REQUIRE(stemmer.stem(std::string("goodness")).compare(std::string("good")) == 0);

	//step4
	REQUIRE(stemmer.stem(std::string("revival")).compare(std::string("reviv")) == 0);
	REQUIRE(stemmer.stem(std::string("allowance")).compare(std::string("allow")) == 0);
	REQUIRE(stemmer.stem(std::string("inference")).compare(std::string("infer")) == 0);
	REQUIRE(stemmer.stem(std::string("airliner")).compare(std::string("airlin")) == 0);
	REQUIRE(stemmer.stem(std::string("gyroscopic")).compare(std::string("gyroscop")) == 0);
	REQUIRE(stemmer.stem(std::string("adjustable")).compare(std::string("adjust")) == 0);
	REQUIRE(stemmer.stem(std::string("defensible")).compare(std::string("defens")) == 0);
	REQUIRE(stemmer.stem(std::string("irritant")).compare(std::string("irrit")) == 0);
	REQUIRE(stemmer.stem(std::string("replacement")).compare(std::string("replac")) == 0);
	REQUIRE(stemmer.stem(std::string("adjustment")).compare(std::string("adjust")) == 0);
	REQUIRE(stemmer.stem(std::string("dependent")).compare(std::string("depend")) == 0);
	REQUIRE(stemmer.stem(std::string("adoption")).compare(std::string("adopt")) == 0);
	REQUIRE(stemmer.stem(std::string("homologou")).compare(std::string("homolog")) == 0);
	REQUIRE(stemmer.stem(std::string("communism")).compare(std::string("commun")) == 0);
	REQUIRE(stemmer.stem(std::string("activate")).compare(std::string("activ")) == 0);
	REQUIRE(stemmer.stem(std::string("angulariti")).compare(std::string("angular")) == 0);
	REQUIRE(stemmer.stem(std::string("homologous")).compare(std::string("homolog")) == 0);
	REQUIRE(stemmer.stem(std::string("effective")).compare(std::string("effect")) == 0);
	REQUIRE(stemmer.stem(std::string("bowdlerize")).compare(std::string("bowdler")) == 0);

	//step5a
	REQUIRE(stemmer.stem(std::string("probate")).compare(std::string("probat")) == 0);
	REQUIRE(stemmer.stem(std::string("rate")).compare(std::string("rate")) == 0);
	REQUIRE(stemmer.stem(std::string("cease")).compare(std::string("ceas")) == 0);

	//step5b
	REQUIRE(stemmer.stem(std::string("controll")).compare(std::string("control")) == 0);
	REQUIRE(stemmer.stem(std::string("roll")).compare(std::string("roll")) == 0);

	//Special
	REQUIRE(stemmer.stem(std::string("argument")).compare(std::string("argument")) == 0);
	REQUIRE(stemmer.stem(std::string("nationality")).compare(std::string("nation")) == 0);
	REQUIRE(stemmer.stem(std::string("adoption")).compare(std::string("adopt")) == 0);
	REQUIRE(stemmer.stem(std::string("controller")).compare(std::string("control")) == 0);
	REQUIRE(stemmer.stem(std::string("roll")).compare(std::string("roll")) == 0);
	REQUIRE(stemmer.stem(std::string("radically")).compare(std::string("radic")) == 0);
}

TEST_CASE("Positional Inverted Index", "[stemmer]") {
	std::string filepath = "testFiles";
	boost::filesystem::path dir(filepath);
	boost::filesystem::directory_iterator it(dir), eod;
	InvertedIndex idx;
	PorterStemmer stemmer;
	std::string input;
	BOOST_FOREACH(boost::filesystem::path const &p, std::make_pair(it, eod))
	{
		if (boost::filesystem::is_regular_file(p))
		{
			std::fstream file;
			file.open(p.string(), std::fstream::in | std::fstream::out | std::fstream::app);
			std::string input;
			int posIndex = 0;
			while (std::getline(file, input)) {
				Tokenizer tkzr(input);
				std::string token;
				while (tkzr.nextToken(token)) {
					idx.addTerm(stemmer.stem(token), p.string(), posIndex);
					posIndex++;
				}
			}
			file.close();
		}
	}
}

TEST_CASE("Query Processing", "[qengine]") {

}