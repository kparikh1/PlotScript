//
// Created by kishanp on 12/3/18.
//

#include <istream>
#include <thread>

#include "Consumer.hpp"
#include "interpreter.hpp"
#include "semantic_error.hpp"

void Consumer::run() {

  std::string message;
  Interpreter interp;

  while (1) {
    incomingMB->wait_and_pop(message);
    if (message == "%stop") {
      outgoingMB->push(Expression("Threading Command", true));
      return;
    } else if (message == "%start") {
      outgoingMB->push(Expression("Threading Command", true));
    }

    std::istringstream iss(message);

    if (!interp.parseStream(iss)) {
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
