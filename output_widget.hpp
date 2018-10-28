//
// Created by kishanp on 10/27/18.
//

#ifndef PLOTSCRIPT_OUTPUT_WIDGET_HPP
#define PLOTSCRIPT_OUTPUT_WIDGET_HPP
#include <QGraphicsView>
#include <QGraphicsScene>

class OutputWidget : public QWidget {

 public:
  OutputWidget();

 private:
  QGraphicsScene *scene;
  QGraphicsView *view;

};

#endif //PLOTSCRIPT_OUTPUT_WIDGET_HPP
