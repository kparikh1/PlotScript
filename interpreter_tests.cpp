#include "catch.hpp"

#include <string>
#include <sstream>
#include <fstream>
#include <iostream>

#include "semantic_error.hpp"
#include "interpreter.hpp"
#include "expression.hpp"

Expression run(const std::string &program) {

  std::istringstream iss(program);

  Interpreter interp;

  bool ok = interp.parseStream(iss);
  if (!ok) {
    std::cerr << "Failed to parse: " << program << std::endl;
  }
  REQUIRE(ok == true);

  Expression result;
  REQUIRE_NOTHROW(result = interp.evaluate());

  return result;
}

TEST_CASE("Test Interpreter parser with expected input", "[interpreter]") {

  std::string program = "(begin (define r 10) (* pi (* r r)))";

  std::istringstream iss(program);

  Interpreter interp;

  bool ok = interp.parseStream(iss);

  REQUIRE(ok == true);
}

TEST_CASE("Test Interpreter parser with numerical literals", "[interpreter]") {

  std::vector<std::string> programs = {"(1)", "(+1)", "(+1e+0)", "(1e-0)"};

  for (auto program : programs) {
    std::istringstream iss(program);

    Interpreter interp;

    bool ok = interp.parseStream(iss);

    REQUIRE(ok == true);
  }

  {
    std::istringstream iss("(define x 1abc)");

    Interpreter interp;

    bool ok = interp.parseStream(iss);

    REQUIRE(ok == false);
  }
}

TEST_CASE("Test Interpreter parser with truncated input", "[interpreter]") {

  {
    std::string program = "(f";
    std::istringstream iss(program);

    Interpreter interp;
    bool ok = interp.parseStream(iss);
    REQUIRE(ok == false);
  }

  {
    std::string program = "(begin (define r 10) (* pi (* r r";
    std::istringstream iss(program);

    Interpreter interp;
    bool ok = interp.parseStream(iss);
    REQUIRE(ok == false);
  }
}

TEST_CASE("Test Interpreter parser with extra input", "[interpreter]") {

  std::string program = "(begin (define r 10) (* pi (* r r))) )";
  std::istringstream iss(program);

  Interpreter interp;

  bool ok = interp.parseStream(iss);

  REQUIRE(ok == false);
}

TEST_CASE("Test Interpreter parser with single non-keyword", "[interpreter]") {

  std::string program = "hello";
  std::istringstream iss(program);

  Interpreter interp;

  bool ok = interp.parseStream(iss);

  REQUIRE(ok == false);
}

TEST_CASE("Test Interpreter parser with empty input", "[interpreter]") {

  std::string program;
  std::istringstream iss(program);

  Interpreter interp;

  bool ok = interp.parseStream(iss);

  REQUIRE(ok == false);
}

TEST_CASE("Test Interpreter parser with empty expression", "[interpreter]") {

  std::string program = "( )";
  std::istringstream iss(program);

  Interpreter interp;

  bool ok = interp.parseStream(iss);

  REQUIRE(ok == false);
}

TEST_CASE("Test Interpreter parser with bad number string", "[interpreter]") {

  std::string program = "(1abc)";
  std::istringstream iss(program);

  Interpreter interp;

  bool ok = interp.parseStream(iss);

  REQUIRE(ok == false);
}

TEST_CASE("Test Interpreter parser with incorrect input. Regression Test", "[interpreter]") {

  std::string program = "(+ 1 2) (+ 3 4)";
  std::istringstream iss(program);

  Interpreter interp;

  bool ok = interp.parseStream(iss);

  REQUIRE(ok == false);
}

TEST_CASE("Test Interpreter result with literal expressions", "[interpreter]") {

  { // Number
    std::string program = "(4)";
    Expression result = run(program);
    REQUIRE(result == Expression(4.));
  }

  { // Symbol
    std::string program = "(pi)";
    Expression result = run(program);
    REQUIRE(result == Expression(atan2(0, -1)));
  }

}

TEST_CASE("Test Interpreter result with simple procedures (add)", "[interpreter]") {

  { // add, binary case
    std::string program = "(+ 1 2)";
    INFO(program);
    Expression result = run(program);
    REQUIRE(result == Expression(3.));
  }

  { // add, 3-ary case
    std::string program = "(+ 1 2 3)";
    INFO(program);
    Expression result = run(program);
    REQUIRE(result == Expression(6.));
  }

  { // add, 6-ary case
    std::string program = "(+ 1 2 3 4 5 6)";
    INFO(program);
    Expression result = run(program);
    REQUIRE(result == Expression(21.));
  }
}

TEST_CASE("Test Interpreter result with simple procedures (sqrt)", "[interpreter]") {

  { // sqrt
    std::string program = "(sqrt 4)";
    INFO(program);
    Expression result = run(program);
    REQUIRE(result == Expression(2.));
  }
  {
    std::string program = "(sqrt -256)";
    INFO(program);
    Expression result = run(program);
    REQUIRE(result == Expression(std::complex<long double>(0., 16.)));
  }
  {
    std::string program = "(sqrt I)";
    INFO(program);
    Expression result = run(program);
    REQUIRE(result == Expression(std::complex<long double>(0.707106781186547524, 0.707106781186547524)));
  }
  {
    std::string program = "(sqrt (+ 1 I))";
    INFO(program);
    Expression result = run(program);
    REQUIRE(result == Expression(std::complex<long double>(1.0986841134678099, 0.4550898605622273)));
  }

}

TEST_CASE("Test Interpreter result with simple procedures (exp)", "[interpreter]") {

  { // exponent
    std::string program = "(^ 4 4)";
    INFO(program);
    Expression result = run(program);
    REQUIRE(result == Expression(256.));
  }

  {
    std::string program = "(^ I 2)";
    INFO(program);
    Expression result = run(program);
    REQUIRE(result == Expression(std::complex<long double>(-1, 0)));
  }
  {
    std::string program = "(^ 2 I)";
    INFO(program);
    Expression result = run(program);
    REQUIRE(
        result == Expression(std::complex<long double>(0.76923890136397212657832999, 0.63896127631363480115003291)));
  }
  {
    std::string program = "(^ I I)";
    INFO(program);
    Expression result = run(program);
    REQUIRE(
        result == Expression(std::complex<long double>(0.207879576350761908546955, 0.)));
  }
}

TEST_CASE("Test Interpreter result with simple procedures (ln)", "[interpreter]") {

  { // natural log
    std::string program = "(ln e)";
    INFO(program);
    Expression result = run(program);
    REQUIRE(result == Expression(1.));
  }
}

TEST_CASE("Test Interpreter result with simple procedures (sin)", "[interpreter]") {

  { // Sin
    std::string program = "(sin (/ pi 2))";
    INFO(program);
    Expression result = run(program);
    REQUIRE(result == Expression(1.));
  }
}

TEST_CASE("Test Interpreter result with simple procedures (cos)", "[interpreter]") {

  { // cos
    std::string program = "(cos (/ pi 2))";
    INFO(program);
    Expression result = run(program);
    REQUIRE(result == Expression(0.));
  }
}

TEST_CASE("Test Interpreter result with simple procedures (tan)", "[interpreter]") {

  { // tan
    std::string program = "(tan 0)";
    INFO(program);
    Expression result = run(program);
    REQUIRE(result == Expression(0.));
  }
}

TEST_CASE("Test Interpreter special forms: begin and define", "[interpreter]") {

  {
    std::string program = "(define answer 42)";
    INFO(program);
    Expression result = run(program);
    REQUIRE(result == Expression(42.));
  }

  {
    std::string program = "(begin (define answer 42)\n(answer))";
    INFO(program);
    Expression result = run(program);
    REQUIRE(result == Expression(42.));
  }

  {
    std::string program = "(begin (define answer (+ 9 11)) (answer))";
    INFO(program);
    Expression result = run(program);
    REQUIRE(result == Expression(20.));
  }

  {
    std::string program = "(begin (define a 1) (define b 1) (+ a b))";
    INFO(program);
    Expression result = run(program);
    REQUIRE(result == Expression(2.));
  }
}

TEST_CASE("Test a medium-sized expression", "[interpreter]") {

  {
    std::string program = "(+ (+ 10 1) (+ 30 (+ 1 1)))";
    Expression result = run(program);
    REQUIRE(result == Expression(43.));
  }
}

TEST_CASE("Test arithmetic Complex procedures", "[interpreter]") {

  {
    std::vector<std::string> programs = {"(+ 1 -2)",
                                         "(+ -3 1 1)",
                                         "(- 1)",
                                         "(- 1 2)",
                                         "(* 1 -1)",
                                         "(* 1 1 -1)",
                                         "(/ -1 1)",
                                         "(/ 1 -1)"};

    for (auto s : programs) {
      Expression result = run(s);
      REQUIRE(result == Expression(-1.));
    }
  }
}

TEST_CASE("Test arithmetic procedures", "[interpreter]") {

  {
    std::vector<std::string> programs = {"(+ -1 (- I))",
                                         "(+ -3 1 1 (- I))",
                                         "(- -1  I)",
                                         "(* 1 (+ -1 (- I)))",
                                         "(* 1 I I I I (+ -1 (- I)))",};
    std::complex<long double> comp(-1, -1);
    for (auto s : programs) {
      Expression result = run(s);
      REQUIRE(result == Expression(comp));
    }
  }
}

TEST_CASE("Test some semantically invalid expresions", "[interpreter]") {

  std::vector<std::string> programs = {"(@ none)", // so such procedure
                                       "(- 1 1 2)", // too many arguments
                                       "(define begin 1)", // redefine special form
                                       "(define pi 3.14)"}; // redefine builtin symbol
  for (auto s : programs) {
    Interpreter interp;

    std::istringstream iss(s);

    bool ok = interp.parseStream(iss);
    REQUIRE(ok == true);

    REQUIRE_THROWS_AS(interp.evaluate(), SemanticError);
  }
}

TEST_CASE("Test for exceptions from semantically incorrect input", "[interpreter]") {

  std::string input = R"(
(+ 1 a)
)";

  Interpreter interp;

  std::istringstream iss(input);

  bool ok = interp.parseStream(iss);
  REQUIRE(ok == true);

  REQUIRE_THROWS_AS(interp.evaluate(), SemanticError);
}

TEST_CASE("Test malformed define", "[interpreter]") {

  std::string input = R"(
(define a 1 2)
)";

  Interpreter interp;

  std::istringstream iss(input);

  bool ok = interp.parseStream(iss);
  REQUIRE(ok == true);

  REQUIRE_THROWS_AS(interp.evaluate(), SemanticError);
}

TEST_CASE("Test using number as procedure", "[interpreter]") {
  std::string input = R"(
(1 2 3)
)";

  Interpreter interp;

  std::istringstream iss(input);

  bool ok = interp.parseStream(iss);
  REQUIRE(ok == true);

  REQUIRE_THROWS_AS(interp.evaluate(), SemanticError);
}

//TEST_CASE("Test negative sqrt procedure", "[interpreter]") {
//  std::string input = R"(
//(sqrt -4)
//)";
//
//  Interpreter interp;
//
//  std::istringstream iss(input);
//
//  bool ok = interp.parseStream(iss);
//  REQUIRE(ok == true);
//
//  REQUIRE_THROWS_AS(interp.evaluate(), SemanticError);
//}

TEST_CASE("Test multiple sqrt arguments", "[interpreter]") {
  std::string input = R"(
(sqrt 4 2)
)";

  Interpreter interp;

  std::istringstream iss(input);

  bool ok = interp.parseStream(iss);
  REQUIRE(ok == true);

  REQUIRE_THROWS_AS(interp.evaluate(), SemanticError);
}

TEST_CASE("Test multiple exponent arguments", "[interpreter]") {
  std::string input = R"(
(^ 0 1 2)
)";

  Interpreter interp;

  std::istringstream iss(input);

  bool ok = interp.parseStream(iss);
  REQUIRE(ok == true);

  REQUIRE_THROWS_AS(interp.evaluate(), SemanticError);
}

TEST_CASE("Test multiple natural log arguments", "[interpreter]") {
  std::string input = R"(
(ln 0 1)
)";

  Interpreter interp;

  std::istringstream iss(input);

  bool ok = interp.parseStream(iss);
  REQUIRE(ok == true);

  REQUIRE_THROWS_AS(interp.evaluate(), SemanticError);
}

TEST_CASE("Test multiple sin arguments", "[interpreter]") {
  std::string input = R"(
(sin 0 1)
)";

  Interpreter interp;

  std::istringstream iss(input);

  bool ok = interp.parseStream(iss);
  REQUIRE(ok == true);

  REQUIRE_THROWS_AS(interp.evaluate(), SemanticError);
}

TEST_CASE("Test multiple cos arguments", "[interpreter]") {
  std::string input = R"(
(cos 0 1)
)";

  Interpreter interp;

  std::istringstream iss(input);

  bool ok = interp.parseStream(iss);
  REQUIRE(ok == true);

  REQUIRE_THROWS_AS(interp.evaluate(), SemanticError);
}

TEST_CASE("Test multiple tan arguments", "[interpreter]") {
  std::string input = R"(
(tan 0 1)
)";

  Interpreter interp;

  std::istringstream iss(input);

  bool ok = interp.parseStream(iss);
  REQUIRE(ok == true);

  REQUIRE_THROWS_AS(interp.evaluate(), SemanticError);
}