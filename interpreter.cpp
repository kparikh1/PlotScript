#include "interpreter.hpp"
#include "startup_config.hpp"

// system includes
#include <stdexcept>
#include <fstream>

// module includes
#include "token.hpp"
#include "parse.hpp"
#include "expression.hpp"
#include "environment.hpp"
#include "semantic_error.hpp"

std::stringstream startUp;

void readStartUpFile() {
  if (startUp.str().empty()) {
    std::ifstream ifs(STARTUP_FILE);
    if (ifs) {
      startUp << ifs.rdbuf();
      ifs.close();
    }
  }
}

bool Interpreter::parseStream(std::istream &expression) noexcept {

  TokenSequenceType tokens = tokenize(expression);

  ast = parse(tokens);

  return (ast != Expression());
};

Expression Interpreter::evaluate() {

  return ast.eval(env);
}
Interpreter::Interpreter() {

  readStartUpFile();

  parseStream(startUp);
  evaluate();

  startUp.clear();
  startUp.seekg(0);
}
