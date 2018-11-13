//
// Created by kishanp on 10/27/18.
//

#ifndef PLOTSCRIPT_OUTPUT_WIDGET_HPP
#define PLOTSCRIPT_OUTPUT_WIDGET_HPP
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QVBoxLayout>
#include <string>
#include "expression.hpp"

class OutputWidget : public QWidget {

 public:
  OutputWidget();

  void resizeEvent(QResizeEvent *event) override;

 private:
  QGraphicsScene *scene;
  QGraphicsView *view;

  bool showExpression(const Expression &result);
  bool isGraphic(const Expression &input);

 public slots:
  void printText(const std::string &text);
  void outputExpression(const Expression &result);
};

#endif //PLOTSCRIPT_OUTPUT_WIDGET_HPP
