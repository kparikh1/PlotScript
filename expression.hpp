/*! \file expression.hpp
Defines the Expression type and assiciated functions.
 */
#ifndef EXPRESSION_HPP
#define EXPRESSION_HPP

#include <string>
#include <vector>
#include <map>
#include <atomic>

#include "token.hpp"
#include "atom.hpp"

// forward declare Environment
class Environment;

/*! \class Expression
\brief An expression is a tree of Atoms.

An expression is an atom called the head followed by a (possibly empty) 
list of expressions called the tail.
 */
class Expression {
 public:

  typedef std::vector<Expression>::const_iterator ConstIteratorType;

  /// Default construct and Expression, whose type in NoneType
  Expression() = default;

  /*! Construct an Expression with given Atom as head an empty tail
    \param atom the atom to make the head
  */
  explicit Expression(const Atom &a);

  /// deep-copy construct an expression (recursive)
  Expression(const Expression &a);

  /// Construct Expression with double
  explicit Expression(const double &value);

  /// Construct Expression with symbol
  explicit Expression(const std::string &value);

  /// Construct Expression with string value
  explicit Expression(const std::string &value, const bool &string);

  /// Construct Expression with complex
  explicit Expression(const std::complex<double> &value);

  /// Construct Expression as point
  explicit Expression(const double &x, const double &y, const double &size);

  /// Construct Expression as Line
  explicit Expression(const double &x1, const double &y1, const double &x2, const double &y2, const double &thickness);

  /// Construct Expression as text
  explicit Expression(const std::string &text,
                      const double &x,
                      const double &y,
                      const double &scale,
                      const double &rotation);

  /// deep-copy assign an expression  (recursive)
  Expression &operator=(const Expression &a);

  /// return a reference to the head Atom
  Atom &head();

  /// return a const-reference to the head Atom
  const Atom &head() const;

  /// append Atom to tail of the expression
  void append(const Atom &a);

  /// return a pointer to the last expression in the tail, or nullptr
  Expression *tail();

  /// return the tail
  const std::vector<Expression> &getTail() const;

  /// return the tail
  std::vector<Expression> &getTail();

  /// return a const-iterator to the beginning of tail
  ConstIteratorType tailConstBegin() const noexcept;

  /// return a const-iterator to the tail end
  ConstIteratorType tailConstEnd() const noexcept;

  /// convienience member to determine if head atom is a number
  bool isHeadNumber() const noexcept;

  /// convienience member to determine if head atom is a complex
  bool isHeadComplex() const noexcept;

  /// convienience member to determine if head atom is a symbol
  bool isHeadSymbol() const noexcept;

  /// convienience number to determine if head atom is a number or complex
  bool isHeadNumCom() const noexcept;

  /// convienience member to determine if the expression is a list type
  bool isList() const noexcept;

  /// convienience member to determine if the expression is a lambda type
  bool isLambda() const noexcept;

  /// convienience member to determine if the expression is a point
  bool isPoint() const noexcept;

  /// convienience member to determine if the expression is a line
  bool isLine() const noexcept;

  /// convienience member to determine if the expression is a text
  bool isText() const noexcept;

  /// Add a property to Expression
  void addProperty(const std::string &key, const Expression &value);

  /// Get value of a property from Expression
  Expression getProperty(std::string key) const;

  /// Evaluate expression using a post-order traversal (recursive)
  Expression eval(Environment &env);

  /// equality comparison for two expressions (recursive)
  bool operator==(const Expression &exp) const noexcept;

 private:

  // the head of the expression
  Atom m_head;

  bool m_Lambda = false;

  // the tail list is expressed as a vector for access efficiency
  // and cache coherence, at the cost of wasted memory.
  std::vector<Expression> m_tail;

  // List of Properties
  std::map<std::string, Expression> m_properties;

  // convenience typedef
  typedef std::vector<Expression>::iterator IteratorType;

  // internal helper methods
  Expression handle_lookup(const Atom &head, const Environment &env);
  Expression handle_define(Environment &env);
  Expression handle_begin(Environment &env);
  Expression handle_list(Environment &env);
  Expression handle_lambda();
  Expression handle_apply(Environment &env);
  Expression handle_map(Environment &env);
  Expression handle_continuousPlot(Environment &env);
};

/// Render expression to output stream
std::ostream &operator<<(std::ostream &out, const Expression &exp);

/// inequality comparison for two expressions (recursive)
bool operator!=(const Expression &left, const Expression &right) noexcept;

/// Creates the scalefactor for the graphs and gets their Min and Max
double scaleFactor(const std::vector<double> &positions, double &max, double &min);

#endif
