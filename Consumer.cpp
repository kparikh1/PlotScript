//
// Created by kishanp on 12/3/18.
//

#include <istream>
#include <thread>

#include "Consumer.hpp"
#include "interpreter.hpp"
#include "semantic_error.hpp"

void Consumer::run() {

  bool ContinueRun = true;
  std::string message;
  Interpreter interp;

  while (ContinueRun) {
    incomingMB->wait_and_pop(message);
    ContinueRun = (message != "%stop");

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
void Consumer::start() {
  std::thread th1(&Consumer::run, NULL);
}
