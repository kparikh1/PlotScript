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

TEST_CASE("Test Interpreter result with simple procedures (div)", "[interpreter]") {
  { // div 1 arg
    std::string program = "(/ 2)";
    INFO(program);
    Expression result = run(program);
    REQUIRE(result == Expression(0.5));
  }
  { // div 2 args
    std::string program = "(/ 6 3)";
    INFO(program);
    Expression result = run(program);
    REQUIRE(result == Expression(2));
  }
  {
    std::string input = R"(
(/ "string" 3)
)";

    Interpreter interp;

    std::istringstream iss(input);

    bool ok = interp.parseStream(iss);
    REQUIRE(ok);
    REQUIRE_THROWS_AS(interp.evaluate(), SemanticError);
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
  {
    std::string input = R"(
(sqrt "hello")
)";

    Interpreter interp;

    std::istringstream iss(input);

    bool ok = interp.parseStream(iss);
    REQUIRE(ok);
    REQUIRE_THROWS_AS(interp.evaluate(), SemanticError);
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
  {
    std::string input = R"(
(^ 2 "hi");
)";

    Interpreter interp;

    std::istringstream iss(input);

    bool ok = interp.parseStream(iss);
    REQUIRE(ok);
    REQUIRE_THROWS_AS(interp.evaluate(), SemanticError);
  }
}

TEST_CASE("Test Interpreter result with simple procedures (ln)", "[interpreter]") {

  { // natural log
    std::string program = "(ln e)";
    INFO(program);
    Expression result = run(program);
    REQUIRE(result == Expression(Atom(1.)));
  }
  {
    std::string input = R"(
(ln "hi")
)";

    Interpreter interp;

    std::istringstream iss(input);

    bool ok = interp.parseStream(iss);
    REQUIRE(ok);
    REQUIRE_THROWS_AS(interp.evaluate(), SemanticError);
  }
}

TEST_CASE("Test Interpreter result with simple procedures (sin)", "[interpreter]") {

  { // Sin
    std::string program = "(sin (/ pi 2))";
    INFO(program);
    Expression result = run(program);
    REQUIRE(result == Expression(Atom(1.)));
  }
  {
    std::string input = R"(
(sin "hi")
)";

    Interpreter interp;

    std::istringstream iss(input);

    bool ok = interp.parseStream(iss);
    REQUIRE(ok);
    REQUIRE_THROWS_AS(interp.evaluate(), SemanticError);
  }
}

TEST_CASE("Test Interpreter result with simple procedures (cos)", "[interpreter]") {

  { // cos
    std::string program = "(cos (/ pi 2))";
    INFO(program);
    Expression result = run(program);
    REQUIRE(result == Expression(Atom(0.)));
  }
  {
    std::string input = R"(
(cos "hi")
)";

    Interpreter interp;

    std::istringstream iss(input);

    bool ok = interp.parseStream(iss);
    REQUIRE(ok);
    REQUIRE_THROWS_AS(interp.evaluate(), SemanticError);
  }
}

TEST_CASE("Test Interpreter result with simple procedures (tan)", "[interpreter]") {

  { // tan
    std::string program = "(tan 0)";
    INFO(program);
    Expression result = run(program);
    REQUIRE(result == Expression(Atom(0.)));
  }
  {
    std::string input = R"(
(tan "hi")
)";

    Interpreter interp;

    std::istringstream iss(input);

    bool ok = interp.parseStream(iss);
    REQUIRE(ok);
    REQUIRE_THROWS_AS(interp.evaluate(), SemanticError);
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

TEST_CASE("Test Interpreter with lambda Expression (shadowing)", "[interpreter]") {

  std::string
      program = "(begin (define a 1) (define x 100) (define f (lambda (x) (begin (define b 12) (+ a b x)))) (f 2))";
  std::istringstream iss(program);

  Interpreter interp;

  REQUIRE(interp.parseStream(iss));
  INFO(program);
  Expression result = run(program);
  REQUIRE(result == Expression(15.));

}

TEST_CASE("Test Interpreter with apply", "[interpreter]") {
  { // simple apply function
    std::string program = "(apply + (list 1 2 3 4))";
    INFO(program);
    Expression result = run(program);
    REQUIRE(result == Expression(10));
  }
  { // simple complex apply function
    std::string
        program =
        "(begin (define complexAsList (lambda (x) (list (real x) (imag x)))) (apply complexAsList (list (+ 1 (* 3 I)))))";
    INFO(program);
    Expression result = run(program);
    Expression expected;
    expected.getTail().emplace_back(Expression(1));
    expected.getTail().emplace_back(Expression(3));
    REQUIRE(result == expected);
  }
  { // complex apply function
    std::string program = "(begin (define linear (lambda (a b x) (+ (* a x) b))) (apply linear (list 3 4 5)))";
    INFO(program);
    Expression result = run(program);
    REQUIRE(result == Expression(19));
  }
  { // Error: second argument to apply not a list
    std::string input = R"(
  (apply + 3)
)";
    Interpreter interp;
    std::istringstream iss(input);
    bool ok = interp.parseStream(iss);
    REQUIRE(ok);
    REQUIRE_THROWS_AS(interp.evaluate(), SemanticError);
  }
  { // Error: first argument to apply not a procedure
    std::string input = R"(
  (apply (+ z I) (list 0))
)";
    Interpreter interp;
    std::istringstream iss(input);
    bool ok = interp.parseStream(iss);
    REQUIRE(ok);
    REQUIRE_THROWS_AS(interp.evaluate(), SemanticError);
  }
  { // Error: during apply: Error in call to division: invalid number of arguments.
    std::string input = R"(
  (apply / (list 1 2 4))
)";
    Interpreter interp;
    std::istringstream iss(input);
    bool ok = interp.parseStream(iss);
    REQUIRE(ok);
    REQUIRE_THROWS_AS(interp.evaluate(), SemanticError);
  }

}

TEST_CASE("Test Interpreter with map", "[interpreter]") {
  { // simple apply function
    std::string program = "(map / (list 1 2 4))";
    INFO(program);
    Expression result = run(program);
    Expression expected;
    expected.getTail().emplace_back(Expression(1));
    expected.getTail().emplace_back(Expression(0.5));
    expected.getTail().emplace_back(Expression(0.25));
    REQUIRE(result == expected);
  }
  { // simple complex map function
    std::string
        program =
        "(begin (define f (lambda (x) (sin x))) (map f (list (- pi) (/ (- pi) 2) 0 (/ pi 2) pi)))";
    INFO(program);
    Expression result = run(program);
    Expression expected;
    expected.getTail().emplace_back(Expression(0));
    expected.getTail().emplace_back(Expression(-1));
    expected.getTail().emplace_back(Expression(0));
    expected.getTail().emplace_back(Expression(1));
    expected.getTail().emplace_back(Expression(0));
    REQUIRE(result == expected);
  }
  { // Error: second argument to map not a list
    std::string input = R"(
  (map + 3)
)";
    Interpreter interp;
    std::istringstream iss(input);
    bool ok = interp.parseStream(iss);
    REQUIRE(ok);
    REQUIRE_THROWS_AS(interp.evaluate(), SemanticError);
  }
  { // Error: first argument to map not a procedure
    std::string input = R"(
  (map 3 (list 1 2 3))
)";
    Interpreter interp;
    std::istringstream iss(input);
    bool ok = interp.parseStream(iss);
    REQUIRE(ok);
    REQUIRE_THROWS_AS(interp.evaluate(), SemanticError);
  }
  { // Error: during map: Error in call to division: invalid number of arguments.
    std::string input = R"(
  (begin (define addtwo (lambda (x y) (+ x y))) (map addtwo (list 1 2 3)))
)";
    Interpreter interp;
    std::istringstream iss(input);
    bool ok = interp.parseStream(iss);
    REQUIRE(ok);
    REQUIRE_THROWS_AS(interp.evaluate(), SemanticError);
  }

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

/// Milestone 2 Tests
/// String Tests

TEST_CASE("Test string implemenation", "[interpreter]") {
  {
    std::string program = "(\"foo\")";
    INFO(program);
    Expression result = run(program);
    REQUIRE(result == Expression("foo", true));
  }
  {
    std::string program = "(\"a string with spaces\")";
    INFO(program);
    Expression result = run(program);
    REQUIRE(result == Expression("a string with spaces", true));
  }
}

/// Setting and getting property tests

TEST_CASE("Test Setting a property", "[interpreter]") {
  {
    std::string program = "(set-property \"number\" \"three\" (3))";
    INFO(program);
    Expression result = run(program);
    REQUIRE(result == Expression(3.));
  }
  {
    std::string program = "(set-property \"number\" (3) \"three\")";
    INFO(program);
    Expression result = run(program);
    REQUIRE(result == Expression("three", true));
  }
  {
    std::string program = "(set-property \"number\" (+ 1 2) \"three\")";
    INFO(program);
    Expression result = run(program);
    REQUIRE(result == Expression("three", true));
  }
  {
    std::string input = R"(
(set-property (+ 1 2) "number" "three")
)";

    Interpreter interp;

    std::istringstream iss(input);

    bool ok = interp.parseStream(iss);
    REQUIRE(ok);

    REQUIRE_THROWS_AS(interp.evaluate(), SemanticError);
  }
  {
    std::string input = R"(
(set-property (+ 1 2) "number" "three" "two")
)";

    Interpreter interp;

    std::istringstream iss(input);

    bool ok = interp.parseStream(iss);
    REQUIRE(ok);

    REQUIRE_THROWS_AS(interp.evaluate(), SemanticError);
  }
}

TEST_CASE("Test getting a property", "[interpreter]") {
  {
    std::string program =
        "(begin (define a (+ 1 I)) (define b (set-property \"note\" \"a complex number\" a)) (get-property \"note\" b))";
    INFO(program);
    Expression result = run(program);
    REQUIRE(result == Expression("a complex number", true));
  }
  {
    std::string program =
        "(begin (define a (+ 1 I)) (define b (set-property \"note\" \"a complex number\" a)) (get-property \"note\" b) (get-property \"foo\" b))";
    INFO(program);
    Expression result = run(program);
    REQUIRE(result == Expression());
  }
  {
    std::string input = R"(
(begin (define a (+ 1 I)) (define b (set-property \"note\" \"a complex number\" a)) (get-property \"foo\" b))
)";

    Interpreter interp;

    std::istringstream iss(input);

    bool ok = interp.parseStream(iss);
    REQUIRE(ok);

    REQUIRE_THROWS_AS(interp.evaluate(), SemanticError);
  }
  {
    std::string input = R"(
(get-property "note" "two" "Home")
)";

    Interpreter interp;

    std::istringstream iss(input);

    bool ok = interp.parseStream(iss);
    REQUIRE(ok);

    REQUIRE_THROWS_AS(interp.evaluate(), SemanticError);
  }
}

TEST_CASE("Test overwriting a property", "[interpreter]") {
  {
    std::string program =
        "(begin (define p1 (make-point 1 2)) (define p2 (set-property \"size\" 2 p1)) (get-property \"size\" p2) )";
    INFO(program);
    Expression result = run(program);
    REQUIRE(result == Expression(2.));
  }
}

/// Make-point, make-line, make-text Tests

TEST_CASE("Test make-point", "[interpreter]") {
  {
    std::string program = "(begin (define p1 (make-point 1 2)) (get-property \"size\" p1))";
    INFO(program);
    Expression result = run(program);
    REQUIRE(result == Expression(0.));
  }
  {
    std::string program = "(begin (define p1 (make-point 1 2)) (get-property \"object-name\" p1))";
    INFO(program);
    Expression result = run(program);
    REQUIRE(result == Expression("point", true));
  }
  {
    std::string program = "(make-point 0 0)";
    INFO(program);
    Expression result = run(program);
    REQUIRE(result.isPoint());
  }
}

TEST_CASE("Test make-line", "[interpreter]") {
  {
    std::string program =
        "(begin (define p1 (make-point 1 2)) (define p2 (make-point 0 1)) (define l1 (make-line p1 p2)) (get-property \"object-name\" l1))";
    INFO(program);
    Expression result = run(program);
    REQUIRE(result == Expression("line", true));
  }
  {
    std::string program =
        "(begin (define p1 (make-point 1 2)) (define p2 (make-point 0 1)) (define l1 (make-line p1 p2)) (get-property \"thickness\" l1))";
    INFO(program);
    Expression result = run(program);
    REQUIRE(result == Expression(1.));
  }
  {
    std::string program = "(make-line (make-point 2 2) (make-point 0 0))";
    INFO(program);
    Expression result = run(program);
    REQUIRE(result.isLine());
  }
}

TEST_CASE("Test make-text", "[interpreter]") {
  {
    std::string program =
        "(begin (define t1 (make-text \"something\")) (get-property \"object-name\" t1))";
    INFO(program);
    Expression result = run(program);
    REQUIRE(result == Expression("text", true));
  }
  {
    std::string program =
        "(begin (define t1 (make-text \"something\")) (get-property \"position\" t1))";
    INFO(program);
    Expression result = run(program);
    Expression t1;
    t1.getTail().emplace_back(Expression(0.));
    t1.getTail().emplace_back(Expression(0.));
    REQUIRE(result == t1);
  }
  {
    std::string program = "(make-text \"Hi\")";
    INFO(program);
    Expression result = run(program);
    REQUIRE(result.isText());
  }
}

/// Discrete Plot

TEST_CASE("Test descrete-plot", "[interpreter]") {
  {
    std::string program =
        "(begin (define f (lambda (x) (list x (+ (* 2 x) 1)))) (discrete-plot (map f (range -2 2 0.5)) (list (list \"title\" \"The Data\") (list \"abscissa-label\" \"X Label\") (list \"ordinate-label\" \"Y Label\") (list \"text-scale\" 1))))";
    INFO(program);
    Expression result = run(program);

    REQUIRE(result.getTail().size() == 31);
  }
  {
    std::string program =
        "(begin (define f (lambda (x) (list x (+ (* -5 x) 1)))) (discrete-plot (map f (range 6 10 0.5)) (list (list \"title\" \"The Data\") (list \"abscissa-label\" \"X Label\") (list \"ordinate-label\" \"Y Label\") (list \"text-scale\" 1))))";
    INFO(program);
    Expression result = run(program);

    REQUIRE(result.getTail().size() == 29);
  }
  {
    std::string program =
        "(begin (define f (lambda (x) (list x (+ (* 5 x) 1)))) (discrete-plot (map f (range 6 10 0.5)) (list (list \"title\" \"The Data\") (list \"abscissa-label\" \"X Label\") (list \"ordinate-label\" \"Y Label\") (list \"text-scale\" 1))))";
    INFO(program);
    Expression result = run(program);

    REQUIRE(result.getTail().size() == 29);
  }
}

TEST_CASE("Test continuous-plot", "[interpreter]") {
  {
    std::string program =
        "(begin\n"
        "    (define f (lambda (x)\n"
        "        (+ (* 2 x) 1)))\n"
        "    (continuous-plot f (list -2 2)\n"
        "        (list\n"
        "        (list \"title\" \"A continuous linear function\")\n"
        "        (list \"abscissa-label\" \"x\")\n"
        "        (list \"ordinate-label\" \"y\")"
        "        (list \"text-scale\" .1))))";
    INFO(program);
    Expression result = run(program);
    REQUIRE(result.getTail().size() == 63);
  }
  {
    std::string program = "(begin (define f (lambda (x) (sin x))) (continuous-plot f (list (- pi) pi)))";
    INFO(program);
    Expression result = run(program);
    REQUIRE(result.getTail().size() == 82);
  }
}

