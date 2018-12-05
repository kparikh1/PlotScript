#include <QTest>
#include <QtWidgets/QGraphicsTextItem>
#include "notebook_app.hpp"
#include <QDebug>

volatile std::atomic_bool interrupt(false);
/// Helper Functions
int findLines(QGraphicsScene *scene, QRectF bbox, qreal margin) {

  QPainterPath selectPath;

  QMarginsF margins(margin, margin, margin, margin);
  selectPath.addRect(bbox.marginsAdded(margins));
  scene->setSelectionArea(selectPath, Qt::ContainsItemShape);

  int numlines(0);
      foreach(auto item, scene->selectedItems()) {
      if (item->type() == QGraphicsLineItem::Type) {
        numlines += 1;
      }
    }

  return numlines;
}

/*
findPoints - find points in a scene contained within a specified rectangle
 */
int findPoints(QGraphicsScene *scene, QPointF center, qreal radius) {

  QPainterPath selectPath;
  selectPath.addRect(QRectF(center.x() - radius, center.y() - radius, 2 * radius, 2 * radius));
  scene->setSelectionArea(selectPath, Qt::ContainsItemShape);

  int numpoints(0);
      foreach(auto item, scene->selectedItems()) {
      if (item->type() == QGraphicsEllipseItem::Type) {
        numpoints += 1;
      }
    }

  return numpoints;
}

/*
findText - find text in a scene centered at a specified point with a given
           rotation and string contents
 */
int findText(QGraphicsScene *scene, QPointF center, qreal rotation, QString contents) {

  int numtext(0);
      foreach(auto item, scene->items(center)) {
      if (item->type() == QGraphicsTextItem::Type) {
        QGraphicsTextItem *text = static_cast<QGraphicsTextItem *>(item);
        if ((text->toPlainText() == contents) &&
            (text->rotation() == rotation) &&
            (text->pos() + text->boundingRect().center() == center)) {
          numtext += 1;
        }
      }
    }

  return numtext;
}

/*
intersectsLine - find lines in a scene that intersect a specified rectangle
 */
int intersectsLine(QGraphicsScene *scene, QPointF center, qreal radius) {

  QPainterPath selectPath;
  selectPath.addRect(QRectF(center.x() - radius, center.y() - radius, 2 * radius, 2 * radius));
  scene->setSelectionArea(selectPath, Qt::IntersectsItemShape);

  int numlines(0);
      foreach(auto item, scene->selectedItems()) {
      if (item->type() == QGraphicsLineItem::Type) {
        numlines += 1;
      }
    }

  return numlines;
}

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
  void testDefaultLine();
  void testDiscretePlotLayout();
  void testContinuousPlotLayout();
  void testContinuousSinPlot();

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
  std::string inputText = "(set-property \"position\" (make-point 2 4) (make-text \"Hello World\" ) )";
  input->setPlainText(QString::fromStdString(inputText));

  QKeyEvent *event = new QKeyEvent(QEvent::KeyPress, Qt::Key_Enter, Qt::ShiftModifier);
  input->keyPressEvent(event);

  auto graphicsObjects = scene->items();

  auto result = (QGraphicsTextItem *) *graphicsObjects.cbegin();

  //std::string error = "Invalid point" + std::to_string(result->pos().toPoint().x()) + ", " + std::to_string(result->pos().toPoint().y());
  //qDebug() << QString::fromStdString(error);
  /// This test is computer dependent
  //QVERIFY2(result->pos().toPoint() == QPoint(-33, -8), "Invalid Point");
  QVERIFY2(result->toPlainText() == QString(QString::fromStdString("Hello World")), "Invalid text outputted");
}

void NotebookTest::testDefaultLine() {
  std::string inputText = "(make-line (make-point 0 0) (make-point 3 2))";
  input->setPlainText(QString::fromStdString(inputText));

  QKeyEvent *event = new QKeyEvent(QEvent::KeyPress, Qt::Key_Enter, Qt::ShiftModifier);
  input->keyPressEvent(event);

  auto graphicsObjects = scene->items();

  auto result = (QGraphicsLineItem *) *graphicsObjects.cbegin();
  QPen pen(QBrush(Qt::black, Qt::SolidPattern), 1);

  QVERIFY2(result->line().toLine() == QLine(0, 0, 3, 2), "Invalid Line");
  QVERIFY2(result->pen() == pen, "Invalid line thickness");

}

void NotebookTest::testDiscretePlotLayout() {

  std::string program = R"(
(discrete-plot (list (list -1 -1) (list 1 1))
    (list (list "title" "The Title")
          (list "abscissa-label" "X Label")
          (list "ordinate-label" "Y Label") ))
)";

  input->setPlainText(QString::fromStdString(program));
  QTest::keyClick(input, Qt::Key_Return, Qt::ShiftModifier);

  auto view = output->findChild<QGraphicsView *>();
  QVERIFY2(view, "Could not find QGraphicsView as child of OutputWidget");

  auto scene = view->scene();

  // first check total number of items
  // 8 lines + 2 points + 7 text = 17
  auto items = scene->items();
  QCOMPARE(items.size(), 17);

  // make them all selectable
      foreach(auto item, items) {
      item->setFlag(QGraphicsItem::ItemIsSelectable);
    }

  double scalex = 20.0 / 2.0;
  double scaley = 20.0 / 2.0;

  double xmin = scalex * -1;
  double xmax = scalex * 1;
  double ymin = scaley * -1;
  double ymax = scaley * 1;
  double xmiddle = (xmax + xmin) / 2;
  double ymiddle = (ymax + ymin) / 2;

  // check title
  QCOMPARE(findText(scene, QPointF(xmiddle, -(ymax + 3)), 0, QString("The Title")), 1);

  // check abscissa label
  QCOMPARE(findText(scene, QPointF(xmiddle, -(ymin - 3)), 0, QString("X Label")), 1);

  // check ordinate label
  QCOMPARE(findText(scene, QPointF(xmin - 3, -ymiddle), -90, QString("Y Label")), 1);

  // check abscissa min label
  QCOMPARE(findText(scene, QPointF(xmin, -(ymin - 2)), 0, QString("-1")), 1);

  // check abscissa max label
  QCOMPARE(findText(scene, QPointF(xmax, -(ymin - 2)), 0, QString("1")), 1);

  // check ordinate min label
  QCOMPARE(findText(scene, QPointF(xmin - 2, -ymin), 0, QString("-1")), 1);

  // check ordinate max label
  QCOMPARE(findText(scene, QPointF(xmin - 2, -ymax), 0, QString("1")), 1);

  // check the bounding box bottom
  QCOMPARE(findLines(scene, QRectF(xmin, -ymin, 20, 0), 0.1), 1);

  // check the bounding box top
  QCOMPARE(findLines(scene, QRectF(xmin, -ymax, 20, 0), 0.1), 1);

  // check the bounding box left and (-1, -1) stem
  QCOMPARE(findLines(scene, QRectF(xmin, -ymax, 0, 20), 0.1), 2);

  // check the bounding box right and (1, 1) stem
  QCOMPARE(findLines(scene, QRectF(xmax, -ymax, 0, 20), 0.1), 2);

  // check the abscissa axis
  QCOMPARE(findLines(scene, QRectF(xmin, 0, 20, 0), 0.1), 1);

  // check the ordinate axis
  QCOMPARE(findLines(scene, QRectF(0, -ymax, 0, 20), 0.1), 1);

  // check the point at (-1,-1)
  QCOMPARE(findPoints(scene, QPointF(-10, 10), 0.6), 1);

  // check the point at (1,1)
  QCOMPARE(findPoints(scene, QPointF(10, -10), 0.6), 1);
}

void NotebookTest::testContinuousPlotLayout() {
  std::string program = R"(
(begin
    (define f (lambda (x)
        (+ (* 2 x) 1)))
    (continuous-plot f (list -2 2)
        (list
        (list "title" "A continuous linear function")
        (list "abscissa-label" "x")
        (list "ordinate-label" "y"))))
)";
  input->setPlainText(QString::fromStdString(program));
  QTest::keyClick(input, Qt::Key_Return, Qt::ShiftModifier);

  auto view = output->findChild<QGraphicsView *>();
  QVERIFY2(view, "Could not find QGraphicsView as child of OutputWidget");

  auto scene = view->scene();

  auto items = scene->items();
  QCOMPARE(items.size(), 63);

}

void NotebookTest::testContinuousSinPlot() {
  std::string program = R"(
(begin (define f (lambda (x) (sin x))) (continuous-plot f (list (- pi) pi)))
)";
  input->setPlainText(QString::fromStdString(program));
  QTest::keyClick(input, Qt::Key_Return, Qt::ShiftModifier);

  auto view = output->findChild<QGraphicsView *>();
  QVERIFY2(view, "Could not find QGraphicsView as child of OutputWidget");

  auto scene = view->scene();

  auto items = scene->items();
  QCOMPARE(items.size(), 82);

}

QTEST_MAIN(NotebookTest)
#include "notebook_test.moc"

