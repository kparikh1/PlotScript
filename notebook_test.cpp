#include <QTest>
#include <QtWidgets/QGraphicsTextItem>
#include "notebook_app.hpp"

class NotebookTest : public QObject {
Q_OBJECT

private slots:

  void initTestCase();

  // TODO: implement additional tests here
  void objectNames();
  void testSimpleExpression();
  void testBasicPoint();

private:
  InputWidget *input;
  OutputWidget *output;
  QGraphicsView *view;
  QGraphicsScene *scene;
  NotebookApp notebook;
};

void NotebookTest::initTestCase() {

  input = notebook.findChild<InputWidget *>("input");
  output = notebook.findChild<OutputWidget *>("output");
  view = output->findChild<QGraphicsView *>();
  scene = view->scene();
}

void NotebookTest::objectNames() {

  QVERIFY2(input, "Could not find widget with name: 'input'");
  QVERIFY2(output, "Could not find widget with name: 'output'");
  QVERIFY2(view, "Could not find view");
  QVERIFY2(scene, "Could not find scene");
}

void NotebookTest::testSimpleExpression() {

  std::string inputText = "(* 2 3)";
  input->setPlainText(QString::fromStdString(inputText));

  QVERIFY2(input->toPlainText() == QString::fromStdString(inputText), "Text not set");

  QKeyEvent *event = new QKeyEvent(QEvent::KeyPress, Qt::Key_Enter, Qt::ShiftModifier);
  input->keyPressEvent(event);

  auto graphicsObjects = scene->items();

  auto *result = (QGraphicsTextItem *) *graphicsObjects.cbegin();

  QVERIFY2(result->toPlainText() == QString("(6)"), "(* 2 3) did not evaluate to (6)");
}

void NotebookTest::testBasicPoint() {

  std::string inputText = "(make-point 1 2)";
  input->setPlainText(QString::fromStdString(inputText));

  QKeyEvent *event = new QKeyEvent(QEvent::KeyPress, Qt::Key_Enter, Qt::ShiftModifier);
  input->keyPressEvent(event);

  auto graphicsObjects = scene->items();

  auto *result = (QGraphicsEllipseItem *) *graphicsObjects.cbegin();

}

QTEST_MAIN(NotebookTest)
#include "notebook_test.moc"

