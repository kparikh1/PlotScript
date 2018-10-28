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

  scene->clear();
  if (result.isList() && !result.isPoint() && !result.isLine()) {
    for (auto &item:result.getTail()) {
      if (!showExpression(item)) {
        return;
      }
    }
  } else {
    showExpression(result);
  }

}
bool OutputWidget::showExpression(const Expression &result) {
  if (result.isLambda())
    return false;
  else if (result.isPoint()) {
    if (result.getProperty("size").isHeadNumber()) {
      QGraphicsEllipseItem *point = scene->addEllipse((result.getTail().cbegin())->head().asNumber(),
                                                      (result.getTail().cbegin() + 1)->head().asNumber(),
                                                      result.getProperty("size").head().asNumber(),
                                                      result.getProperty("size").head().asNumber(),
                                                      QPen(Qt::black),
                                                      QBrush(Qt::black, Qt::SolidPattern));
      point->setVisible(true);
    } else {
      printText("Error: make-point size not a positive number");
      return false;
    }
  } else if (result.isLine()) {
    if (result.getProperty("thickness").isHeadNumber()) {
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
      return false;
    }
  } else if (result.isText()) {
    if (result.getProperty("position").isPoint()) {
      QGraphicsTextItem *textItem = scene->addText(QString::fromStdString(result.head().asString()));
      textItem->setPos(result.getProperty("position").getTail().cbegin()->head().asNumber(),
                       (result.getProperty("position").getTail().cbegin() + 1)->head().asNumber());
    } else {
      printText("Error: make-text position not a point");
      return false;
    }

  } else {
    std::stringstream iss;
    iss << result;
    QGraphicsTextItem *textItem = scene->addText(QString::fromStdString(iss.str()));
    textItem->setPos(0, 0);
  }
  return true;
}
