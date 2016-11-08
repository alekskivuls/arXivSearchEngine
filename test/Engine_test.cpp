#include "Engine.h"
#include "gtest/gtest.h"
#include <string>

class EngineTest : public ::testing::Test {
 protected:
  //virtual void SetUp() {}
  //virtual void TearDown() {}

  Engine engine;
};

TEST_F(EngineTest, porterStemmer)
{
    std::string caresses = std::string("caresses");
    EXPECT_EQ(engine.stem(caresses),"caress");
}
