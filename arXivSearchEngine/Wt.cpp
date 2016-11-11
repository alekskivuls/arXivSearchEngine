#include <Wt/WApplication>
#include <Wt/WBreak>
#include <Wt/WContainerWidget>
#include <Wt/WLineEdit>
#include <Wt/WPushButton>
#include <Wt/WText>
#include <Wt/WTextArea>
#include <Wt/WLink>
#include <Wt/WLabel>

#include <boost/spirit/home/classic/phoenix/composite.hpp>

#include "Engine.h"

using namespace Wt;

/*
 * A simple application to interact and display arXiv search engine results
 */
class WTApplication : public WApplication
{
public:
  WTApplication(const WEnvironment& env);

private:
  Engine engine;

  WLineEdit *searchBox_;
  WLineEdit *indexPath_;
  WLabel *numResults_;
  WTextArea *result_;

  void index();
  void stem();
  void search();
};

/*
 * The env argument contains information about the new session, and
 * the initial request. It must be passed to the WApplication
 * constructor so it is typically also an argument for your custom
 * application constructor.
*/
WTApplication::WTApplication(const WEnvironment& env)
  : WApplication(env)
{
  setTitle("arXivSearchEngine");                         // application title

  root()->addWidget(new WText("Filepath: "));
  indexPath_ = new WLineEdit(root());
  indexPath_->setTextSize(100);
  indexPath_->setFocus();
  WPushButton *indexButton
    = new WPushButton("Index", root());                   // create a button
  indexButton->setMargin(5, Left);                            // add 5 pixels margin
  root()->addWidget(new WBreak());

  root()->addWidget(new WText("Query: "));               // show some text
  searchBox_ = new WLineEdit(root());                    // allow text input

  WPushButton *stemButton
    = new WPushButton("Stem", root());                   // create a button
  stemButton->setMargin(5, Left);                            // add 5 pixels margin

  WPushButton *searchButton
    = new WPushButton("Search", root());                   // create a button
  searchButton->setMargin(5);                            // add 5 pixels margin
  root()->addWidget(new WText("Num Results: "));
  numResults_ = new WLabel(root());

  root()->addWidget(new WBreak());                       // insert a line break

  result_ = new WTextArea(root());                       // empty text
  result_->setColumns(200);
  result_->setRows(50);

  /*
   * Connect signals with slots
   *
   * - simple Wt-way
   */
  indexButton->clicked().connect(this, &WTApplication::index);
  stemButton->clicked().connect(this, &WTApplication::stem);
  searchButton->clicked().connect(this, &WTApplication::search);

  /*
   * - using an arbitrary function object (binding values with boost::bind())
   */
  searchBox_->enterPressed().connect
    (boost::bind(&WTApplication::stem, this));
}

void WTApplication::index()
{
  /*
   * Update the text, using text input into the stemResult_ field.
   */
  engine.index(indexPath_->text().toUTF8());
  result_->setText("Indexing completed");
  //std::string vocabSize = std::to_string(engine.getVocab().size());
  numResults_->setText(std::to_string(engine.getVocab().size()));
}


void WTApplication::stem()
{
  /*
   * Update the text, using text input into the stemResult_ field.
   */
  std::string textString = searchBox_->text().toUTF8();
  result_->setText("Stemmed: " + engine.stem(textString));
}

void WTApplication::search()
{
  /*
   * Update the text, using text input into the stemResult_ field.
   */
  //std::string vocabList;
  //for(auto term : engine.getVocab())
  //  vocabList += term + "\n";
  std::string query = searchBox_->text().toUTF8();
  std::vector<std::string> output = engine.getQuery(query);

  std::string queryResult = "";
  for (auto di : output)
      queryResult += di + '\t';
  queryResult += '\n' + output.size() + '\n';
  result_->setText("Results: \n" + queryResult);
  numResults_->setText(std::to_string(output.size()));
}

WApplication *createApplication(const WEnvironment& env)
{
  /*
   * You could read information from the environment to decide whether
   * the user has permission to start a new application
   */
  return new WTApplication(env);
}

int main(int argc, char **argv)
{
  /*
   * Your main method may set up some shared resources, but should then
   * start the server application (FastCGI or httpd) that starts listening
   * for requests, and handles all of the application life cycles.
   *
   * The last argument to WRun specifies the function that will instantiate
   * new application objects. That function is executed when a new user surfs
   * to the Wt application, and after the library has negotiated browser
   * support. The function should return a newly instantiated application
   * object.
   */
  return WRun(argc, argv, &createApplication);
}
