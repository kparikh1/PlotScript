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

bool Interpreter::parseStream(std::istream & expression) noexcept{

  TokenSequenceType tokens = tokenize(expression);

  ast = parse(tokens);

  return (ast != Expression());
};
				     

Expression Interpreter::evaluate(){

  return ast.eval(env);
}
Interpreter::Interpreter() {

  std::ifstream ifs(STARTUP_FILE);

  parseStream(ifs);
  evaluate();

}
