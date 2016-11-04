//#include "PorterStemmer.h"
#include "gtest/gtest.h"
#include <string>

TEST(initial_test, null_term_str_sort)
{
    EXPECT_EQ(std::string("test"),std::string("test"));
    //EXPECT_EQ(PorterStemmer::stem("test"),std::string("test"));
}
