#include "KgramIndex.h"
#include "gtest/gtest.h"
#include <boost/filesystem.hpp>
#include <string>
#include "KSerializer.h"
#include "KDeserializer.h"

class KserialTest : public ::testing::Test {

public:
    KgramIndex index1 = KgramIndex(1);
    KgramIndex index2 = KgramIndex(2);
    KgramIndex index3 = KgramIndex(3);
    std::string file;

protected:
   virtual void SetUp() {
       //Requires CORPUS_TEST_DIR environment variable to be set
       file = std::getenv("CORPUS_TEST_DIR");
   }

   virtual void TearDown() {
       //Add generated file cleanup
   }
};

/**
 * @brief TEST_F : Testing that the getGrams static method in
 * the KgramIndex class is working the way it should for 3-gram.
 */
TEST_F(KserialTest, serial)
{
    KDeserializer deserial = KDeserializer(file);

    //std::string file = "C:/Users/pkim7/Documents/Visual Studio 2015/Projects/arXivSearchEngine/test/documents/testCorpus"; // // change to your input directory C:\Users\pkim7\Documents\Visual Studio 2015\Projects\arXivSearchEngine\test\documents\testCorpus
    boost::filesystem::path dir(file);
    std::string hello = std::string("hello");
    index3.addTerm(hello);
    index2.addTerm(hello);
    index1.addTerm(hello);

    KSerializer::buildIndex(dir, index1, index2, index3);
    deserial.printAllTerms(index3);
}

TEST_F(KserialTest, deserial)
{
    KDeserializer deserial = KDeserializer(file);

    //std::string file = "C:/Users/pkim7/Documents/Visual Studio 2015/Projects/arXivSearchEngine/test/documents/testCorpus"; // // change to your input directory C:\Users\pkim7\Documents\Visual Studio 2015\Projects\arXivSearchEngine\test\documents\testCorpus
    boost::filesystem::path dir(file);
    std::string hello = std::string("hello");
    std::list<std::string> kgramList;

    std::cout << "This is what is in the index1 before: " << std::endl;
    index1.vocab(); //print for before
    std::cout << std::endl;
    deserial.toKgramIndex(index1, index2, index3);
    std::cout << "This is what is in the index1 after deserial: " << std::endl;
    index1.vocab();
    std::cout << std::endl;

    kgramList = index1.getKgramList();
    std::cout << "Returned kgramList: " << kgramList.front() << std::endl;
    EXPECT_EQ(kgramList.front(), "o");
}
