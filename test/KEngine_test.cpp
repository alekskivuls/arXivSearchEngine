#include "KEngine.h"
#include "KgramIndex.h"
#include "gtest/gtest.h"
#include <string>

class KengineTest : public ::testing::Test {
 protected:
  //virtual void SetUp() {}
  //virtual void TearDown() {}

//  KEngine engine = KEngine();
  KgramIndex index3 = KgramIndex(3);
  KgramIndex index1 = KgramIndex(1);
  KgramIndex index = KgramIndex(0);
};

/**
 * @brief TEST_F :
 */
TEST_F(KengineTest, kgram_getgrams3)
{
    std::string hello = std::string("hello");
    std::string jello = std::string("jello");
    index3.addTerm(hello);
    index3.addTerm(jello);
    std::string hillo = std::string("hillo");
    KEngine::jaccard(hello, hillo, 3); //bigger kgram less accurate.
    EXPECT_EQ(KEngine::jaccard(jello, hillo, 3), 0.25);
    std::list<std::string> res = KEngine::correctSpelling(hillo, index3);
    EXPECT_EQ(res.size(), 1); //they are different
    EXPECT_EQ(res.front(), hello);
}

//Document feq to break tie; implement that logic in engine main.


