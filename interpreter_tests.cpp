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
  REQUIRE(ok);

  Expression result;
  REQUIRE_NOTHROW(result = interp.evaluate());

  return result;
}

TEST_CASE("Test Interpreter parser with expected input", "[interpreter]") {

  std::string program = "(begin (define r 10) (* pi (* r r)))";

  std::istringstream iss(program);

  Interpreter interp;

  bool ok = interp.parseStream(iss);

  REQUIRE(ok);
}

TEST_CASE("Test Interpreter parser with numerical literals", "[interpreter]") {

  std::vector<std::string> programs = {"(1)", "(+1)", "(+1e+0)", "(1e-0)"};

  for (const auto &program : programs) {
    std::istringstream iss(program);

    Interpreter interp;

    bool ok = interp.parseStream(iss);

    REQUIRE(ok);
  }

  {
    std::istringstream iss("(define x 1abc)");

    Interpreter interp;

    bool ok = interp.parseStream(iss);

    REQUIRE(!ok);
  }
}

TEST_CASE("Test Interpreter parser with truncated input", "[interpreter]") {

  {
    std::string program = "(f";
    std::istringstream iss(program);

    Interpreter interp;
    bool ok = interp.parseStream(iss);
    REQUIRE(!ok);
  }

  {
    std::string program = "(begin (define r 10) (* pi (* r r";
    std::istringstream iss(program);

    Interpreter interp;
    bool ok = interp.parseStream(iss);
    REQUIRE(!ok);
  }
}

TEST_CASE("Test Interpreter parser with extra input", "[interpreter]") {

  std::string program = "(begin (define r 10) (* pi (* r r))) )";
  std::istringstream iss(program);

  Interpreter interp;

  bool ok = interp.parseStream(iss);

  REQUIRE(!ok);
}

TEST_CASE("Test Interpreter parser with single non-keyword", "[interpreter]") {

  std::string program = "hello";
  std::istringstream iss(program);

  Interpreter interp;

  bool ok = interp.parseStream(iss);

  REQUIRE(!ok);
}

TEST_CASE("Test Interpreter parser with empty input", "[interpreter]") {

  std::string program;
  std::istringstream iss(program);

  Interpreter interp;

  bool ok = interp.parseStream(iss);

  REQUIRE(!ok);
}

TEST_CASE("Test Interpreter parser with empty expression", "[interpreter]") {

  std::string program = "( )";
  std::istringstream iss(program);

  Interpreter interp;

  bool ok = interp.parseStream(iss);

  REQUIRE(!ok);
}

TEST_CASE("Test Interpreter parser with bad number string", "[interpreter]") {

  std::string program = "(1abc)";
  std::istringstream iss(program);

  Interpreter interp;

  bool ok = interp.parseStream(iss);

  REQUIRE(!ok);
}

TEST_CASE("Test Interpreter parser with incorrect input. Regression Test", "[interpreter]") {

  std::string program = "(+ 1 2) (+ 3 4)";
  std::istringstream iss(program);

  Interpreter interp;

  bool ok = interp.parseStream(iss);

  REQUIRE(!ok);
}

TEST_CASE("Test Interpreter result with literal expressions", "[interpreter]") {

  { // Number
    std::string program = "(4)";
    Expression result = run(program);
    REQUIRE(result == Expression(Atom(4.)));
  }

  { // Symbol
    std::string program = "(pi)";
    Expression result = run(program);
    REQUIRE(result == Expression(Atom(atan2(0, -1))));
  }

  { // Empty List
    std::string program = "(list)";
    Expression result = run(program);
    REQUIRE(result == Expression(""));
  }

}

TEST_CASE("Test Interpreter result with simple procedures (add)", "[interpreter]") {

  { // add, binary case
    std::string program = "(+ 1 2)";
    INFO(program);
    Expression result = run(program);
    REQUIRE(result == Expression(Atom(3.)));
  }

  { // add, 3-ary case
    std::string program = "(+ 1 2 3)";
    INFO(program);
    Expression result = run(program);
    REQUIRE(result == Expression(Atom(6.)));
  }

  { // add, 6-ary case
    std::string program = "(+ 1 2 3 4 5 6)";
    INFO(program);
    Expression result = run(program);
    REQUIRE(result == Expression(Atom(21.)));
  }
}

TEST_CASE("Test Interpreter result with simple procedures (sqrt)", "[interpreter]") {

  { // sqrt
    std::string program = "(sqrt 4)";
    INFO(program);
    Expression result = run(program);
    REQUIRE(result == Expression(Atom(2.)));
  }
  {
    std::string program = "(sqrt -256)";
    INFO(program);
    Expression result = run(program);
    REQUIRE(result == Expression(Atom(std::complex<double>(0., 16.))));
  }
  {
    std::string program = "(sqrt I)";
    INFO(program);
    Expression result = run(program);
    REQUIRE(result == Expression(Atom(std::complex<double>(0.707106781186547524, 0.707106781186547524))));
  }
  {
    std::string program = "(sqrt (+ 1 I))";
    INFO(program);
    Expression result = run(program);
    REQUIRE(result == Expression(Atom(std::complex<double>(1.0986841134678099, 0.4550898605622273))));
  }

}

TEST_CASE("Test Interpreter result with simple procedures (exp)", "[interpreter]") {

  { // exponent
    std::string program = "(^ 4 4)";
    INFO(program);
    Expression result = run(program);
    REQUIRE(result == Expression(Atom(256.)));
  }
  { // exponent
    std::string program = "(^ 6 3)";
    INFO(program);
    Expression result = run(program);
    REQUIRE(result == Expression(Atom(216.)));
  }

  {
    std::string program = "(^ I 2)";
    INFO(program);
    Expression result = run(program);
    REQUIRE(result == Expression(Atom(std::complex<double>(-1, 0))));
  }
  {
    std::string program = "(^ 2 I)";
    INFO(program);
    Expression result = run(program);
    REQUIRE(
        result == Expression(Atom(std::complex<double>(0.76923890136397212657832999, 0.63896127631363480115003291))));
  }
  {
    std::string program = "(^ I I)";
    INFO(program);
    Expression result = run(program);
    REQUIRE(
        result == Expression(Atom(std::complex<double>(0.207879576350761908546955, 0.))));
  }
  {
    std::string program = "(^ e (- (* pi I)))";
    INFO(program);
    Expression result = run(program);
    REQUIRE(
        result == Expression(Atom(std::complex<double>(-1., 0.))));
  }
}

TEST_CASE("Test Interpreter result with simple procedures (ln)", "[interpreter]") {

  { // natural log
    std::string program = "(ln e)";
    INFO(program);
    Expression result = run(program);
    REQUIRE(result == Expression(Atom(1.)));
  }
}

TEST_CASE("Test Interpreter result with simple procedures (sin)", "[interpreter]") {

  { // Sin
    std::string program = "(sin (/ pi 2))";
    INFO(program);
    Expression result = run(program);
    REQUIRE(result == Expression(Atom(1.)));
  }
}

TEST_CASE("Test Interpreter result with simple procedures (cos)", "[interpreter]") {

  { // cos
    std::string program = "(cos (/ pi 2))";
    INFO(program);
    Expression result = run(program);
    REQUIRE(result == Expression(Atom(0.)));
  }
}

TEST_CASE("Test Interpreter result with simple procedures (tan)", "[interpreter]") {

  { // tan
    std::string program = "(tan 0)";
    INFO(program);
    Expression result = run(program);
    REQUIRE(result == Expression(Atom(0.)));
  }
}

TEST_CASE("Test Interpreter result with simple procedures (real)", "[interpreter]") {

  { // real
    std::string program = "(real (+ 1 I))";
    INFO(program);
    Expression result = run(program);
    REQUIRE(result == Expression(1.));
  }
}

TEST_CASE("Test Interpreter result with simple procedures (imag)", "[interpreter]") {

  { // imag
    std::string program = "(imag (+ 1 (* 2 I)))";
    INFO(program);
    Expression result = run(program);
    REQUIRE(result == Expression(2.));
  }
}

TEST_CASE("Test Interpreter result with simple procedures (mag)", "[interpreter]") {

  { // mag
    std::string program = "(mag (+ 1 (* 2 I)))";
    INFO(program);
    Expression result = run(program);
    REQUIRE(result == Expression(2.23606797749979));
  }
}

TEST_CASE("Test Interpreter result with simple procedures (arg)", "[interpreter]") {

  { // arg
    std::string program = "(arg (+ 1 (* 2 I)))";
    INFO(program);
    Expression result = run(program);
    REQUIRE(result == Expression(1.10714871779409));
  }
}

TEST_CASE("Test Interpreter result with simple procedures (conj)", "[interpreter]") {

  { // conj
    std::string program = "(conj (+ 1 (* 2 I)))";
    INFO(program);
    Expression result = run(program);
    REQUIRE(result == Expression(std::complex<double>(1, -2)));
  }
}

TEST_CASE("Test Interpreter result with simple procedures (first)", "[interpreter]") {

  { // first
    std::string program = "(first (list 1 2 3))";
    INFO(program);
    Expression result = run(program);
    REQUIRE(result == Expression(1));
  }
}

TEST_CASE("Test Interpreter result with simple procedures (rest)", "[interpreter]") {

  { // rest
    std::string program = "(rest (list 1 2 3))";
    INFO(program);
    Expression result = run(program);
    Expression exp;
    exp.getTail().emplace_back(Expression(2));
    exp.getTail().emplace_back(Expression(3));
    REQUIRE(result == exp);
  }
}

TEST_CASE("Test Interpreter result with simple procedures (length)", "[interpreter]") {

  { // length
    std::string program = "(length (list 1 2 3))";
    INFO(program);
    Expression result = run(program);
    REQUIRE(result == Expression(3));

    program = "(length (list))";
    INFO(program);
    result = run(program);
    REQUIRE(result == Expression(0));
  }
}

TEST_CASE("Test Interpreter result with simple procedures (append)", "[interpreter]") {

  { // append
    std::string program = "(append (list 1 2 3))";
    INFO(program);
    Expression result = run(program);
    Expression exp;
    exp.getTail().emplace_back(Expression(1));
    exp.getTail().emplace_back(Expression(2));
    exp.getTail().emplace_back(Expression(3));
    REQUIRE(result == exp);

    program = "(append (list))";
    INFO(program);
    result = run(program);
    REQUIRE(result == Expression(""));

    program = "(append (list 1 2 3) 4 5 6)";
    INFO(program);
    result = run(program);
    exp.getTail().clear();
    exp.getTail().emplace_back(Expression(1));
    exp.getTail().emplace_back(Expression(2));
    exp.getTail().emplace_back(Expression(3));
    exp.getTail().emplace_back(Expression(4));
    exp.getTail().emplace_back(Expression(5));
    exp.getTail().emplace_back(Expression(6));
    REQUIRE(result == exp);

  }
}

TEST_CASE("Test Interpreter result with simple procedures (join)", "[interpreter]") {
  // join
  std::string program = "(join (list 1 2 3) (list 4 5 6))";
  INFO(program);
  Expression result = run(program);
  Expression exp;
  exp.getTail().emplace_back(Expression(1));
  exp.getTail().emplace_back(Expression(2));
  exp.getTail().emplace_back(Expression(3));
  exp.getTail().emplace_back(Expression(4));
  exp.getTail().emplace_back(Expression(5));
  exp.getTail().emplace_back(Expression(6));
  REQUIRE(result == exp);

  program = "(join (list 1 2 3) (list 4 5 6) (list (+ 2 I)))";
  INFO(program);
  result = run(program);
  exp.getTail().clear();
  exp.getTail().emplace_back(Expression(1));
  exp.getTail().emplace_back(Expression(2));
  exp.getTail().emplace_back(Expression(3));
  exp.getTail().emplace_back(Expression(4));
  exp.getTail().emplace_back(Expression(5));
  exp.getTail().emplace_back(Expression(6));
  exp.getTail().emplace_back(Expression(std::complex<double>(2, 1)));
  REQUIRE(result == exp);
}

TEST_CASE("Test Interpreter result with simple procedures (range)", "[interpreter]") {
  // range
  std::string program = "(range 1 6 1)";
  INFO(program);
  Expression result = run(program);
  Expression exp;
  exp.getTail().emplace_back(Expression(1));
  exp.getTail().emplace_back(Expression(2));
  exp.getTail().emplace_back(Expression(3));
  exp.getTail().emplace_back(Expression(4));
  exp.getTail().emplace_back(Expression(5));
  exp.getTail().emplace_back(Expression(6));
  REQUIRE(result == exp);
}

TEST_CASE("Test Interpreter special forms: begin and define and list", "[interpreter]") {

  {
    std::string program = "(define answer 42)";
    INFO(program);
    Expression result = run(program);
    REQUIRE(result == Expression(Atom(42.)));
  }

  {
    std::string program = "(begin (define answer 42)\n(answer))";
    INFO(program);
    Expression result = run(program);
    REQUIRE(result == Expression(Atom(42.)));
  }

  {
    std::string program = "(begin (define answer (+ 9 11)) (answer))";
    INFO(program);
    Expression result = run(program);
    REQUIRE(result == Expression(Atom(20.)));
  }

  {
    std::string program = "(begin (define a 1) (define b 1) (+ a b))";
    INFO(program);
    Expression result = run(program);
    REQUIRE(result == Expression(Atom(2.)));
  }

  {
    std::string program = "(list 1 2 3 4)";
    INFO(program);
    Expression result = run(program);
    Expression exp;
    exp.getTail().emplace_back(Expression(1));
    exp.getTail().emplace_back(Expression(2));
    exp.getTail().emplace_back(Expression(3));
    exp.getTail().emplace_back(Expression(4));
    REQUIRE(result == exp);
  }
}

TEST_CASE("Test Interpreter with lambda Expression (double)", "[interpreter]") {

  std::string program = "(begin (define double (lambda (x) (* 2 x))) (double 2))";
  std::istringstream iss(program);

  Interpreter interp;

  REQUIRE(interp.parseStream(iss));
  INFO(program);
  Expression result = run(program);
  REQUIRE(result == Expression(4.));

}

TEST_CASE("Test a medium-sized expression", "[interpreter]") {

  {
    std::string program = "(+ (+ 10 1) (+ 30 (+ 1 1)))";
    Expression result = run(program);
    REQUIRE(result == Expression(Atom(43.)));
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
      REQUIRE(result == Expression(Atom(-1.)));
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
    std::complex<double> comp(-1, -1);
    for (auto s : programs) {
      Expression result = run(s);
      REQUIRE(result == Expression(Atom(comp)));
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
    REQUIRE(ok);

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
  REQUIRE(ok);

  REQUIRE_THROWS_AS(interp.evaluate(), SemanticError);
}

TEST_CASE("Test malformed define", "[interpreter]") {

  std::string input = R"(
(define a 1 2)
)";

  Interpreter interp;

  std::istringstream iss(input);

  bool ok = interp.parseStream(iss);
  REQUIRE(ok);

  REQUIRE_THROWS_AS(interp.evaluate(), SemanticError);
}

TEST_CASE("Test using number as procedure", "[interpreter]") {
  std::string input = R"(
(1 2 3)
)";

  Interpreter interp;

  std::istringstream iss(input);

  bool ok = interp.parseStream(iss);
  REQUIRE(ok);

  REQUIRE_THROWS_AS(interp.evaluate(), SemanticError);
}

TEST_CASE("Test multiple div arguments", "[interpreter]") {
  std::string input = R"(
(div (+ 1 2 3))
)";

  Interpreter interp;

  std::istringstream iss(input);

  bool ok = interp.parseStream(iss);
  REQUIRE(ok);

  REQUIRE_THROWS_AS(interp.evaluate(), SemanticError);
}

TEST_CASE("Test multiple sqrt arguments", "[interpreter]") {
  std::string input = R"(
(sqrt 4 2)
)";

  Interpreter interp;

  std::istringstream iss(input);

  bool ok = interp.parseStream(iss);
  REQUIRE(ok);

  REQUIRE_THROWS_AS(interp.evaluate(), SemanticError);
}

TEST_CASE("Test multiple exponent arguments", "[interpreter]") {
  std::string input = R"(
(^ 0 1 2)
)";

  Interpreter interp;

  std::istringstream iss(input);

  bool ok = interp.parseStream(iss);
  REQUIRE(ok);

  REQUIRE_THROWS_AS(interp.evaluate(), SemanticError);
}

TEST_CASE("Test multiple natural log arguments", "[interpreter]") {
  std::string input = R"(
(ln 0 1)
)";

  Interpreter interp;

  std::istringstream iss(input);

  bool ok = interp.parseStream(iss);
  REQUIRE(ok);

  REQUIRE_THROWS_AS(interp.evaluate(), SemanticError);
}

TEST_CASE("Test multiple sin arguments", "[interpreter]") {
  std::string input = R"(
(sin 0 1)
)";

  Interpreter interp;

  std::istringstream iss(input);

  bool ok = interp.parseStream(iss);
  REQUIRE(ok);

  REQUIRE_THROWS_AS(interp.evaluate(), SemanticError);
}

TEST_CASE("Test multiple cos arguments", "[interpreter]") {
  std::string input = R"(
(cos 0 1)
)";

  Interpreter interp;

  std::istringstream iss(input);

  bool ok = interp.parseStream(iss);
  REQUIRE(ok);

  REQUIRE_THROWS_AS(interp.evaluate(), SemanticError);
}

TEST_CASE("Test multiple tan arguments", "[interpreter]") {
  std::string input = R"(
(tan 0 1)
)";

  Interpreter interp;

  std::istringstream iss(input);

  bool ok = interp.parseStream(iss);
  REQUIRE(ok);

  REQUIRE_THROWS_AS(interp.evaluate(), SemanticError);
}

TEST_CASE("Test multiple real arguments", "[interpreter]") {
  std::string input = R"(
(real (+ 1 I) 1)
)";

  Interpreter interp;

  std::istringstream iss(input);

  bool ok = interp.parseStream(iss);
  REQUIRE(ok == true);

  REQUIRE_THROWS_AS(interp.evaluate(), SemanticError);
}

TEST_CASE("Test multiple imag arguments", "[interpreter]") {
  std::string input = R"(
(imag (+ 1 I) 1)
)";

  Interpreter interp;

  std::istringstream iss(input);

  bool ok = interp.parseStream(iss);
  REQUIRE(ok == true);

  REQUIRE_THROWS_AS(interp.evaluate(), SemanticError);
}

TEST_CASE("Test multiple mag arguments", "[interpreter]") {
  std::string input = R"(
(mag (+ 1 I) 1)
)";

  Interpreter interp;

  std::istringstream iss(input);

  bool ok = interp.parseStream(iss);
  REQUIRE(ok == true);

  REQUIRE_THROWS_AS(interp.evaluate(), SemanticError);
}

TEST_CASE("Test multiple arg arguments", "[interpreter]") {
  std::string input = R"(
(arg (+ 1 I) 1)
)";

  Interpreter interp;

  std::istringstream iss(input);

  bool ok = interp.parseStream(iss);
  REQUIRE(ok == true);

  REQUIRE_THROWS_AS(interp.evaluate(), SemanticError);
}

TEST_CASE("Test multiple conj arguments", "[interpreter]") {
  std::string input = R"(
(conj (+ 1 I) (- 1 I))
)";

  Interpreter interp;

  std::istringstream iss(input);

  bool ok = interp.parseStream(iss);
  REQUIRE(ok);

  REQUIRE_THROWS_AS(interp.evaluate(), SemanticError);
}

TEST_CASE("Test multiple first arguments", "[interpreter]") {
  std::string input = R"(
(first (list 1 2 3) (list 1 2 3))
)";

  Interpreter interp;

  std::istringstream iss(input);

  bool ok = interp.parseStream(iss);
  REQUIRE(ok);

  REQUIRE_THROWS_AS(interp.evaluate(), SemanticError);
}

TEST_CASE("Test multiple rest arguments", "[interpreter]") {
  std::string input = R"(
(rest (list 1 2 3) (list 1 2 3))
)";

  Interpreter interp;

  std::istringstream iss(input);

  bool ok = interp.parseStream(iss);
  REQUIRE(ok);

  REQUIRE_THROWS_AS(interp.evaluate(), SemanticError);
}

TEST_CASE("Test multiple length arguments", "[interpreter]") {
  std::string input = R"(
(length (list 1 2 3) (list 1 2 3))
)";

  Interpreter interp;

  std::istringstream iss(input);

  bool ok = interp.parseStream(iss);
  REQUIRE(ok);

  REQUIRE_THROWS_AS(interp.evaluate(), SemanticError);
}

TEST_CASE("Test empty argument to first", "[interpreter]") {
  std::string input = R"(
(first (list))
)";

  Interpreter interp;

  std::istringstream iss(input);

  bool ok = interp.parseStream(iss);
  REQUIRE(ok);

  REQUIRE_THROWS_AS(interp.evaluate(), SemanticError);
}

TEST_CASE("Test empty argument to rest", "[interpreter]") {
  std::string input = R"(
(rest (list))
)";

  Interpreter interp;

  std::istringstream iss(input);

  bool ok = interp.parseStream(iss);
  REQUIRE(ok);

  REQUIRE_THROWS_AS(interp.evaluate(), SemanticError);
}

TEST_CASE("Test multiple argument to range", "[interpreter]") {
  std::string input = R"(
(range 0 1 2 3)
)";

  Interpreter interp;

  std::istringstream iss(input);

  bool ok = interp.parseStream(iss);
  REQUIRE(ok);

  REQUIRE_THROWS_AS(interp.evaluate(), SemanticError);
}

TEST_CASE("Test multiple argument to lambda", "[interpreter]") {
  std::string input = R"(
(lambda (x) (*2 x) (- x))
)";

  Interpreter interp;

  std::istringstream iss(input);

  bool ok = interp.parseStream(iss);
  REQUIRE(ok);

  REQUIRE_THROWS_AS(interp.evaluate(), SemanticError);
}

TEST_CASE("Test invalid real arguments", "[interpreter]") {
  std::string input = R"(
(real 1)
)";

  Interpreter interp;

  std::istringstream iss(input);

  bool ok = interp.parseStream(iss);
  REQUIRE(ok);

  REQUIRE_THROWS_AS(interp.evaluate(), SemanticError);
}

TEST_CASE("Test invalid imag arguments", "[interpreter]") {
  std::string input = R"(
(imag 1)
)";

  Interpreter interp;

  std::istringstream iss(input);

  bool ok = interp.parseStream(iss);
  REQUIRE(ok);

  REQUIRE_THROWS_AS(interp.evaluate(), SemanticError);
}

TEST_CASE("Test invalid mag arguments", "[interpreter]") {
  std::string input = R"(
(mag 1)
)";

  Interpreter interp;

  std::istringstream iss(input);

  bool ok = interp.parseStream(iss);
  REQUIRE(ok);

  REQUIRE_THROWS_AS(interp.evaluate(), SemanticError);
}

TEST_CASE("Test invalid arg arguments", "[interpreter]") {
  std::string input = R"(
(arg 1)
)";

  Interpreter interp;

  std::istringstream iss(input);

  bool ok = interp.parseStream(iss);
  REQUIRE(ok);

  REQUIRE_THROWS_AS(interp.evaluate(), SemanticError);
}

TEST_CASE("Test invalid conj arguments", "[interpreter]") {
  std::string input = R"(
(conj 1)
)";

  Interpreter interp;

  std::istringstream iss(input);

  bool ok = interp.parseStream(iss);
  REQUIRE(ok);

  REQUIRE_THROWS_AS(interp.evaluate(), SemanticError);
}

TEST_CASE("Test invalid first arguments", "[interpreter]") {
  std::string input = R"(
(first 1)
)";

  Interpreter interp;

  std::istringstream iss(input);

  bool ok = interp.parseStream(iss);
  REQUIRE(ok);

  REQUIRE_THROWS_AS(interp.evaluate(), SemanticError);
}

TEST_CASE("Test invalid rest arguments", "[interpreter]") {
  std::string input = R"(
(rest 1)
)";

  Interpreter interp;

  std::istringstream iss(input);

  bool ok = interp.parseStream(iss);
  REQUIRE(ok);

  REQUIRE_THROWS_AS(interp.evaluate(), SemanticError);
}

TEST_CASE("Test invalid length arguments", "[interpreter]") {
  std::string input = R"(
(length 1)
)";

  Interpreter interp;

  std::istringstream iss(input);

  bool ok = interp.parseStream(iss);
  REQUIRE(ok);

  REQUIRE_THROWS_AS(interp.evaluate(), SemanticError);
}

TEST_CASE("Test invalid append arguments", "[interpreter]") {
  std::string input = R"(
(append 1 (list 1 3 4))
)";

  Interpreter interp;

  std::istringstream iss(input);

  bool ok = interp.parseStream(iss);
  REQUIRE(ok);

  REQUIRE_THROWS_AS(interp.evaluate(), SemanticError);
}

TEST_CASE("Test invalid join arguments", "[interpreter]") {
  std::string input = R"(
(join (list 1 2 3) 23 (list 1 23 4))
)";

  Interpreter interp;

  std::istringstream iss(input);

  bool ok = interp.parseStream(iss);
  REQUIRE(ok);

  REQUIRE_THROWS_AS(interp.evaluate(), SemanticError);
}

TEST_CASE("Test invalid range arguments (complex)", "[interpreter]") {
  std::string input = R"(
(range (- I I) 3 1)
)";

  Interpreter interp;

  std::istringstream iss(input);

  bool ok = interp.parseStream(iss);
  REQUIRE(ok);

  REQUIRE_THROWS_AS(interp.evaluate(), SemanticError);
}

TEST_CASE("Test invalid range arguments (begin/end)", "[interpreter]") {
  std::string input = R"(
(range 2 1 1)
)";

  Interpreter interp;

  std::istringstream iss(input);

  bool ok = interp.parseStream(iss);
  REQUIRE(ok);

  REQUIRE_THROWS_AS(interp.evaluate(), SemanticError);
}

TEST_CASE("Test invalid range arguments (zero incement)", "[interpreter]") {
  std::string input = R"(
(range 1 4 0)
)";

  Interpreter interp;

  std::istringstream iss(input);

  bool ok = interp.parseStream(iss);
  REQUIRE(ok);

  REQUIRE_THROWS_AS(interp.evaluate(), SemanticError);
}

TEST_CASE("Test invalid range arguments (negative incement)", "[interpreter]") {
  std::string input = R"(
(range 0 5 (- 1))
)";

  Interpreter interp;

  std::istringstream iss(input);

  bool ok = interp.parseStream(iss);
  REQUIRE(ok);

  REQUIRE_THROWS_AS(interp.evaluate(), SemanticError);
}