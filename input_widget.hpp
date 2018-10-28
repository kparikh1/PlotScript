//
// Created by kishanp on 10/27/18.
//

#ifndef PLOTSCRIPT_INPUT_WIDGET_HPP
#define PLOTSCRIPT_INPUT_WIDGET_HPP
#include <QPlainTextEdit>

class InputWidget : public QPlainTextEdit {
 Q_OBJECT
 public:
  InputWidget() = default;

  void keyPressEvent(QKeyEvent *event) override;

 private:
};

#endif //PLOTSCRIPT_INPUT_WIDGET_HPP
