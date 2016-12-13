#include "Engine.h"
#include "ClassifierEngine.h"
#include "gtest/gtest.h"
#include "DiskInvertedIndex.h"
#include <string>

class ClassifierTest : public ::testing::Test {
protected:
    virtual void SetUp() {
        //Called everytime you set up test case.
        //Requires CORPUS_TEST_DIR environment variable to be set
        boost::filesystem::path dir(std::getenv("CORPUS_TEST_DIR"));
        engine.createIndex(dir);
        //cengine = ClassifierEngine(engine.dIdx);
    }

    virtual void TearDown() {
        //Add generated file cleanup
    }

    Engine engine;
    //ClassifierEngine cengine;
};

//Test that a case of porter stemmer is still working
TEST_F(ClassifierTest, calculations)
{
    std::string classifyClassName = "HAMILTON OR MADISON";
    std::vector<std::string> classList;
    for(auto author : engine.dIdx.getAuthorList()) {
        if(author.compare(classifyClassName) != 0)
            classList.push_back(author);
    }

    ClassifierEngine cengine(engine.dIdx, classList);
    //0.000110536 rounded.
    std::string expected = std::string("0.000111");
    double result = cengine.featureSelect(49, 27652, 141, 774106);
    EXPECT_EQ(std::to_string(result), expected);
}

TEST_F(ClassifierTest, populate_pq)
{
    std::string classifyClassName = "HAMILTON OR MADISON";
    std::vector<std::string> classList;
    for(auto author : engine.dIdx.getAuthorList()) {
        if(author.compare(classifyClassName) != 0)
            classList.push_back(author);
    }

    //MADISON and HAMILTON have 2, and JAY has 1.
    ClassifierEngine cengine(engine.dIdx, classList);
    std::string explor = std::string("explor");
    std::string bayes = std::string("0.019973");
    EXPECT_EQ(cengine.globalClass.top().second, explor);
    EXPECT_EQ(std::to_string(cengine.globalClass.top().first), bayes);
    //EXPECT_EQ(cengine.getGlobalList().top().second, explor);

}
