#include "expression.hpp"

#include <sstream>
#include <list>

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
}

Expression::Expression(const double &value) {
  m_head = Atom(value);
}

Expression::Expression(const std::string &value) {
  m_head = Atom(value);
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
  } else {
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
  catch (SemanticError error) {
    std::string errorName = "Error during apply: ";
    errorName.append(error.what());
    throw SemanticError(errorName);
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

bool operator!=(const Expression &left, const Expression &right) noexcept {

  return !(left == right);
}
