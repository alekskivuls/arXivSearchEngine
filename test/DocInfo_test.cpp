#include "DocInfo.h"
#include "gtest/gtest.h"

class DocInfoTest : public ::testing::Test {
protected:
    //virtual void SetUp() {}
    //virtual void TearDown() {}
};

TEST_F(DocInfoTest, getDocId)
{
    const uint32_t docId0 = 0;
    DocInfo docInfo0(docId0);
    EXPECT_EQ(docId0, docInfo0.getDocId());

    const uint32_t docId1 = 1;
    DocInfo docInfo1(docId1);
    EXPECT_EQ(docId1, docInfo1.getDocId());
}

TEST_F(DocInfoTest, addPosition)
{
    const uint32_t docId = 0;
    DocInfo docInfo(docId);
    docInfo.addPosition(0);
    docInfo.addPosition(1);

    auto positions = docInfo.getPositions();
    auto itr = positions.begin();
    EXPECT_EQ(*itr++, 0);
    EXPECT_EQ(*itr++, 1);
}
