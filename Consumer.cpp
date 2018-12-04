//
// Created by kishanp on 12/3/18.
//

#include <iostream>
#include "Consumer.hpp"
#include "interpreter.hpp"
#include "semantic_error.hpp"

void Consumer::run(bool ContinueRun) {

  std::istringstream message;
  Interpreter interp;

  while (ContinueRun) {
    incomingMB->wait_and_pop(message);
    ContinueRun = (message.str() != "%stop");

    if (!interp.parseStream(message)) {
      outgoingMB->push(Expression("Invalid Expression. Could not parse.", false));
    } else {
      try {
        Expression exp = interp.evaluate();
        outgoingMB->push(exp);
      }
      catch (const SemanticError &ex) {
        outgoingMB->push(Expression(ex.what(), false));
      }
    }

  }
}
