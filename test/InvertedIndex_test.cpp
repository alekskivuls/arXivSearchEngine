#include "InvertedIndex.h"
#include "gtest/gtest.h"

class InvertedIndexTest : public ::testing::Test {
protected:
    virtual void SetUp() {}
    virtual void TearDown() {}

    InvertedIndex idx;
};

TEST_F(InvertedIndexTest, getVocabList)
{

}

TEST_F(InvertedIndexTest, addTerm)
{

}

TEST_F(InvertedIndexTest, hasPostings)
{

}

TEST_F(InvertedIndexTest, getPostings)
{

}

TEST_F(InvertedIndexTest, getIndex)
{

}

TEST_F(InvertedIndexTest, getTermCount)
{

}

TEST_F(InvertedIndexTest, authorDocs)
{
    std::string author("author");
    idx.addAuthorDoc(author, 0);
    auto docList = idx.getAuthorDocs(author);
    auto itr = docList.begin();
    EXPECT_EQ(*itr++, 0);
}
