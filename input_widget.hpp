//
// Created by kishanp on 10/27/18.
//

#ifndef PLOTSCRIPT_INPUT_WIDGET_HPP
#define PLOTSCRIPT_INPUT_WIDGET_HPP
#include <QPlainTextEdit>
#include "expression.hpp"
#include "Consumer.hpp"
#include <string>
#include <thread>
#include <QtCore/QTimer>

class InputWidget : public QPlainTextEdit {
 Q_OBJECT
 public:
  InputWidget() = default;
  ~InputWidget();

  void init();

  void keyPressEvent(QKeyEvent *event) override;

 signals:

  void exceptionThrown(const std::string &exception);
  void sendResult(const Expression &result);

 public slots:

  void popResult();
  void startInterpreter();
  void stopInterpreter();
  void resetInterpreter();
  void interruptInterpreter();

 private:
  std::size_t id = 1;
  MessageQueue<std::string> in;
  MessageQueue<Expression> out;
  Consumer *worker;
  std::thread th1;

  QTimer timer;
};

#endif //PLOTSCRIPT_INPUT_WIDGET_HPP
