//
// Created by kishanp on 10/27/18.
//

#ifndef PLOTSCRIPT_NOTEBOOK_APP_HPP
#define PLOTSCRIPT_NOTEBOOK_APP_HPP
#include <QWidget>
#include <QVBoxLayout>
#include "input_widget.hpp"
#include "output_widget.hpp"

class notebook_app : public QWidget {
 Q_OBJECT
 public:
  notebook_app();

 private:
  input_widget *input;
  output_widget *output;
};

#endif //PLOTSCRIPT_NOTEBOOK_APP_HPP
