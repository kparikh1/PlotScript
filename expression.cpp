#include "expression.hpp"

#include <sstream>
#include <list>
#include <string>
#include <iomanip>

#include "environment.hpp"
#include "semantic_error.hpp"

Expression::Expression(const Atom &a) {

  m_head = a;
}

// recursive copy
Expression::Expression(const Expression &a) {

  m_head = a.m_head;
  for (auto e : a.m_tail) {
    m_tail.emplace_back(e);
  }
  m_Lambda = a.m_Lambda;
  m_properties = a.m_properties;
}

Expression::Expression(const double &value) {
  m_head = Atom(value);
}

Expression::Expression(const std::string &value) {
  m_head = Atom(value);
}

Expression::Expression(const std::string &value, const bool &error) {
  m_head = Atom(value, error);
}

Expression::Expression(const std::complex<double> &value) {
  m_head = Atom(value);
}

Expression &Expression::operator=(const Expression &a) {

  // prevent self-assignment
  if (this != &a) {
    m_head = a.m_head;
    m_tail.clear();
    for (auto e : a.m_tail) {
      m_tail.emplace_back(e);
    }
  }
  m_Lambda = a.m_Lambda;
  m_properties = a.m_properties;

  return *this;
}

Atom &Expression::head() {
  return m_head;
}

const Atom &Expression::head() const {
  return m_head;
}

bool Expression::isHeadNumber() const noexcept {
  return m_head.isNumber();
}

bool Expression::isHeadComplex() const noexcept {
  return m_head.isComplex();
}

bool Expression::isHeadSymbol() const noexcept {
  return m_head.isSymbol();
}

bool Expression::isHeadNumCom() const noexcept {
  return m_head.isNumCom();
}

bool Expression::isList() const noexcept {
  return (m_head.isSymbol() && m_head.asSymbol().empty()) || !m_tail.empty();
}

bool Expression::isLambda() const noexcept {
  return m_Lambda;
}

void Expression::addProperty(const std::string &key, const Expression &value) {
  auto exists = m_properties.find(key);
  if (exists == m_properties.end())
    m_properties.emplace(key, value);
  else {
    m_properties.erase(exists);
    m_properties.emplace(key, value);
  }
}

Expression Expression::getProperty(std::string key) const {
  auto value = m_properties.find(key);
  if (value != m_properties.end())
    return value->second;
  else
    return Expression();
}

void Expression::append(const Atom &a) {
  m_tail.emplace_back(a);
}

Expression *Expression::tail() {
  Expression *ptr = nullptr;

  if (!m_tail.empty()) {
    ptr = &m_tail.back();
  }

  return ptr;
}

const std::vector<Expression> &Expression::getTail() const {
  return m_tail;
}

std::vector<Expression> &Expression::getTail() {
  return m_tail;
}

Expression::ConstIteratorType Expression::tailConstBegin() const noexcept {
  return m_tail.cbegin();
}

Expression::ConstIteratorType Expression::tailConstEnd() const noexcept {
  return m_tail.cend();
}

Expression lambda(const std::vector<Expression> &args, const Environment &env) {

  Expression lambda = *(args.cend() - 1);
  Environment dummyEnv(env);
  auto it = args.cbegin();
  for (auto a:lambda.getTail().cbegin()->getTail()) {
    if (!env.is_exp(a.head())) {
      dummyEnv.add_exp(a.head(), *it);
    } else {
      dummyEnv.rem_exp(a.head());
      dummyEnv.add_exp(a.head(), *it);
    }
    ++it;
  }

  return (lambda.getTail().end() - 1)->eval(dummyEnv);

}

Expression apply(const Atom &op, const std::vector<Expression> &args, const Environment &env) {

  // Return if it is a string
  if (op.isString())
    return Expression(op);

  // head must be a symbol
  if (!op.isSymbol()) {
    throw SemanticError("Error during evaluation: procedure name not symbol");
  }

  // must map to a proc or lambda
  if (!env.is_proc(op) && !env.is_lambda(op)) {
    throw SemanticError("Error during evaluation: symbol does not name a procedure");
  }

  if (env.is_proc(op)) {
    // map from symbol to proc
    Procedure proc = env.get_proc(op);
    // call proc with args
    return proc(args);
  } else {
    std::vector<Expression> args_Copy = args;
    args_Copy.push_back(env.get_lambda(op));
    return lambda(args_Copy, env);
  }
}

Expression Expression::handle_lookup(const Atom &head, const Environment &env) {
  if (head.isSymbol()) { // if symbol is in env return value
    if (env.is_exp(head)) {
      return env.get_exp(head);
    } else if (env.is_lambda(head)) {
      return env.get_lambda(head);
    } else {
      throw SemanticError("Error during evaluation: unknown symbol");
    }
  } else if (head.isNumber()) {
    return Expression(head);
  } else if (head.isString())
    return Expression(head);
  else {
    throw SemanticError("Error during evaluation: Invalid type in terminal expression");
  }
}

Expression Expression::handle_begin(Environment &env) {

  if (m_tail.empty()) {
    throw SemanticError("Error during evaluation: zero arguments to begin");
  }

  // evaluate each arg from tail, return the last
  Expression result;
  for (auto it:m_tail) {
    result = it.eval(env);
  }

  return result;
}

Expression Expression::handle_define(Environment &env) {

  // tail must have size 3 or error
  if (m_tail.size() != 2) {
    throw SemanticError("Error during evaluation: invalid number of arguments to define");
  }

  // tail[0] must be symbol
  if (!m_tail[0].isHeadSymbol()) {
    throw SemanticError("Error during evaluation: first argument to define not symbol");
  }

  // but tail[0] must not be a special-form or procedure
  std::string s = m_tail[0].head().asSymbol();
  if ((s == "define") || (s == "begin")) {
    throw SemanticError("Error during evaluation: attempt to redefine a special-form");
  }

  if (env.is_proc(m_head)) {
    throw SemanticError("Error during evaluation: attempt to redefine a built-in procedure");
  }

  // eval tail[1]
  Expression result = m_tail[1].eval(env);

  if (env.is_exp(m_head)) {
    throw SemanticError("Error during evaluation: attempt to redefine a previously defined symbol");
  }

  //and add to env
  env.add_exp(m_tail[0].head(), result);

  return result;
}

Expression Expression::handle_list(Environment &env) {

  Expression result;
  for (auto it:m_tail) {
    result.m_tail.push_back(it.eval(env));
  }

  return result;
}

Expression Expression::handle_lambda() {

  if (m_tail.size() != 2)
    throw SemanticError("Error: Invalid number of arguments to Lambda");
  for (const auto &a:m_tail)
    if (!a.isList() && !a.isHeadSymbol())
      throw SemanticError("Error: Invalid type of argument to Lambda");
  for (const auto &a:m_tail.cbegin()->m_tail)
    if (!a.isHeadSymbol())
      throw SemanticError("Error: Invalid variable definitions in Lambda");

  Expression result;
  m_tail.begin()->getTail().insert(m_tail.cbegin()->getTail().cbegin(), Expression(m_tail.cbegin()->head()));
  m_tail.begin()->head().Clear();
  for (auto a:m_tail)
    result.getTail().emplace_back(a);
  result.m_Lambda = true;

  return result;
}

Expression Expression::handle_apply(Environment &env) {
  if (m_tail.size() != 2)
    throw SemanticError("Error: Not enough Arguments to Apply");
  if ((!env.is_lambda(m_tail.cbegin()->head()) && !env.is_proc(m_tail.cbegin()->head())) || m_tail.cbegin()->isList())
    throw SemanticError("Error: First argument to apply not a procedure");
  if (!(m_tail.cbegin() + 1)->isList())
    throw SemanticError("Error: Second argument to apply not a list");

  Expression result = *m_tail.cbegin();
  for (auto &a:(m_tail.cbegin() + 1)->m_tail)
    result.m_tail.push_back(a);
  try {
    result = result.eval(env);
  }
  catch (SemanticError &error) {
    std::string errorName = "Error during apply: ";
    errorName.append(error.what());
    throw SemanticError(errorName);
  }

  return result;
}

Expression Expression::handle_map(Environment &env) {
  if (m_tail.size() != 2)
    throw SemanticError("Error: Not enough Arguments to map");
  if ((!env.is_lambda(m_tail.cbegin()->head()) && !env.is_proc(m_tail.cbegin()->head())) || m_tail.cbegin()->isList())
    throw SemanticError("Error: First argument to map not a procedure");
  if (!(m_tail.cbegin() + 1)->isList())
    throw SemanticError("Error: Second argument to map not a list");

  if ((m_tail.cbegin() + 1)->isHeadSymbol()
      && (env.is_proc((m_tail.cbegin() + 1)->head()) || env.is_known((m_tail.cbegin() + 1)->head()))) {
    Expression Evaluate = *(m_tail.cbegin() + 1);
    m_tail.pop_back();
    m_tail.emplace_back(Evaluate.eval(env));
  }

  Expression result;
  Expression entry = Expression(m_tail.cbegin()->head());
  for (auto a:(m_tail.cbegin() + 1)->getTail()) {
    entry.m_tail.emplace_back(a);
    try {
      result.m_tail.emplace_back(entry.eval(env));
    } catch (SemanticError &error) {
      std::string errorName = "Error during map: ";
      errorName.append(error.what());
      throw SemanticError(errorName);
    }
    entry.m_tail.clear();
  }
  return result;

}

Expression Expression::handle_continuousPlot(Environment &env) {

  if (m_tail.size() < 2)
    throw SemanticError("Error: Invalid number of parameters to continuous-plot");

  if (!(m_tail.cbegin() + 1)->isList())
    throw SemanticError("Error: Invalid type of argument to continuous-plot");

  /// Deconstruct Parameters
  Expression lambdaFunction = *m_tail.cbegin();
  Expression arguments = (m_tail.begin() + 1)->eval(env);

  /// Create Data
  double stepSize =
      (arguments.getTail().cbegin()->head().asNumber() - (arguments.getTail().cbegin() + 1)->head().asNumber()) / -50;
  arguments.getTail().emplace_back(Expression(stepSize));
  Expression xPositionsExpressions("range");
  xPositionsExpressions.getTail()
      .insert(xPositionsExpressions.getTail().end(), arguments.getTail().begin(), arguments.getTail().end());
  xPositionsExpressions = xPositionsExpressions.eval(env);

  if ((xPositionsExpressions.getTail().cend() - 1)->head().asNumber()
      != (arguments.getTail().cbegin() + 1)->head().asNumber())
    xPositionsExpressions.getTail().push_back(*(arguments.getTail().cbegin() + 1));

  Expression yPositionsExpressions("map");
  yPositionsExpressions.getTail().push_back(lambdaFunction);
  yPositionsExpressions.getTail().push_back(xPositionsExpressions);
  yPositionsExpressions = yPositionsExpressions.eval(env);

  /// Create a vector of x and y positions
  std::vector<double> xPositions;
  std::vector<double> yPositions;

  for (auto point:xPositionsExpressions.getTail()) {
    xPositions.emplace_back(point.head().asNumber());
  }
  for (auto point:yPositionsExpressions.getTail()) {
    yPositions.emplace_back(point.head().asNumber());
  }

  Expression result;

  /// Smooth Lines
  /// Algorithm is a, b, c where b is vertex.
  bool antiAliased = true;
  for (int8_t i = 0; i < 10 && antiAliased; i++) {
    bool addedPoint = false;
    antiAliased = false;
    for (std::size_t it = 0; it < xPositions.size() - 2; it++) {
      double angle = std::abs((std::atan2(
          *(yPositions.cbegin() + it + (addedPoint ? 3 : 2)) - *(yPositions.cbegin() + it + (addedPoint ? 2 : 1)),
          *(xPositions.cbegin() + it + (addedPoint ? 3 : 2)) - *(xPositions.cbegin() + it + (addedPoint ? 2 : 1)))
          - std::atan2(
              *(yPositions.cbegin() + it) - *(yPositions.cbegin() + it + (addedPoint ? 2 : 1)),
              *(xPositions.cbegin() + it)
                  - *(xPositions.cbegin() + it + (addedPoint ? 2 : 1)))) * -180 / std::atan2(0, -1));
      if (angle < 175 || angle > 185) {
        double mid1 = *(xPositions.cbegin() + it)
            + (*(xPositions.cbegin() + it) - *(xPositions.cbegin() + it + (addedPoint ? 2 : 1))) / 2;
        double mid2 =
            *(xPositions.cbegin() + it + (addedPoint ? 2 : 1)) - (*(xPositions.cbegin() + it + (addedPoint ? 3 : 2))
                - *(xPositions.cbegin() + it + (addedPoint ? 2 : 1))) / 2;

        if (!addedPoint) {
          lambdaFunction.getTail().clear();
          lambdaFunction.getTail().emplace_back(Expression(mid1));
          yPositions.emplace((yPositions.cbegin() + it), lambdaFunction.eval(env).head().asNumber());
          xPositions.emplace((xPositions.cbegin() + it), mid1);
        }
        it++;
        lambdaFunction.getTail().clear();
        lambdaFunction.getTail().emplace_back(Expression(mid2));
        yPositions.emplace((yPositions.cbegin() + it + 1), lambdaFunction.eval(env).head().asNumber());
        xPositions.emplace((xPositions.cbegin() + it + 1), mid2);
        addedPoint = true;
        antiAliased = true;
      } else {
        addedPoint = false;
      }
    }
  }

  /// Create Scale Factor
  double yMax, xMax, xMin, yMin;
  double xScaleFactor = scaleFactor(xPositions, xMax, xMin);
  double yScaleFactor = scaleFactor(yPositions, yMax, yMin);

  for (auto &x:xPositions) {
    x = x * xScaleFactor;
  }

  /// You must flip the y value
  for (auto &y:yPositions) {
    y = y * -yScaleFactor;
  }
  yMax = -yMax;
  yMin = -yMin;

  /// Create the graph border
  result.getTail().emplace_back(Expression(xMin, yMax, xMin, yMin, 0));
  result.getTail().emplace_back(Expression(xMax, yMax, xMax, yMin, 0));
  result.getTail().emplace_back(Expression(xMin, yMax, xMax, yMax, 0));
  result.getTail().emplace_back(Expression(xMin, yMin, xMax, yMin, 0));

  /// Find Y Axis
  if (xMax > 0 && xMin < 0)
    result.getTail().emplace_back(Expression(0, yMax, 0, yMin, 0));

  /// Find X Axis
  if (yMax < 0 && yMin > 0)
    result.getTail().emplace_back(Expression(xMax, 0, xMin, 0, 0));

  double textScale = 1;
  if (m_tail.size() == 3 && (m_tail.cbegin() + 2)->isList()) {
    Expression properties = (m_tail.begin() + 2)->eval(env);

    /// Get Properties
    std::string title, abscLabel, ordLabel;
    for (auto &option:properties.getTail()) {
      if (option.getTail().cbegin()->head().asString() == "title")
        title = (option.getTail().cbegin() + 1)->head().asString();
      else if (option.getTail().cbegin()->head().asString() == "abscissa-label")
        abscLabel = (option.getTail().cbegin() + 1)->head().asString();
      else if (option.getTail().cbegin()->head().asString() == "ordinate-label")
        ordLabel = (option.getTail().cbegin() + 1)->head().asString();
      else if (option.getTail().cbegin()->head().asString() == "text-scale")
        textScale = (option.getTail().cbegin() + 1)->head().asNumber();
    }

    /// Add Graph Labels
    result.getTail().emplace_back(Expression(title, xMax - ((xMax - xMin) / 2), (yMax - 3), textScale, 0));
    result.getTail().emplace_back(Expression(abscLabel, xMax - ((xMax - xMin) / 2), (yMin + 3), textScale, 0));
    result.getTail().emplace_back(Expression(ordLabel,
                                             (xMin - 3),
                                             yMin - (yMin - yMax) / 2,
                                             textScale,
                                             -std::atan2(0, -1) / 2));
  }
  /// Add Graph number labels
  std::stringstream ss;
  ss << std::setprecision(2);
  ss << yMax / -yScaleFactor;
  result.getTail().emplace_back(Expression(ss.str(), (xMin - 2), yMax, textScale, 0));
  ss.str("");
  ss << yMin / -yScaleFactor;
  result.getTail().emplace_back(Expression(ss.str(), (xMin - 2), yMin, textScale, 0));
  ss.str("");
  ss << xMax / xScaleFactor;
  result.getTail().emplace_back(Expression(ss.str(), xMax, (yMin + 2), textScale, 0));
  ss.str("");
  ss << xMin / xScaleFactor;
  result.getTail().emplace_back(Expression(ss.str(), xMin, (yMin + 2), textScale, 0));



  /// Add Lines
  for (std::size_t it = 0; it < xPositions.size() - 1; it++) {
    result.getTail().emplace_back(Expression(*(xPositions.cbegin() + it), *(yPositions.cbegin() + it),
                                             *(xPositions.cbegin() + it + 1), *(yPositions.cbegin() + it + 1), 0));
  }

  return result;

}

// this is a simple recursive version. the iterative version is more
// difficult with the ast data structure used (no parent pointer).
// this limits the practical depth of our AST
Expression Expression::eval(Environment &env) {

  if (m_tail.empty()) {
    return handle_lookup(m_head, env);
  }
    // handle begin special-form
  else if (m_head.isSymbol() && m_head.asSymbol() == "begin") {
    return handle_begin(env);
  }
    // handle define special-form
  else if (m_head.isSymbol() && m_head.asSymbol() == "define") {
    return handle_define(env);
  }
    // handle list special-form
  else if (m_head.isSymbol() && m_head.asSymbol() == "list") {
    return handle_list(env);
  }
    // handle lambda special-form
  else if (m_head.isSymbol() && m_head.asSymbol() == "lambda")
    return handle_lambda();
    // handle apply special-form
  else if (m_head.isSymbol() && m_head.asSymbol() == "apply")
    return handle_apply(env);
    // handle map special-form
  else if (m_head.isSymbol() && m_head.asSymbol() == "map")
    return handle_map(env);
  else if (m_head.isSymbol() && m_head.asSymbol() == "continuous-plot")
    return handle_continuousPlot(env);
    // else attempt to treat as procedure
  else {
    std::vector<Expression> results;
    for (auto it:m_tail) {
      results.push_back(it.eval(env));
    }
    return apply(m_head, results, env);
  }
}

std::ostream &operator<<(std::ostream &out, const Expression &exp) {

  if (exp.head().isError()) {
    out << exp.head();
    return out;
  }

  if (exp.head().isNone() && exp.getTail().empty()) {
    out << "NONE";
    return out;
  }
  out << "(";
  out << exp.head();

  if (!exp.head().isNone() && !exp.getTail().empty())
    out << " ";

  for (auto e = exp.tailConstBegin(); e != exp.tailConstEnd(); ++e) {
    out << *e;
    if (e + 1 != exp.tailConstEnd())
      out << " ";
  }

  out << ")";

  return out;
}

bool Expression::operator==(const Expression &exp) const noexcept {

  bool result = (m_head == exp.m_head);

  result = result && (m_tail.size() == exp.m_tail.size());

  if (result) {
    for (auto lefte = m_tail.begin(), righte = exp.m_tail.begin();
         (lefte != m_tail.end()) && (righte != exp.m_tail.end());
         ++lefte, ++righte) {
      result = result && (*lefte == *righte);
    }
  }

  return result;
}

bool Expression::isPoint() const noexcept {
  return this->getProperty("object-name") == Expression("point", true);
}
bool Expression::isLine() const noexcept {
  return this->getProperty("object-name") == Expression("line", true);
}
bool Expression::isText() const noexcept {
  return this->getProperty("object-name") == Expression("text", true);
}

Expression::Expression(const double &x, const double &y, const double &size) {
  m_tail.emplace_back(Expression(x));
  m_tail.emplace_back(Expression(y));

  addProperty("size", Expression(size));
  addProperty("object-name", Expression("point", true));
}

Expression::Expression(const double &x1,
                       const double &y1,
                       const double &x2,
                       const double &y2,
                       const double &thickness) {

  m_tail.emplace_back(Expression(x1, y1, 0));
  m_tail.emplace_back(Expression(x2, y2, 0));

  addProperty("thickness", Expression(thickness));
  addProperty("object-name", Expression("line", true));
}
Expression::Expression(const std::string &text,
                       const double &x,
                       const double &y,
                       const double &scale,
                       const double &rotation) {

  m_head = Atom(text, true);
  addProperty("object-name", Expression("text", true));
  addProperty("position", Expression(x, y, 0));
  addProperty("text-scale", Expression(scale));
  addProperty("text-rotation", Expression(rotation));
}

bool operator!=(const Expression &left, const Expression &right) noexcept {

  return !(left == right);
}

double scaleFactor(const std::vector<double> &positions, double &max, double &min) {

  max = *positions.cbegin();
  min = *positions.cbegin();
  for (auto &pos:positions) {
    if (max < pos)
      max = pos;
    if (min > pos)
      min = pos;
  }

  double scaleFact = 20 / (max - min);
  max *= scaleFact;
  min *= scaleFact;

  return scaleFact;
}

