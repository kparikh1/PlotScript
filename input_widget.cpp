//
// Created by kishanp on 10/27/18.
//

#include "input_widget.hpp"
#include "interpreter.hpp"
#include "semantic_error.hpp"
#include <QDebug>

void InputWidget::keyPressEvent(QKeyEvent *event) {

  if ((event->key() == Qt::Key_Enter || event->key() == Qt::Key_Return) && event->modifiers() == Qt::ShiftModifier) {

    std::string exception;

    Interpreter interp;

    std::istringstream input(toPlainText().toStdString());

    if (!interp.parseStream(input)) {
      emit exceptionThrown("Error: Invalid Program. Could not parse.");
      return;
    }

    try {
      Expression result = interp.evaluate();
      emit sendResult(result);
    } catch (const SemanticError &error) {
      emit exceptionThrown(error.what());
    }
  } else {
    QPlainTextEdit::keyPressEvent(event);
  }

}
