#include "Engine.h"
#include "gtest/gtest.h"
#include <string>

class KgramsTest : public ::testing::Test {
 protected:
  //virtual void SetUp() {}
  //virtual void TearDown() {}

  Engine engine;
};

TEST_F(KgramsTest, porterStemmer)
{
    std::string caresses = std::string("caresses");
    EXPECT_EQ(engine.stem(caresses),"caress");
}
