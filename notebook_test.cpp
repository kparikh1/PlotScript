#include <QTest>
#include <QtWidgets/QGraphicsTextItem>
#include "notebook_app.hpp"

class NotebookTest : public QObject {
 Q_OBJECT

 private slots:

  void initTestCase();

  // TODO: implement additional tests here
  void objectNames();
  void testTextOutput();
  void testBasicPoint();
  void testBasicLine();
  void testBasicText();

 private:
  InputWidget *input;
  OutputWidget *output;
  QGraphicsView *view;
  QGraphicsScene *scene;
  NotebookApp *notebook;
};

void NotebookTest::initTestCase() {

  notebook = new NotebookApp();
  input = notebook->findChild<InputWidget *>("input");
  output = notebook->findChild<OutputWidget *>("output");
  view = output->findChild<QGraphicsView *>();
  scene = view->scene();
}

void NotebookTest::objectNames() {

  QVERIFY2(input, "Could not find widget with name: 'input'");
  QVERIFY2(output, "Could not find widget with name: 'output'");
  QVERIFY2(view, "Could not find view");
  QVERIFY2(scene, "Could not find scene");
}

void NotebookTest::testTextOutput() {

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

  std::string inputText = "(set-property \"size\" 200 (make-point 1 2))";
  input->setPlainText(QString::fromStdString(inputText));

  QKeyEvent *event = new QKeyEvent(QEvent::KeyPress, Qt::Key_Enter, Qt::ShiftModifier);
  input->keyPressEvent(event);

  auto graphicsObjects = scene->items();

  auto *result = (QGraphicsEllipseItem *) *graphicsObjects.cbegin();

  QVERIFY2(result->rect().center().toPoint() == QPoint(1, 2), "Invalid point");
  QVERIFY2(result->rect().size().toSize() == QSize(200, 200), "Invalid point size");

}

void NotebookTest::testBasicLine() {
  std::string inputText = "(set-property \"thickness\" 2 (make-line (make-point 0 2) (make-point 2 0)))";
  input->setPlainText(QString::fromStdString(inputText));

  QKeyEvent *event = new QKeyEvent(QEvent::KeyPress, Qt::Key_Enter, Qt::ShiftModifier);
  input->keyPressEvent(event);

  auto graphicsObjects = scene->items();

  auto *result = (QGraphicsLineItem *) *graphicsObjects.cbegin();
  QPen pen(QBrush(Qt::black, Qt::SolidPattern), 2);

  QVERIFY2(result->line().toLine() == QLine(0, 2, 2, 0), "Invalid Line");
  QVERIFY2(result->pen() == pen, "Invalid line thickness");
}

void NotebookTest::testBasicText() {
  std::string inputText = "(set-property \"position\" (make-point 2 4) (make-text \"Hi\" ) )";
  input->setPlainText(QString::fromStdString(inputText));

  QKeyEvent *event = new QKeyEvent(QEvent::KeyPress, Qt::Key_Enter, Qt::ShiftModifier);
  input->keyPressEvent(event);

  auto graphicsObjects = scene->items();

  auto result = (QGraphicsTextItem *) *graphicsObjects.cbegin();

  QVERIFY2(result->pos().toPoint() == QPoint(2, 4), "Invalid point");
  QVERIFY2(result->toPlainText() == QString(QString::fromStdString("Hi")), "Invalid text outputted");
}

QTEST_MAIN(NotebookTest)
#include "notebook_test.moc"

