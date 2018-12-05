//
// Created by kishanp on 10/27/18.
//

#ifndef PLOTSCRIPT_NOTEBOOK_APP_HPP
#define PLOTSCRIPT_NOTEBOOK_APP_HPP
#include <QWidget>
#include <QVBoxLayout>
#include "input_widget.hpp"
#include "output_widget.hpp"
#include "ButtonWidget.hpp"

class NotebookApp : public QWidget {
 Q_OBJECT
 public:
  NotebookApp();

 private:
  InputWidget *input;
  OutputWidget *output;

  ButtonWidget *startBtn;
  ButtonWidget *stopBtn;
  ButtonWidget *resetBtn;
  ButtonWidget *interruptBtn;
};

#endif //PLOTSCRIPT_NOTEBOOK_APP_HPP
