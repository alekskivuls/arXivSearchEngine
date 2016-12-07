#include "Engine.h"
#include "gtest/gtest.h"
#include <string>

class EngineTest : public ::testing::Test {
protected:
    virtual void SetUp() {
        //Requires CORPUS_TEST_DIR environment variable to be set
        boost::filesystem::path dir(std::getenv("CORPUS_TEST_DIR"));
        engine.createIndex(dir);
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

TEST_F(EngineTest, query)
{
    std::string query = "park";
    auto queryResult = engine.getQuery(query);
    auto itr = queryResult.begin();
    EXPECT_EQ(*itr++, "article0");
    EXPECT_EQ(*itr++, "article1");
    EXPECT_EQ(*itr++, "article3");
    EXPECT_EQ(*itr++, "article4");
    EXPECT_EQ(itr, queryResult.end());
}

TEST_F(EngineTest, loadIndex) {
    auto cwd = boost::filesystem::current_path();
    engine.loadIndex(cwd);
}
