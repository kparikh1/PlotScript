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
  std::stringstream iss;
  iss << result;
  scene->clear();
  QGraphicsTextItem *textItem = scene->addText(QString::fromStdString(iss.str()));
  textItem->setPos(0, 0);

}
