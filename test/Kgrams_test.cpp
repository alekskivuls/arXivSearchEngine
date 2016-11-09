#include "KEngine.h"
#include "KgramIndex.h"
#include "gtest/gtest.h"
#include <string>

class KgramsTest : public ::testing::Test {
 protected:
  //virtual void SetUp() {}
  //virtual void TearDown() {}

  KEngine kengine;
  KgramIndex index3 = KgramIndex(3);
  KgramIndex index1 = KgramIndex(1);
  KgramIndex index = KgramIndex(0);
};

TEST_F(KgramsTest, kgram_index3)
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
}

TEST_F(KgramsTest, kgram_index1)
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
