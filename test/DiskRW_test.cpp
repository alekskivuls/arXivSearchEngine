#include "DiskReader.h"
#include "DiskWriter.h"
#include "gtest/gtest.h"
#include <boost/filesystem.hpp>

class DiskRWTest : public ::testing::Test {
protected:
    virtual void SetUp() {}
    virtual void TearDown() {}
};

TEST_F(DiskRWTest, character) {
    uint8_t letter = 'A';
    {   //Scope to force flush of buffer on destruction
        DiskWriter writer = DiskWriter("/charTest.bin");
        writer.writeChar(letter);
    }

    DiskReader reader = DiskReader("/charTest.bin");
    EXPECT_EQ(letter, reader.readChar());
}

TEST_F(DiskRWTest, integer) {
    uint32_t integer = 15;
    {   //Scope to force flush of buffer on destruction
        DiskWriter writer = DiskWriter("/intTest.bin");
        writer.writeInt(integer);
    }
    DiskReader reader = DiskReader("/intTest.bin");
    EXPECT_EQ(integer, reader.readInt());
}

TEST_F(DiskRWTest, longs) {
    uint64_t longInt = 15;
    {   //Scope to force flush of buffer on destruction
        DiskWriter writer = DiskWriter("/longTest.bin");
        writer.writeLong(longInt);
    }

    DiskReader reader = DiskReader("/longTest.bin");
    EXPECT_EQ(longInt, reader.readLong());
}

TEST_F(DiskRWTest, decimal) {
    double_t decimal = 3.14;
    {   //Scope to force flush of buffer on destruction
        DiskWriter writer = DiskWriter("/doubleTest.bin");
        writer.writeDouble(decimal);
    }

    DiskReader reader = DiskReader("/doubleTest.bin");
    EXPECT_EQ(decimal, reader.readDouble());
}

TEST_F(DiskRWTest, vbeInt) {
    uint32_t integer = 15;
    {   //Scope to force flush of buffer on destruction
        DiskWriter writer = DiskWriter("/vbeIntTest.bin");
        writer.writeVbeInt(integer);
    }

    DiskReader reader = DiskReader("/vbeIntTest.bin");
    EXPECT_EQ(integer, reader.readVbeInt());
}
