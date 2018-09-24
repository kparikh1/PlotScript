#include "environment.hpp"

#include <cassert>
#include <complex>

#include "environment.hpp"
#include "semantic_error.hpp"

/*********************************************************************** 
Helper Functions
**********************************************************************/

// predicate, the number of args is nargs
bool nargs_equal(const std::vector<Expression> &args, unsigned nargs) {
  return args.size() == nargs;
}

/*********************************************************************** 
Each of the functions below have the signature that corresponds to the
typedef'd Procedure function pointer.
**********************************************************************/

// the default procedure always returns an expresison of type None
Expression default_proc(const std::vector<Expression> &args) {
  args.size(); // make compiler happy we used this parameter
  return Expression();
};

Expression add(const std::vector<Expression> &args) {

  // check all arguments are numbers, while adding
  std::complex<double> result(0, 0);
  bool complex = false;
  for (auto &a :args) {
    if (a.isHeadNumber()) {
      result += a.head().asNumber();
    } else if (a.isHeadComplex()) {
      result += a.head().asComplex();
      complex = true;
    } else {
      throw SemanticError("Error in call to add, argument not a number");
    }
  }
  return complex ? Expression(Atom(result)) : Expression(Atom(result.real()));
};

Expression mul(const std::vector<Expression> &args) {

  // check all aruments are numbers, while multiplying
  std::complex<double> result(1, 0);
  bool complex = false;
  for (auto &a :args) {
    if (a.isHeadNumber()) {
      result *= a.head().asNumber();
    } else if (a.isHeadComplex()) {
      result *= a.head().asComplex();
      complex = true;
    } else {
      throw SemanticError("Error in call to mul, argument not a number");
    }
  }

  return complex ? Expression(Atom(result)) : Expression(Atom(result.real()));
};

Expression subneg(const std::vector<Expression> &args) {

  std::complex<double> result;
  bool complex = false;
  // preconditions
  if (nargs_equal(args, 1)) {
    if (args[0].isHeadNumCom()) {
      result = -args[0].head().getComplex();
      complex = args[0].isHeadComplex();
    } else {
      throw SemanticError("Error in call to negate: invalid argument.");
    }
  } else if (nargs_equal(args, 2)) {
    if ((args[0].isHeadNumCom()) && (args[1].isHeadNumCom())) {
      result = args[0].head().getComplex() - args[1].head().getComplex();
      complex = args[0].isHeadComplex() || args[1].isHeadComplex();
    } else {
      throw SemanticError("Error in call to subtraction: invalid argument.");
    }
  } else {
    throw SemanticError("Error in call to subtraction or negation: invalid number of arguments.");
  }

  return complex ? Expression(Atom(result)) : Expression(Atom(result.real()));
};

Expression div(const std::vector<Expression> &args) {

  std::complex<double> result;
  bool complex = args[0].isHeadComplex() || args[1].isHeadComplex();
  if (nargs_equal(args, 2)) {
    if (args[0].isHeadNumCom() && args[1].isHeadNumCom()) {
      result = args[0].head().getComplex() / args[1].head().getComplex();
    } else {
      throw SemanticError("Error in call to division: invalid argument.");
    }
  } else {
    throw SemanticError("Error in call to division: invalid number of arguments.");
  }
  return complex ? Expression(Atom(result)) : Expression(Atom(result.real()));
};

Expression sqrt(const std::vector<Expression> &args) {

  // check if one argument
  if (args.size() != 1)
    throw SemanticError("Error: invalid number of arguments for sqrt");
  else if (args.cbegin()->isHeadNumCom()) {
    std::complex<double> comp = std::sqrt(args.cbegin()->head().getComplex());
    return comp.imag() == 0 ? Expression(Atom(comp.real())) : Expression(Atom(comp));
  } else
    throw SemanticError("Error in call to sqrt, argument not a number");
};

Expression pow(const std::vector<Expression> &args) {

  // Check if 2 args
  if (args.size() != 2)
    throw SemanticError("Error: invalid number of arguments for exponential");
  else if (args.cbegin()->isHeadNumCom() && args.at(1).isHeadNumCom()) {
    std::complex<double> comp = std::pow(args.cbegin()->head().getComplex(), args.at(1).head().getComplex());
    return (args.cbegin()->isHeadComplex() || args.at(1).isHeadComplex()) ? Expression(Atom(comp))
                                                                          : Expression(Atom(comp.real()));
  } else
    throw SemanticError("Error in call to exponent, argument not a number");
};

Expression ln(const std::vector<Expression> &args) {
  // Check if 1 args
  if (args.size() != 1)
    throw SemanticError("Error: invalid number of arguments for natural log");
  else if (args.cbegin()->isHeadNumCom()) {
    std::complex<double> comp = std::log(args.cbegin()->head().getComplex());
    return (args.cbegin()->isHeadComplex()) ? Expression(Atom(comp)) : Expression(Atom(comp.real()));
  } else
    throw SemanticError("Error in call to natural log, argument not a number");
};

Expression sin(const std::vector<Expression> &args) {
  // Check if 1 args
  if (args.size() != 1)
    throw SemanticError("Error: invalid number of arguments for sin");
  else if (args.cbegin()->isHeadNumCom()) {
    std::complex<double> comp = std::sin(args.cbegin()->head().getComplex());
    return (args.cbegin()->isHeadComplex()) ? Expression(Atom(comp)) : Expression(Atom(comp.real()));
  } else
    throw SemanticError("Error in call to sin, argument not a number");
};

Expression cos(const std::vector<Expression> &args) {
  // Check if 1 args
  if (args.size() != 1)
    throw SemanticError("Error: invalid number of arguments for cos");
  else if (args.cbegin()->isHeadNumCom()) {
    std::complex<double> comp = std::cos(args.cbegin()->head().getComplex());
    return (args.cbegin()->isHeadComplex()) ? Expression(Atom(comp)) : Expression(Atom(comp.real()));
  } else
    throw SemanticError("Error in call to cos, argument not a number");
};

Expression tan(const std::vector<Expression> &args) {
  // Check if 1 args
  if (args.size() != 1)
    throw SemanticError("Error: invalid number of arguments for tan");
  else if (args.cbegin()->isHeadNumCom()) {
    std::complex<double> comp = std::tan(args.cbegin()->head().getComplex());
    return (args.cbegin()->isHeadComplex()) ? Expression(Atom(comp)) : Expression(Atom(comp.real()));
  } else
    throw SemanticError("Error in call to tan, argument not a number");
};

Expression real(const std::vector<Expression> &args) {
  if (nargs_equal(args, 1)) {
    if (args.cbegin()->isHeadComplex()) {
      return Expression(Atom(args.cbegin()->head().getComplex().real()));
    } else {
      throw SemanticError("Error in call to real: Not a Complex.");
    }
  } else {
    throw SemanticError("Error in call to real: invalid number of arguments.");
  }
};

Expression imag(const std::vector<Expression> &args) {
  if (nargs_equal(args, 1)) {
    if (args.cbegin()->isHeadComplex()) {
      return Expression(Atom(args.cbegin()->head().getComplex().imag()));
    } else {
      throw SemanticError("Error in call to imag: Not a Complex.");
    }
  } else {
    throw SemanticError("Error in call to imag: invalid number of arguments.");
  }
};

Expression mag(const std::vector<Expression> &args) {
  if (nargs_equal(args, 1)) {
    if (args.cbegin()->isHeadComplex()) {
      return Expression(Atom(std::abs(args.cbegin()->head().getComplex())));
    } else {
      throw SemanticError("Error in call to mag: Not a Complex.");
    }
  } else {
    throw SemanticError("Error in call to mag: invalid number of arguments.");
  }
};

Expression arg(const std::vector<Expression> &args) {
  if (nargs_equal(args, 1)) {
    if (args.cbegin()->isHeadComplex()) {
      return Expression(Atom(std::arg(args.cbegin()->head().getComplex())));
    } else {
      throw SemanticError("Error in call to arg: Not a Complex.");
    }
  } else {
    throw SemanticError("Error in call to arg: invalid number of arguments.");
  }
};

Expression conj(const std::vector<Expression> &args) {
  if (nargs_equal(args, 1)) {
    if (args.cbegin()->isHeadComplex()) {
      return Expression(Atom(std::conj(args.cbegin()->head().getComplex())));
    } else {
      throw SemanticError("Error in call to arg: Not a Complex.");
    }
  } else {
    throw SemanticError("Error in call to arg: invalid number of arguments.");
  }
};

Expression first(const std::vector<Expression> &args) {
  if (nargs_equal(args, 1)) {
    if (args.cbegin()->head().isSymbol() && args.cbegin()->head().asSymbol().empty()) {
      throw SemanticError("Error: argument to first is an empty list");
    } else if (args.cbegin()->isHeadNumCom()) {
      throw SemanticError("Error: argument to first is not a list");
    } else {
      return *args.cbegin()->getTail().cbegin();
    }
  } else {
    throw SemanticError("Error: more than one argument in call to first");
  }
};

Expression rest(const std::vector<Expression> &args) {
  if (nargs_equal(args, 1)) {
    if (args.cbegin()->head().isSymbol() && args.cbegin()->head().asSymbol().empty()) {
      throw SemanticError("Error: argument to first is an empty list");
    } else if (args.cbegin()->isHeadNumCom()) {
      throw SemanticError("Error: argument to first is not a list");
    } else {
      Expression exp;
      for (auto e:args.cbegin()->getTail())
        if (e != *args.cbegin()->getTail().cbegin())
          exp.getTail().emplace_back(e);
      return exp;
    }
  } else {
    throw SemanticError("Error: more than one argument in call to rest");
  }
};

Expression length(const std::vector<Expression> &args) {
  if (nargs_equal(args, 1)) {
    if (args.cbegin()->isHeadNumCom()) {
      throw SemanticError("Error: argument to first is not a list");
    } else {
      return Expression(args.cbegin()->getTail().size());
    }
  } else {
    throw SemanticError("Error: more than one argument in call to length");
  }
};

Expression append(const std::vector<Expression> &args) {
  if (!args.cbegin()->isList()) {
    throw SemanticError("Error: First argument not list in append");
  } else {
    Expression value = *args.cbegin();
    for (auto it = args.cbegin() + 1; it != args.cend(); ++it)
      value.getTail().push_back(*it);
    return value;
  }
};

Expression join(const std::vector<Expression> &args) {
  Expression result = *args.cbegin();
  for (auto arg = args.cbegin() + 1; arg != args.cend(); ++arg) {
    if (!arg->isList())
      throw SemanticError("Error: Argument to join is not a list");
    for (auto value:arg->getTail()) {
      result.getTail().emplace_back(value);
    }
  }
  return result;
};

Expression range(const std::vector<Expression> &args) {
  if (nargs_equal(args, 3)) {
    for (auto &arg:args)
      if (!arg.isHeadNumber())
        throw SemanticError("Error: Argument in Range not a number or complex");
    if (args.cbegin()->head().asNumber() > (args.cbegin() + 1)->head().asNumber())
      throw SemanticError("Error: First argument is smaller than second in range");
    if (args.cend()->head().asNumber() <= 0)
      throw SemanticError("Error: negative or zero increment in range");
    Expression result;
    for (double i = args.cbegin()->head().asNumber(); i <= (args.cbegin() + 1)->head().asNumber();
         i = i + (args.cbegin() + 2)->head().asNumber())
      result.getTail().emplace_back(Expression(i));
    return result;
  } else {
    throw SemanticError("Error: Invalid number of arguments in Range");
  }
};

const double PI = std::atan2(0, -1);
const double EXP = std::exp(1);
const std::complex<double> I(0.0, 1.0);

Environment::Environment() {

  reset();
}

bool Environment::is_known(const Atom &sym) const {
  if (!sym.isSymbol())
    return false;

  return envmap.find(sym.asSymbol()) != envmap.end();
}

bool Environment::is_exp(const Atom &sym) const {
  if (!sym.isSymbol())
    return false;

  auto result = envmap.find(sym.asSymbol());
  return (result != envmap.end()) && (result->second.type == ExpressionType);
}

Expression Environment::get_exp(const Atom &sym) const {

  Expression
      exp;

  if (sym.isSymbol()) {
    auto result = envmap.find(sym.asSymbol());
    if ((result != envmap.end()) && (result->second.type == ExpressionType)) {
      exp = result->second.exp;
    }
  }

  return exp;
}

void Environment::add_exp(const Atom &sym, const Expression &exp) {

  if (!sym.isSymbol()) {
    throw SemanticError("Attempt to add non-symbol to environment");
  }

  // error if overwriting symbol map
  if (envmap.find(sym.asSymbol()) != envmap.end()) {
    throw SemanticError("Attempt to overwrite symbol in environemnt");
  }

  envmap.emplace(sym.asSymbol(), EnvResult(ExpressionType, exp));
}

bool Environment::is_proc(const Atom &sym) const {
  if (!sym.isSymbol())
    return false;

  auto result = envmap.find(sym.asSymbol());
  return (result != envmap.end()) && (result->second.type == ProcedureType);
}

Procedure Environment::get_proc(const Atom &sym) const {

  //Procedure proc = default_proc;

  if (sym.isSymbol()) {
    auto result = envmap.find(sym.asSymbol());
    if ((result != envmap.end()) && (result->second.type == ProcedureType)) {
      return result->second.proc;
    }
  }

  return default_proc;
}

/*
Reset the environment to the default state. First remove all entries and
then re-add the default ones.
 */
void Environment::reset() {

  envmap.clear();

  // Built-In value of pi
  envmap.emplace("pi", EnvResult(ExpressionType, Expression(Atom(PI))));

  // Built-In value of e
  envmap.emplace("e", EnvResult(ExpressionType, Expression(Atom(EXP))));

  // Built-In value of I
  envmap.emplace("I", EnvResult(ExpressionType, Expression(Atom(I))));

  // Built-In value of list;
  envmap.emplace("list", EnvResult(ExpressionType, Expression(Atom(""))));

  // Procedure: add;
  envmap.emplace("+", EnvResult(ProcedureType, add));

  // Procedure: subneg;
  envmap.emplace("-", EnvResult(ProcedureType, subneg));

  // Procedure: mul;
  envmap.emplace("*", EnvResult(ProcedureType, mul));

  // Procedure: div;
  envmap.emplace("/", EnvResult(ProcedureType, div));

  // Procedure: sqrt;
  envmap.emplace("sqrt", EnvResult(ProcedureType, sqrt));

  // Procedure: pow;
  envmap.emplace("^", EnvResult(ProcedureType, pow));

  // Procedure: ln;
  envmap.emplace("ln", EnvResult(ProcedureType, ln));

  // Procedure: sin;
  envmap.emplace("sin", EnvResult(ProcedureType, sin));

  // Procedure: cos;
  envmap.emplace("cos", EnvResult(ProcedureType, cos));

  // Procedure: tan;
  envmap.emplace("tan", EnvResult(ProcedureType, tan));

  // Procedure: real;
  envmap.emplace("real", EnvResult(ProcedureType, real));

  // Procedure: imag;
  envmap.emplace("imag", EnvResult(ProcedureType, imag));

  // Procedure: mag;
  envmap.emplace("mag", EnvResult(ProcedureType, mag));

  // Procedure: arg;
  envmap.emplace("arg", EnvResult(ProcedureType, arg));

  // Procedure: conj;
  envmap.emplace("conj", EnvResult(ProcedureType, conj));

  // Procedure: first;
  envmap.emplace("first", EnvResult(ProcedureType, first));

  // Procedure: rest;
  envmap.emplace("rest", EnvResult(ProcedureType, rest));

  // Procedure: length;
  envmap.emplace("length", EnvResult(ProcedureType, length));

  // Procedure: append;
  envmap.emplace("append", EnvResult(ProcedureType, append));

  // Procedure: join;
  envmap.emplace("join", EnvResult(ProcedureType, join));

  // Procedure: range;
  envmap.emplace("range", EnvResult(ProcedureType, range));
}
