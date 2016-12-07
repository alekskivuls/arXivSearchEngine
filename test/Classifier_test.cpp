#include "ClassifierEngine.h"
#include "gtest/gtest.h"
#include <string>

class ClassifierTest : public ::testing::Test {
protected:
    virtual void SetUp() {
    }

    virtual void TearDown() {
        //Add generated file cleanup
    }


    ClassifierEngine cengine;
};

//Test that a case of porter stemmer is still working
TEST_F(ClassifierTest, calculations)
{
    //0.000110536 rounded.
    std::string expected = std::string("0.000111");
    double result = cengine.featureSelect(49, 27652, 141, 774106);
    EXPECT_EQ(std::to_string(result), expected);
}

TEST_F(ClassifierTest, diskWrite)
{

}
