#include "PorterStemmer.h"
#include "gtest/gtest.h"
#include <string>

std::string stem(std::string token) {
    return PorterStemmer::stem(token);
}

TEST(porter_test, 1a)
{
    EXPECT_EQ(stem("caresses"),"caress");
    EXPECT_EQ(stem("ponies"),"poni");
    EXPECT_EQ(stem("ties"),"ti");
    EXPECT_EQ(stem("caress"),"caress");
    EXPECT_EQ(stem("cats"),"cat");
}

TEST(porter_test, 1b)
{
    EXPECT_EQ(stem("feed"),"feed");
    EXPECT_EQ(stem("agreed"),"agre");//agree
    EXPECT_EQ(stem("plastered"),"plaster");
    EXPECT_EQ(stem("bled"),"bled");
    EXPECT_EQ(stem("motoring"),"motor");
    EXPECT_EQ(stem("sing"),"sing");
}

TEST(porter_test, 1bextra)
{
    EXPECT_EQ(stem("conflated"),"conflat");//conflate
    EXPECT_EQ(stem("troubled"),"troubl");//trouble
    EXPECT_EQ(stem("sized"),"size");
    EXPECT_EQ(stem("hopping"),"hop");
    EXPECT_EQ(stem("tanned"),"tan");
    EXPECT_EQ(stem("falling"),"fall");
    EXPECT_EQ(stem("hissing"),"hiss");
    EXPECT_EQ(stem("fizzed"),"fizz");
    EXPECT_EQ(stem("failing"),"fail");
    EXPECT_EQ(stem("filing"),"file");
}

TEST(porter_test, 1c)
{
    EXPECT_EQ(stem("happy"),"happi");
    EXPECT_EQ(stem("sky"),"sky");
}

TEST(porter_test, 2)
{
    EXPECT_EQ(stem("relational"),"relat");//relate
    EXPECT_EQ(stem("conditional"),"condit");//condition
    EXPECT_EQ(stem("rational"),"ration");//rational
    EXPECT_EQ(stem("valenci"),"valenc");//valence
    EXPECT_EQ(stem("hesitanci"),"hesit");//hesitance
    EXPECT_EQ(stem("digitizer"),"digit");//digitize
    EXPECT_EQ(stem("conformabli"),"conform");//conformable
    EXPECT_EQ(stem("radicalli"),"radic");//radical
    EXPECT_EQ(stem("differentli"),"differ");//different
    EXPECT_EQ(stem("vileli"),"vile");
    EXPECT_EQ(stem("analogousli"),"analog");//analogous
    EXPECT_EQ(stem("vietnamization"),"vietnam");//vietnam
    EXPECT_EQ(stem("predication"),"predic");//predicate
    EXPECT_EQ(stem("operator"),"oper");//operate
    EXPECT_EQ(stem("feudalism"),"feudal");
    EXPECT_EQ(stem("decisiveness"),"decis");//decisive
    EXPECT_EQ(stem("hopefulness"),"hope");//hopeful
    EXPECT_EQ(stem("callousness"),"callous");
    EXPECT_EQ(stem("formaliti"),"formal");
    EXPECT_EQ(stem("sensitiviti"),"sensit");//sensitive
    EXPECT_EQ(stem("sensibiliti"),"sensibl");//sensible
}

TEST(porter_test, 3)
{
    EXPECT_EQ(stem("triplicate"),"triplic");
    EXPECT_EQ(stem("formative"),"form");
    EXPECT_EQ(stem("formalize"),"formal");
    EXPECT_EQ(stem("electriciti"),"electr");//electric
    EXPECT_EQ(stem("electrical"),"electr");//electric
    EXPECT_EQ(stem("hopeful"),"hope");
    EXPECT_EQ(stem("goodness"),"good");
}

TEST(porter_test, 4)
{
    EXPECT_EQ(stem("revival"),"reviv");
    EXPECT_EQ(stem("allowance"),"allow");
    EXPECT_EQ(stem("inference"),"infer");
    EXPECT_EQ(stem("airliner"),"airlin");
    EXPECT_EQ(stem("gyroscopic"),"gyroscop");
    EXPECT_EQ(stem("adjustable"),"adjust");
    EXPECT_EQ(stem("defensible"),"defens");
    EXPECT_EQ(stem("irritant"),"irrit");
    EXPECT_EQ(stem("replacement"),"replac");
    EXPECT_EQ(stem("adjustment"),"adjust");
    EXPECT_EQ(stem("dependent"),"depend");
    EXPECT_EQ(stem("adoption"),"adopt");
    EXPECT_EQ(stem("homologou"),"homolog");
    EXPECT_EQ(stem("communism"),"commun");
    EXPECT_EQ(stem("activate"),"activ");
    EXPECT_EQ(stem("angulariti"),"angular");
    EXPECT_EQ(stem("homologous"),"homolog");
    EXPECT_EQ(stem("effective"),"effect");
    EXPECT_EQ(stem("bowdlerize"),"bowdler");
}

TEST(porter_test, 5a)
{
    EXPECT_EQ(stem("probate"),"probat");
    EXPECT_EQ(stem("rate"),"rate");
    EXPECT_EQ(stem("cease"),"ceas");
}

TEST(porter_test, 5b)
{
    EXPECT_EQ(stem("controll"),"control");
    EXPECT_EQ(stem("roll"),"roll");
}

TEST(porter_test, special)
{
    EXPECT_EQ(stem("argument"),"argument");
    EXPECT_EQ(stem("nationality"),"nation");
    EXPECT_EQ(stem("adoption"),"adopt");
    EXPECT_EQ(stem("controller"),"control");
    EXPECT_EQ(stem("roll"),"roll");
    EXPECT_EQ(stem("radically"),"radic");
}
