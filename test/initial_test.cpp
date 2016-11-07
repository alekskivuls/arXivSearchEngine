#include "PorterStemmer.h"
#include "gtest/gtest.h"
#include <string>

TEST(porter_test, test)
{
    //EXPECT_EQ(std::string("test"),std::string("test"));
    std::string test = std::string("test");
    EXPECT_EQ(PorterStemmer::stem(test),std::string("test"));
}
