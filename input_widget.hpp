//
// Created by kishanp on 10/27/18.
//

#ifndef PLOTSCRIPT_INPUT_WIDGET_HPP
#define PLOTSCRIPT_INPUT_WIDGET_HPP
#include <QPlainTextEdit>
#include "expression.hpp"
#include <string>

class InputWidget : public QPlainTextEdit {
 Q_OBJECT
 public:
  InputWidget() = default;

  void keyPressEvent(QKeyEvent *event) override;

 signals:

  void exceptionThrown(const std::string &exception);
  void sendResult(const Expression &result);

 private:
};

#endif //PLOTSCRIPT_INPUT_WIDGET_HPP
