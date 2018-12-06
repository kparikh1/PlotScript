//
// Created by kishanp on 10/27/18.
//

#ifndef PLOTSCRIPT_NOTEBOOK_APP_HPP
#define PLOTSCRIPT_NOTEBOOK_APP_HPP
#include <QWidget>
#include <QVBoxLayout>
#include <QtWidgets/QPushButton>

#include "input_widget.hpp"
#include "output_widget.hpp"

class NotebookApp : public QWidget {
 Q_OBJECT
 public:
  NotebookApp();

 private:
  InputWidget *input;
  OutputWidget *output;

  QPushButton *startBtn;
  QPushButton *stopBtn;
  QPushButton *resetBtn;
  QPushButton *interruptBtn;
};

#endif //PLOTSCRIPT_NOTEBOOK_APP_HPP
