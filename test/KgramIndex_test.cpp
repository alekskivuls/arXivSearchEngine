#include "KgramIndex.h"
#include "gtest/gtest.h"
#include <string>

class KgramsTest : public ::testing::Test {
 protected:
  //virtual void SetUp() {}
  //virtual void TearDown() {}


  KgramIndex index3 = KgramIndex(3);
  KgramIndex index1 = KgramIndex(1);
  KgramIndex index = KgramIndex(0);
};

/**
 * @brief TEST_F : Testing that the getGrams static method in
 * the KgramIndex class is working the way it should for 3-gram.
 */
TEST_F(KgramsTest, kgram_getgrams3)
{
    std::string hello = std::string("hello");
    //trying to test the getgrams method to index
    std::list<std::string> res = index3.getGrams(hello, 3);
    EXPECT_EQ(res.size(), hello.length());
    EXPECT_EQ(res.front(), "$he");
    res.pop_front();
    EXPECT_EQ(res.front(), "hel");
    res.pop_front();
    EXPECT_EQ(res.front(), "ell");
    res.pop_front();
    EXPECT_EQ(res.front(), "llo");
    res.pop_front();
    EXPECT_EQ(res.front(), "lo$");
}

/**
 * @brief TEST_F : Testing that the getGrams static method in
 * the KgramIndex class is working the way it should for 1-gram.
 */
TEST_F(KgramsTest, kgram_getgrams1)
{
    std::string hello = std::string("hello");
    //trying to test the getgrams method to index
    std::list<std::string> res = index1.getGrams(hello, 1);
    EXPECT_EQ(res.size(), hello.length());
    EXPECT_EQ(res.front(), "h");
    res.pop_front();
    EXPECT_EQ(res.front(), "e");
    res.pop_front();
    EXPECT_EQ(res.front(), "l");
    res.pop_front();
    EXPECT_EQ(res.front(), "l");
    res.pop_front();
    EXPECT_EQ(res.front(), "o");
}

/**
 * @brief TEST_F : Testing addTerm, getTerms on index3 and see if
 * the saved kgram and term is correct.
 */
TEST_F(KgramsTest, kgram_addterm3) {
    std::string hello = std::string("hello");
    std::string jello = std::string("jello");
    index3.addTerm(hello);
    index3.addTerm(jello);
    EXPECT_EQ(index3.getGramCount(), 7); //there should be 7 different grams
    std::string ell = std::string("ell");
    std::list<std::string> res = index3.getTerms(ell);
    EXPECT_EQ(res.front(), "hello");
    res.pop_front();
    EXPECT_EQ(res.front(), "jello");
    std::string $he = std::string("$he");
    res = index3.getTerms($he);
    EXPECT_EQ(res.front(), "hello");
    EXPECT_EQ(res.size(), 1);
}

