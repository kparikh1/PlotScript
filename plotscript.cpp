#include <string>
#include <sstream>
#include <iostream>
#include <fstream>
#include <thread>

#include "interpreter.hpp"
#include "semantic_error.hpp"
#include "Consumer.hpp"
#include "MessageQueue.hpp"

void prompt() {
  std::cout << "\nplotscript> ";
}

std::string readline() {
  std::string line;
  std::getline(std::cin, line);

  return line;
}

void error(const std::string &err_str) {
  std::cerr << "Error: " << err_str << std::endl;
}

void info(const std::string &err_str) {
  std::cout << "Info: " << err_str << std::endl;
}

int eval_from_stream(std::istream &stream) {

  Interpreter interp;

  if (!interp.parseStream(stream)) {
    error("Invalid Program. Could not parse.");
    return EXIT_FAILURE;
  } else {
    try {
      Expression exp = interp.evaluate();
      std::cout << exp << std::endl;
    }
    catch (const SemanticError &ex) {
      std::cerr << ex.what() << std::endl;
      return EXIT_FAILURE;
    }
  }

  return EXIT_SUCCESS;
}

int eval_from_file(const std::string &filename) {

  std::ifstream ifs(filename);

  if (!ifs) {
    error("Could not open file for reading.");
    return EXIT_FAILURE;
  }

  return eval_from_stream(ifs);
}

int eval_from_command(const std::string &argexp) {

  std::istringstream expression(argexp);

  return eval_from_stream(expression);
}

// A REPL is a repeated read-eval-print loop
void repl() {
  // Interpreter interp;

  std::size_t id = 1;
  MessageQueue<std::string> in;
  MessageQueue<Expression> out;
  Consumer worker(&in, &out, id);

  std::thread th1(&Consumer::run, worker);

  while (!std::cin.eof()) {

    prompt();
    std::string line = readline();

    if (line.empty())
      continue;

    if (line == "%reset") {
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

      Expression result;
      out.wait_and_pop(result);

      if (result.head().isString() && (result.head().asString() == "Threading Command") && th1.joinable()) {
        th1.join();
      } else if (result.head().isError()) {
        std::cerr << result << std::endl;
      } else
        std::cout << result << std::endl;
    } else if (!th1.joinable()) {
      std::cerr << "Error: interpreter kernel not running" << std::endl;
    }
    //std::istringstream expression(line);

//    if (!interp.parseStream(expression)) {
//      error("Invalid Expression. Could not parse.");
//    } else {
//      try {
//        Expression exp = interp.evaluate();
//        std::cout << exp << std::endl;
//      }
//      catch (const SemanticError &ex) {
//        std::cerr << ex.what() << std::endl;
//      }
//    }
  }
}

int main(int argc, char *argv[]) {
  if (argc == 2) {
    return eval_from_file(argv[1]);
  } else if (argc == 3) {
    if (std::string(argv[1]) == "-e") {
      return eval_from_command(argv[2]);
    } else {
      error("Incorrect number of command line arguments.");
    }
  } else {
    repl();
  }

  return EXIT_SUCCESS;
}
