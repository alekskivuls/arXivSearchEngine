#include "PorterStemmer.h"
#include "gtest/gtest.h"
#include <string>

TEST(porter_test, 1a)
{
    std::string caresses = std::string("caresses");
    EXPECT_EQ(PorterStemmer::stem(caresses),"caress");
    std::string ponies = std::string("ponies");
    EXPECT_EQ(PorterStemmer::stem(ponies),"poni");
    std::string ties = std::string("ties");
    EXPECT_EQ(PorterStemmer::stem(ties),"ti");
    std::string caress = std::string("caress");
    EXPECT_EQ(PorterStemmer::stem(caress),"caress");
    std::string cats = std::string("cats");
    EXPECT_EQ(PorterStemmer::stem(cats),"cat");
}

TEST(porter_test, 1b)
{
    std::string feed = std::string("feed");
    EXPECT_EQ(PorterStemmer::stem(feed),"feed");
    std::string agreed = std::string("agreed");
    EXPECT_EQ(PorterStemmer::stem(agreed),"agre");//agree
    std::string plastered = std::string("plastered");
    EXPECT_EQ(PorterStemmer::stem(plastered),"plaster");
    std::string bled = std::string("bled");
    EXPECT_EQ(PorterStemmer::stem(bled),"bled");
    std::string motoring = std::string("motoring");
    EXPECT_EQ(PorterStemmer::stem(motoring),"motor");
    std::string sing = std::string("sing");
    EXPECT_EQ(PorterStemmer::stem(sing),"sing");
}

TEST(porter_test, 1bextra)
{
    std::string conflated = std::string("conflated");
    EXPECT_EQ(PorterStemmer::stem(conflated),"conflat");//conflate
    std::string troubled = std::string("troubled");
    EXPECT_EQ(PorterStemmer::stem(troubled),"troubl");//trouble
    std::string sized = std::string("sized");
    EXPECT_EQ(PorterStemmer::stem(sized),"size");
    std::string hopping = std::string("hopping");
    EXPECT_EQ(PorterStemmer::stem(hopping),"hop");
    std::string tanned = std::string("tanned");
    EXPECT_EQ(PorterStemmer::stem(tanned),"tan");
    std::string falling = std::string("falling");
    EXPECT_EQ(PorterStemmer::stem(falling),"fall");
    std::string hissing = std::string("hissing");
    EXPECT_EQ(PorterStemmer::stem(hissing),"hiss");
    std::string fizzed = std::string("fizzed");
    EXPECT_EQ(PorterStemmer::stem(fizzed),"fizz");
    std::string failing = std::string("failing");
    EXPECT_EQ(PorterStemmer::stem(failing),"fail");
    std::string filing = std::string("filing");
    EXPECT_EQ(PorterStemmer::stem(filing),"file");
}

TEST(porter_test, 1c)
{
    std::string happy = std::string("happy");
    EXPECT_EQ(PorterStemmer::stem(happy),"happi");
    std::string sky = std::string("sky");
    EXPECT_EQ(PorterStemmer::stem(sky),"sky");
}

TEST(porter_test, 2)
{
    std::string relational = std::string("relational");
    EXPECT_EQ(PorterStemmer::stem(relational),"relat");//relate
    std::string conditional = std::string("conditional");
    EXPECT_EQ(PorterStemmer::stem(conditional),"condit");//condition
    std::string rational = std::string("rational");
    EXPECT_EQ(PorterStemmer::stem(rational),"ration");//rational
    std::string valenci = std::string("valenci");
    EXPECT_EQ(PorterStemmer::stem(valenci),"valenc");//valence
    std::string hesitanci = std::string("hesitanci");
    EXPECT_EQ(PorterStemmer::stem(hesitanci),"hesit");//hesitance
    std::string digitizer = std::string("digitizer");
    EXPECT_EQ(PorterStemmer::stem(digitizer),"digit");//digitize
    std::string conformabli = std::string("conformabli");
    EXPECT_EQ(PorterStemmer::stem(conformabli),"conform");//conformable
    std::string radicalli = std::string("radicalli");
    EXPECT_EQ(PorterStemmer::stem(radicalli),"radic");//radical
    std::string differentli = std::string("differentli");
    EXPECT_EQ(PorterStemmer::stem(differentli),"differ");//different
    std::string vileli = std::string("vileli");
    EXPECT_EQ(PorterStemmer::stem(vileli),"vile");
    std::string analogousli = std::string("analogousli");
    EXPECT_EQ(PorterStemmer::stem(analogousli),"analog");//analogous
    std::string vietnamization = std::string("vietnamization");
    EXPECT_EQ(PorterStemmer::stem(vietnamization),"vietnam");//vietnam
    std::string predication = std::string("predication");
    EXPECT_EQ(PorterStemmer::stem(predication),"predic");//predicate
    std::string strOperator = std::string("operator");
    EXPECT_EQ(PorterStemmer::stem(strOperator),"oper");//operate
    std::string feudalism = std::string("feudalism");
    EXPECT_EQ(PorterStemmer::stem(feudalism),"feudal");
    std::string decisiveness = std::string("decisiveness");
    EXPECT_EQ(PorterStemmer::stem(decisiveness),"decis");//decisive
    std::string hopefulness = std::string("hopefulness");
    EXPECT_EQ(PorterStemmer::stem(hopefulness),"hope");//hopeful
    std::string callousness = std::string("callousness");
    EXPECT_EQ(PorterStemmer::stem(callousness),"callous");
    std::string formaliti = std::string("formaliti");
    EXPECT_EQ(PorterStemmer::stem(formaliti),"formal");
    std::string sensitiviti = std::string("sensitiviti");
    EXPECT_EQ(PorterStemmer::stem(sensitiviti),"sensit");//sensitive
    std::string sensibiliti = std::string("sensibiliti");
    EXPECT_EQ(PorterStemmer::stem(sensibiliti),"sensibl");//sensible
}

TEST(porter_test, 3)
{
    std::string triplicate = std::string("triplicate");
    EXPECT_EQ(PorterStemmer::stem(triplicate),"triplic");
    std::string formative = std::string("formative");
    EXPECT_EQ(PorterStemmer::stem(formative),"form");
    std::string formalize = std::string("formalize");
    EXPECT_EQ(PorterStemmer::stem(formalize),"formal");
    std::string electriciti = std::string("electriciti");
    EXPECT_EQ(PorterStemmer::stem(electriciti),"electr");//electric
    std::string electrical = std::string("electrical");
    EXPECT_EQ(PorterStemmer::stem(electrical),"electr");//electric
    std::string hopeful = std::string("hopeful");
    EXPECT_EQ(PorterStemmer::stem(hopeful),"hope");
    std::string goodness = std::string("goodness");
    EXPECT_EQ(PorterStemmer::stem(goodness),"good");
}

TEST(porter_test, 4)
{
    std::string revival = std::string("revival");
    EXPECT_EQ(PorterStemmer::stem(revival),"reviv");
    std::string allowance = std::string("allowance");
    EXPECT_EQ(PorterStemmer::stem(allowance),"allow");
    std::string inference = std::string("inference");
    EXPECT_EQ(PorterStemmer::stem(inference),"infer");
    std::string airliner = std::string("airliner");
    EXPECT_EQ(PorterStemmer::stem(airliner),"airlin");
    std::string gyroscopic = std::string("gyroscopic");
    EXPECT_EQ(PorterStemmer::stem(gyroscopic),"gyroscop");
    std::string adjustable = std::string("adjustable");
    EXPECT_EQ(PorterStemmer::stem(adjustable),"adjust");
    std::string defensible = std::string("defensible");
    EXPECT_EQ(PorterStemmer::stem(defensible),"defens");
    std::string irritant = std::string("irritant");
    EXPECT_EQ(PorterStemmer::stem(irritant),"irrit");
    std::string replacement = std::string("replacement");
    EXPECT_EQ(PorterStemmer::stem(replacement),"replac");
    std::string adjustment = std::string("adjustment");
    EXPECT_EQ(PorterStemmer::stem(adjustment),"adjust");
    std::string dependent = std::string("dependent");
    EXPECT_EQ(PorterStemmer::stem(dependent),"depend");
    std::string adoption = std::string("adoption");
    EXPECT_EQ(PorterStemmer::stem(adoption),"adopt");
    std::string homologou = std::string("homologou");
    EXPECT_EQ(PorterStemmer::stem(homologou),"homolog");
    std::string communism = std::string("communism");
    EXPECT_EQ(PorterStemmer::stem(communism),"commun");
    std::string activate = std::string("activate");
    EXPECT_EQ(PorterStemmer::stem(activate),"activ");
    std::string angulariti = std::string("angulariti");
    EXPECT_EQ(PorterStemmer::stem(angulariti),"angular");
    std::string homologous = std::string("homologous");
    EXPECT_EQ(PorterStemmer::stem(homologous),"homolog");
    std::string effective = std::string("effective");
    EXPECT_EQ(PorterStemmer::stem(effective),"effect");
    std::string bowdlerize = std::string("bowdlerize");
    EXPECT_EQ(PorterStemmer::stem(bowdlerize),"bowdler");
}

TEST(porter_test, 5a)
{
    std::string probate = std::string("probate");
    EXPECT_EQ(PorterStemmer::stem(probate),"probat");
    std::string rate = std::string("rate");
    EXPECT_EQ(PorterStemmer::stem(rate),"rate");
    std::string cease = std::string("cease");
    EXPECT_EQ(PorterStemmer::stem(cease),"ceas");
}

TEST(porter_test, 5b)
{
    std::string controll = std::string("controll");
    EXPECT_EQ(PorterStemmer::stem(controll),"control");
    std::string roll = std::string("roll");
    EXPECT_EQ(PorterStemmer::stem(roll),"roll");
}

TEST(porter_test, special)
{
    std::string argument = std::string("argument");
    EXPECT_EQ(PorterStemmer::stem(argument),"argument");
    std::string nationality = std::string("nationality");
    EXPECT_EQ(PorterStemmer::stem(nationality),"nation");
    std::string adoption = std::string("adoption");
    EXPECT_EQ(PorterStemmer::stem(adoption),"adopt");
    std::string controller = std::string("controller");
    EXPECT_EQ(PorterStemmer::stem(controller),"control");
    std::string roll = std::string("roll");
    EXPECT_EQ(PorterStemmer::stem(roll),"roll");
    std::string radically = std::string("radically");
    EXPECT_EQ(PorterStemmer::stem(radically),"radic");
}

