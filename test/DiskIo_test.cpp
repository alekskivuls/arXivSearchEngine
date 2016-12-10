#include "DiskIo.h"
#include "gtest/gtest.h"
#include <boost/filesystem.hpp>

class DiskIoTest : public ::testing::Test {
protected:
    virtual void SetUp() {}
    virtual void TearDown() {}
};

TEST_F(DiskIoTest, character) {
    auto filepath = boost::filesystem::current_path().append("/charTest.bin", boost::filesystem::path::codecvt()).string();
    std::ofstream ofstream;
    ofstream.open(filepath, std::ios_base::out | std::ios_base::binary);
    uint8_t letter = 'A';
    DiskIo::WriteChar(ofstream, letter);
    ofstream.close();

    std::cout << filepath << std::endl;

    std::ifstream ifstream;
    ifstream.open(filepath, std::ios_base::in | std::ios_base::binary);
    EXPECT_EQ(letter, DiskIo::ReadChar(ifstream));
    ifstream.close();
}

TEST_F(DiskIoTest, integer) {
    auto filepath = boost::filesystem::current_path().append("/intTest.bin", boost::filesystem::path::codecvt()).string();
    std::ofstream ofstream;
    ofstream.open(filepath, std::ios_base::out | std::ios_base::binary);
    uint32_t integer = 15;
    DiskIo::WriteInt(ofstream, integer);
    ofstream.close();

    std::cout << filepath << std::endl;

    std::ifstream ifstream;
    ifstream.open(filepath, std::ios_base::in | std::ios_base::binary);
    EXPECT_EQ(integer, DiskIo::ReadInt(ifstream));
    ifstream.close();
}

TEST_F(DiskIoTest, longs) {
    auto filepath = boost::filesystem::current_path().append("/longTest.bin", boost::filesystem::path::codecvt()).string();
    std::ofstream ofstream;
    ofstream.open(filepath, std::ios_base::out | std::ios_base::binary);
    uint64_t number = 15;
    DiskIo::WriteLong(ofstream, number);
    ofstream.close();

    std::cout << filepath << std::endl;

    std::ifstream ifstream;
    ifstream.open(filepath, std::ios_base::in | std::ios_base::binary);
    EXPECT_EQ(number, DiskIo::ReadLong(ifstream));
    ifstream.close();
}

TEST_F(DiskIoTest, decimal) {
    auto filepath = boost::filesystem::current_path().append("/doubleTest.bin", boost::filesystem::path::codecvt()).string();
    std::ofstream ofstream;
    ofstream.open(filepath, std::ios_base::out | std::ios_base::binary);
    double_t number = 3.14;
    DiskIo::WriteDouble(ofstream, number);
    ofstream.close();

    std::cout << filepath << std::endl;

    std::ifstream ifstream;
    ifstream.open(filepath, std::ios_base::in | std::ios_base::binary);
    EXPECT_EQ(number, DiskIo::ReadDouble(ifstream));
    ifstream.close();
}
