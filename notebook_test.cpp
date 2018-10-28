#include <QTest>
#include "notebook_app.hpp"

class NotebookTest : public QObject {
 Q_OBJECT

 private slots:

  void initTestCase();

  // TODO: implement additional tests here
  void objectNames();

 private:
  InputWidget input;
  OutputWidget output;
  NotebookApp notebook;
};

void NotebookTest::initTestCase() {

}
void NotebookTest::objectNames() {
  auto inp = notebook.findChild<InputWidget *>("input");
  auto outp = notebook.findChild<OutputWidget *>("output");

  QVERIFY2(inp, "Could not find widget with name: 'input'");
  QVERIFY2(outp, "Could not find widget with name: 'output'");
}

QTEST_MAIN(NotebookTest)
#include "notebook_test.moc"
