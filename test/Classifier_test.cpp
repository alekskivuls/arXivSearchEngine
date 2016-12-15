#include "Engine.h"
#include "ClassifierEngine.h"
#include "gtest/gtest.h"
#include "DiskInvertedIndex.h"
#include <string>

class ClassifierTest : public ::testing::Test {
protected:
    virtual void SetUp() {
        //Called everytime you set up test case.
        //Requires CORPUS_TEST_DIR environment variable to be set
        boost::filesystem::path dir(std::getenv("CORPUS_TEST_DIR"));
        engine.createIndex(dir);
        //cengine = ClassifierEngine(engine.dIdx);
    }

    virtual void TearDown() {
        //Add generated file cleanup
    }

    Engine engine;
    //ClassifierEngine cengine;
};

TEST_F(ClassifierTest, calculations)
{
    ClassifierEngine cengine(engine.dIdx);
    //0.000110536 rounded.
    std::string expected = std::string("0.000111");
    double result = cengine.featureSelect(49, 27652, 141, 774106);
    EXPECT_EQ(std::to_string(result), expected);
}

TEST_F(ClassifierTest, populate_pq)
{
    ClassifierEngine cEngine(engine.dIdx);
    for(auto author : engine.dIdx.getAuthorList()) {
        if(author!="HAMILTON OR MADISON")
            for(auto doc : engine.dIdx.getAuthorDocs(author)) {
                cEngine.addTrainingDoc(author, doc.getDocId());
            }
    }
    uint32_t numFeatures = 10;
    cEngine.generateFeaturesList();
    cEngine.generateFeatureProbability(numFeatures);

    //MADISON and HAMILTON have 2, and JAY has 1.
    //std::string explor = std::string("explor"); //Order dependent
    std::string bayes = std::string("0.970951");
    auto topFeatures = cEngine.getNumTopFeatures(numFeatures);
    for(auto feature : topFeatures)
        std::cout << feature << std::endl;
    EXPECT_NE(find(topFeatures.begin(), topFeatures.end(), "thi"), topFeatures.end());
    EXPECT_NE(find(topFeatures.begin(), topFeatures.end(), "reef"), topFeatures.end());
    EXPECT_NE(find(topFeatures.begin(), topFeatures.end(), "explor"), topFeatures.end());
    EXPECT_NE(find(topFeatures.begin(), topFeatures.end(), "coral"), topFeatures.end());
    //EXPECT_EQ(topFeature.front(), explor);
    //EXPECT_EQ(cEngine.globalClass.top().second, explor);
    EXPECT_EQ(std::to_string(cEngine.globalClass.top().first), bayes);
}
