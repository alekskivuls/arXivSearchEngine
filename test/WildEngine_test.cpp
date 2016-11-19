#include <WildEngine.h>
#include "gtest/gtest.h"
#include <string>

class WildTest : public ::testing::Test {
 protected:
  //virtual void SetUp() {}
  //virtual void TearDown() {}

  KgramIndex index3 = KgramIndex(3);
  KgramIndex index1 = KgramIndex(1);
  KgramIndex index2 = KgramIndex(2);
//  WildEngine wengine = WildEngine();
};

/**
 * @brief TEST_F : Testing that the getGrams static method in
 * the KgramIndex class is working the way it should for 3-gram.
 */
TEST_F(WildTest, wildcard)
{
    std::string wildcard = std::string("h***o");
    std::string hello = std::string("hello");
    std::string jello = std::string("jello");
    std::string healo = std::string("healo");
    std::string wip = std::string("wip");
    index3.addTerm(hello);
    index3.addTerm(jello);
    index3.addTerm(healo);
    index3.addTerm(wip);
    index2.addTerm(hello);
    index2.addTerm(jello);
    index2.addTerm(healo);
    index2.addTerm(wip);
    index1.addTerm(hello);
    index1.addTerm(jello);
    index1.addTerm(healo);
    index1.addTerm(wip);
    //index3.vocab(); //check if pass by ref works
    std::list<std::string> res = WildEngine::potentials(wildcard, index1, index2, index3);
//    EXPECT_EQ(res.front(), "hello");
    std::cout << res.front() << std::endl;
//    EXPECT_EQ(res.front(), "ell");
//    res.pop_front();
//    EXPECT_EQ(res.front(), "llo");
//    res.pop_front();
//    EXPECT_EQ(res.front(), "lo$");
}
