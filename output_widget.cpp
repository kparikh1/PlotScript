//
// Created by kishanp on 10/27/18.
//

#include "output_widget.hpp"
#include <QVBoxLayout>
#include <QString>
#include <QtWidgets/QGraphicsTextItem>
#include <QtGui/QResizeEvent>

OutputWidget::OutputWidget() {

  view = new QGraphicsView();
  scene = new QGraphicsScene();

  view->setScene(scene);

  auto layout = new QVBoxLayout();
  layout->addWidget(view);

  setLayout(layout);
}
void OutputWidget::printText(const std::string &text) {

  std::string newText = text;

  if (text == "Invalid Expression. Could not parse.") {
    newText = "Error: Invalid Expression. Could not parse.";
  }

  scene->clear();
  QGraphicsTextItem *textItem = scene->addText(QString::fromStdString(newText));
  textItem->setPos(0, 0);
  view->fitInView(0, 0, scene->width(), scene->height(), Qt::KeepAspectRatio);

}
void OutputWidget::outputExpression(const Expression &result) {

  scene->clear();
  if (result.isList() && isGraphic(result) && !result.isLine()) {
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
      double offset = result.getProperty("size").head().asNumber() / 2;
      QGraphicsEllipseItem *point = scene->addEllipse((result.getTail().cbegin())->head().asNumber() - offset,
                                                      (result.getTail().cbegin() + 1)->head().asNumber() - offset,
                                                      result.getProperty("size").head().asNumber(),
                                                      result.getProperty("size").head().asNumber(),
                                                      QPen(Qt::NoPen),
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
      textItem->setPos(
          result.getProperty("position").getTail().cbegin()->head().asNumber() - textItem->boundingRect().width() / 2,
          (result.getProperty("position").getTail().cbegin() + 1)->head().asNumber()
              - textItem->boundingRect().height() / 2);
      textItem->setTransformOriginPoint(textItem->boundingRect().width() / 2, textItem->boundingRect().height() / 2);
      if (result.getProperty("text-scale").isHeadNumber())
        textItem->setScale(result.getProperty("text-scale").head().asNumber());
      if (result.getProperty("text-rotation").isHeadNumber())
        textItem->setRotation(result.getProperty("text-rotation").head().asNumber() * 180 / std::atan2(0, -1));
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

  view->fitInView(0, 0, scene->width(), scene->height(), Qt::KeepAspectRatio);
  return true;
}

bool OutputWidget::isGraphic(const Expression &input) {
  for (auto &item:input.getTail()) {
    if (item.isPoint() || item.isLine() || item.isText())
      return true;
  }
  return false;
}

void OutputWidget::resizeEvent(QResizeEvent *event) {
  QWidget::resizeEvent(event);

  view->fitInView(0, 0, scene->width(), scene->height(), Qt::KeepAspectRatio);
}

