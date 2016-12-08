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
    ClassifierEngine cengine(engine.dIdx);
    //0.000110536 rounded.
    std::string expected = std::string("0.000111");
    double result = cengine.featureSelect(49, 27652, 141, 774106);
    EXPECT_EQ(std::to_string(result), expected);
}

TEST_F(ClassifierTest, driver)
{
    //MADISON and HAMILTON have 2, and JAY has 1.
    ClassifierEngine cengine(engine.dIdx);
    cengine.driver();
    std::pair<double, std::string> pairing = cengine.globalclass.top();
    cengine.globalclass.pop();
    std::cout << pairing.first << " " << pairing.second << std::endl;
}
