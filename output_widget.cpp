//
// Created by kishanp on 10/27/18.
//

#include "output_widget.hpp"
#include <QVBoxLayout>
#include <QString>
#include <QtWidgets/QGraphicsTextItem>

OutputWidget::OutputWidget() {

  view = new QGraphicsView();
  scene = new QGraphicsScene();

  view->setScene(scene);

  auto layout = new QVBoxLayout();
  layout->addWidget(view);

  setLayout(layout);
}
void OutputWidget::printText(const std::string &text) {

  scene->clear();
  QGraphicsTextItem *textItem = scene->addText(QString::fromStdString(text));
  textItem->setPos(0, 0);
}
void OutputWidget::outputExpression(const Expression &result) {

  if (result.isLambda())
    return;
  else if (result.isPoint()) {
    if (result.getProperty("size").isHeadNumber()) {
      scene->clear();
      QGraphicsEllipseItem *point = scene->addEllipse((result.getTail().cbegin())->head().asNumber(),
                                                      (result.getTail().cbegin() + 1)->head().asNumber(),
                                                      result.getProperty("size").head().asNumber(),
                                                      result.getProperty("size").head().asNumber(),
                                                      QPen(Qt::black),
                                                      QBrush(Qt::black, Qt::SolidPattern));
      point->setVisible(true);
    } else {
      printText("Error: make-point size not a positive number");
    }
  } else if (result.isLine()) {
    if (result.getProperty("thickness").isHeadNumber()) {
      scene->clear();
      QGraphicsLineItem *line = scene->addLine((result.getTail().cbegin())->getTail().cbegin()->head().asNumber(),
                                               (result.getTail().cbegin()->getTail().cbegin() + 1)->head().asNumber(),
                                               ((result.getTail().cbegin() + 1)->getTail().cbegin())->head().asNumber(),
                                               ((result.getTail().cbegin() + 1)->getTail().cbegin()
                                                   + 1)->head().asNumber(),
                                               QPen(QBrush(Qt::black, Qt::SolidPattern),
                                                    result.getProperty("thickness").head().asNumber()));
      line->setVisible(true);
    } else {
      printText(("Error: make-line thickness not a number"));
    }
  } else {
    std::stringstream iss;
    iss << result;
    scene->clear();
    QGraphicsTextItem *textItem = scene->addText(QString::fromStdString(iss.str()));
    textItem->setPos(0, 0);
  }

}
