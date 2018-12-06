//
// Created by kishanp on 10/27/18.
//

#include "input_widget.hpp"
#include "interpreter.hpp"
#include "semantic_error.hpp"
#include "Consumer.hpp"

#include <QDebug>
#include <thread>

void InputWidget::keyPressEvent(QKeyEvent *event) {

  if ((event->key() == Qt::Key_Enter || event->key() == Qt::Key_Return) && event->modifiers() == Qt::ShiftModifier) {

    std::string line = toPlainText().toStdString();

    if (line == "%exit")
      exit(EXIT_SUCCESS);
    else if (line == "%reset") {
      Expression temp;
      if (th1.joinable()) {
        in.push("%stop");
        out.wait_and_pop(temp);
        th1.join();
      }
      std::thread th2(&Consumer::run, worker);
      std::swap(th1, th2);
    } else if (!th1.joinable() && (line == "%start")) {
      std::thread th2(&Consumer::run, worker);
      std::swap(th1, th2);
    } else if (th1.joinable() && (line != "%start")) {
      in.push(line);
      setReadOnly(true);
      Expression result;
      out.wait_and_pop(result);
      setReadOnly(false);
      if (result.head().isString() && (result.head().asString() == "Threading Command") && th1.joinable()) {
        th1.join();
      } else if (result.head().isError()) {
        emit exceptionThrown(result.head().asError());
      } else {
        emit sendResult(result);
      }//else
      //timer.start(40);
    } else if (!th1.joinable()) {
      emit exceptionThrown("Error: interpreter kernel not running");
    }
  } else {
    QPlainTextEdit::keyPressEvent(event);
  }
}

void InputWidget::popResult() {

  Expression result;
  if (out.try_pop(result)) {
    setReadOnly(false);
    timer.stop();
    if (result.head().isString() && (result.head().asString() == "Threading Command") && th1.joinable()) {
      th1.join();
    } else if (result.head().isError()) {
      emit exceptionThrown(result.head().asError());
    } else {
      emit sendResult(result);
    }
  }
}

void InputWidget::init() {
  connect(&timer, &QTimer::timeout, this, &InputWidget::popResult);

  worker = new Consumer(&in, &out, id);

  std::thread th2(&Consumer::run, worker);

  std::swap(th1, th2);
}
void InputWidget::startInterpreter() {

  if (!th1.joinable()) {
    std::thread th2(&Consumer::run, worker);
    std::swap(th1, th2);
  }
}

void InputWidget::stopInterpreter() {
  if (th1.joinable()) {
    Expression temp;
    in.push("%stop");
    out.wait_and_pop(temp);
    th1.join();
  }
}

void InputWidget::resetInterpreter() {
  Expression temp;
  if (th1.joinable()) {
    in.push("%stop");
    out.wait_and_pop(temp);
    th1.join();
  }
  std::thread th2(&Consumer::run, worker);
  std::swap(th1, th2);
}

void InputWidget::interruptInterpreter() {
  interrupt = true;
}

InputWidget::~InputWidget() {
  if (th1.joinable()) {
    Expression temp;
    in.push("%stop");
    out.wait_and_pop(temp);
    th1.join();
  }
}
