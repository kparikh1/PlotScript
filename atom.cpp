#include "atom.hpp"

#include <sstream>
#include <cctype>
#include <cmath>
#include <limits>

Atom::Atom() : m_type(NoneKind) {}

Atom::Atom(double value) {

  setNumber(value);
}

Atom::Atom(std::complex<double> comp) {

  setComplex(comp);
}

Atom::Atom(const Token &token) : Atom() {

  // is token a number?
  double temp;
  std::istringstream iss(token.asString());
  if (iss >> temp) {
    // check for trailing characters if >> succeeds
    if (iss.rdbuf()->in_avail() == 0) {
      setNumber(temp);
    }
  } else { // else assume symbol
    // make sure does not start with number
    if (!std::isdigit(token.asString()[0])) {
      setSymbol(token.asString());
    }
  }
}

Atom::Atom(const Token &token, bool &string) : Atom() {

  // is token a number?
  double temp;
  std::istringstream iss(token.asString());
  if (iss >> temp) {
    // check for trailing characters if >> succeeds
    if (iss.rdbuf()->in_avail() == 0) {
      setNumber(temp);
    }
  } else if (string) {
    setString(token.asString());
    string = false;
  } else { // else assume symbol
    // make sure does not start with number
    if (!std::isdigit(token.asString()[0])) {
      setSymbol(token.asString());
    }
  }
}

Atom::Atom(const std::string &value) : Atom() {

  setSymbol(value);
}

Atom::Atom(const std::string &value, const bool &string) {
  if (string)
    setString(value);
  else
    setError(value);
}

Atom::Atom(const Atom &x) : Atom() {
  if (x.isNumber()) {
    setNumber(x.complexValue.real());
  } else if (x.isSymbol()) {
    setSymbol(x.stringValue);
  } else if (x.isComplex()) {
    setComplex(x.complexValue);
  } else if (x.isString()) {
    setString(stringValue);
  } else if (x.isError()) {
    setError(stringValue);
  }
}

Atom &Atom::operator=(const Atom &x) {

  if (this != &x) {
    if (x.m_type == NoneKind) {
      setNone();
    } else if (x.m_type == NumberKind) {
      setNumber(x.complexValue.real());
    } else if (x.m_type == SymbolKind) {
      setSymbol(x.stringValue);
    } else if (x.m_type == ComplexKind) {
      setComplex(x.complexValue);
    } else if (x.m_type == StringKind)
      setString(x.stringValue);
    else if (x.m_type == ErrorKind)
      setError(x.stringValue);
  }
  return *this;
}

Atom::~Atom() {

// we need to ensure the destructor of the symbol string is called
  if (m_type == SymbolKind || m_type == StringKind || m_type == ErrorKind) {
    stringValue.~
        basic_string();
  }
}

void Atom::Clear() {
  setComplex(std::complex<double>(0, 0));
  setSymbol("");
  m_type = NoneKind;
};

bool Atom::isNone() const noexcept {
  return m_type == NoneKind;
}

bool Atom::isNumber() const noexcept {
  return m_type == NumberKind;
}

bool Atom::isComplex() const noexcept {
  return m_type == ComplexKind;
}

bool Atom::isNumCom() const noexcept {
  return isNumber() || isComplex();
}

bool Atom::isSymbol() const noexcept {
  return m_type == SymbolKind;
}

void Atom::setNumber(double value) {

  m_type = NumberKind;
  complexValue = value;
}

void Atom::setSymbol(const std::string &value) {

  // we need to ensure the destructor of the symbol string is called
  if (m_type == SymbolKind || m_type == StringKind || m_type == ErrorKind) {
    stringValue.~basic_string();
  }

  m_type = SymbolKind;

  // copy construct in place
  new(&stringValue) std::string(value);
}

void Atom::setComplex(const std::complex<double> &comp) {

  m_type = ComplexKind;
  complexValue = comp;
}

double Atom::asNumber() const noexcept {

  return (m_type == NumberKind) ? complexValue.real() : 0.0;
}

std::string Atom::asSymbol() const noexcept {

  std::string result;

  if (m_type == SymbolKind) {
    result = stringValue;
  }

  return result;
}

std::string Atom::asString() const noexcept {

  std::string result;

  if (m_type == StringKind) {
    result = stringValue;
  }

  return result;
}

std::string Atom::asError() const noexcept {

  std::string result;

  if (m_type == ErrorKind) {
    result = stringValue;
  }

  return result;
}

std::complex<double> Atom::asComplex() const noexcept {

  return isComplex() ? complexValue : std::complex<double>(0, 0);
}

std::complex<double> Atom::getComplex() const noexcept {
  return complexValue;
}

bool Atom::operator==(const Atom &right) const noexcept {

  if (m_type != right.m_type)
    return false;

  switch (m_type) {
    case NoneKind:
      if (right.m_type != NoneKind)
        return false;
      break;
    case NumberKind: {
      if (right.m_type != NumberKind)
        return false;
      if (Epsilon(complexValue.real(), right.complexValue.real()))
        return false;
    }
      break;
    case ComplexKind: {
      if (right.m_type != ComplexKind ||
          Epsilon(complexValue.real(), right.complexValue.real())
          || Epsilon(complexValue.imag(), right.complexValue.imag()))
        return false;
    }
      break;
    case SymbolKind: {
      if (right.m_type != SymbolKind)
        return false;
      return stringValue == right.stringValue;
    }
    case StringKind: {
      if (right.m_type != StringKind)
        return false;
      return stringValue == right.stringValue;
    }
      break;
    default:return false;
  }

  return true;
}
bool Atom::isString() const noexcept {
  return m_type == StringKind;
}

void Atom::setString(const std::string &value) {
  // we need to ensure the destructor of the symbol string is called
  if (m_type == StringKind || m_type == SymbolKind || m_type == ErrorKind) {
    stringValue.~basic_string();
  }

  m_type = StringKind;

  // copy construct in place
  new(&stringValue) std::string(value);
}

void Atom::setError(const std::string &value) {

  // we need to ensure the destructor of the symbol string is called
  if (m_type == SymbolKind || m_type == StringKind || m_type == ErrorKind) {
    stringValue.~basic_string();
  }

  m_type = ErrorKind;

  // copy construct in place
  new(&stringValue) std::string(value);
}

void Atom::setNone() {
  if (m_type == SymbolKind || m_type == StringKind)
    stringValue.~basic_string();
  m_type = NoneKind;
}

bool operator!=(const Atom &left, const Atom &right) noexcept {

  return !(left == right);
}

std::ostream &operator<<(std::ostream &out, const Atom &a) {

  if (a.isNumber()) {
    out << a.asNumber();
  }
  if (a.isSymbol()) {
    out << a.asSymbol();
  }
  if (a.isComplex()) {
    out << a.asComplex().real() << "," << a.asComplex().imag();
  }
  if (a.isString()) {
    out << "\"" << a.asString() << "\"";
  }
  if (a.isError()) {
    out << a.asError();
  }
  return out;
}

bool Epsilon(const double &dLeft, const double &dRight) {
  double diff = fabs(dLeft - dRight);
  return (std::isnan(diff) || (diff > (std::numeric_limits<double>::epsilon() * 256)));
}
