#include "Engine.h"
#include "gtest/gtest.h"
#include <string>

class EngineTest : public ::testing::Test {
protected:
    virtual void SetUp() {
        //Requires CORPUS_TEST_DIR environment variable to be set
        engine.index(std::getenv("CORPUS_TEST_DIR"));
    }

    virtual void TearDown() {
        //Add generated file cleanup
    }


    Engine engine;
};

//Test that a case of porter stemmer is still working
TEST_F(EngineTest, porterStemmer)
{
    std::string caresses = std::string("caresses");
    EXPECT_EQ(engine.stem(caresses),"caress");
}

TEST_F(EngineTest, diskWrite)
{

}

TEST_F(EngineTest, vocab)
{
    int VOCAB_SIZE = 13;
    auto vocab = engine.getVocab();
    EXPECT_EQ(vocab.size(),VOCAB_SIZE);
    auto itr = vocab.begin();
    EXPECT_EQ(*itr++, "about");
    EXPECT_EQ(*itr++, "ar");
    EXPECT_EQ(*itr++, "coral");
    EXPECT_EQ(*itr++, "explor");
    EXPECT_EQ(*itr++, "in");
    EXPECT_EQ(*itr++, "learn");
    EXPECT_EQ(*itr++, "nation");
    EXPECT_EQ(*itr++, "park");
    EXPECT_EQ(*itr++, "reef");
    EXPECT_EQ(*itr++, "site");
    EXPECT_EQ(*itr++, "studi");
    EXPECT_EQ(*itr++, "the");
    EXPECT_EQ(*itr++, "thi");
    EXPECT_EQ(itr, vocab.end());
}
