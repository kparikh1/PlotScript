#include "token.hpp"
#include "semantic_error.hpp"

// system includes
#include <cctype>
#include <iostream>

// define constants for special characters
const char OPENCHAR = '(';
const char CLOSECHAR = ')';
const char COMMENTCHAR = ';';
const char STRINGCHAR = '\"';

Token::Token(TokenType t) : m_type(t) {}

Token::Token(const std::string &str) : m_type(STRING), value(str) {}

Token::TokenType Token::type() const {
  return m_type;
}

std::string Token::asString() const {
  switch (m_type) {
    case OPEN:return "(";
    case CLOSE:return ")";
    case STRING:return value;
  }
  return "";
}

// add token to sequence unless it is empty, clears token
void store_ifnot_empty(std::string &token, TokenSequenceType &seq) {
  if (!token.empty()) {
    seq.emplace_back(token);
    token.clear();
  }
}

TokenSequenceType tokenize(std::istream &seq) {
  TokenSequenceType tokens;
  std::string token;

  while (true) {
    char c = (char) seq.get();
    if (seq.eof()) break;

    if (c == COMMENTCHAR) {
      // chomp until the end of the line
      while ((!seq.eof()) && (c != '\n')) {
        c = (char) seq.get();
      }
      if (seq.eof()) break;
    } else if (c == STRINGCHAR) {
      store_ifnot_empty(token, tokens);
      tokens.push_back(Token::TokenType::STRINGOPEN);
      while (true) {
        c = (char) seq.get();
        if (c == STRINGCHAR) {
          store_ifnot_empty(token, tokens);
          tokens.push_back(Token::TokenType::STRINGCLOSE);
          break;
        } else if (seq.eof())
          throw SemanticError("Error: No end of string character");
        else
          token.push_back(c);
      }
    } else if (c == OPENCHAR) {
      store_ifnot_empty(token, tokens);
      tokens.push_back(Token::TokenType::OPEN);
    } else if (c == CLOSECHAR) {
      store_ifnot_empty(token, tokens);
      tokens.push_back(Token::TokenType::CLOSE);
    } else if (isspace(c)) {
      store_ifnot_empty(token, tokens);
    } else {
      token.push_back(c);
    }
  }
  store_ifnot_empty(token, tokens);

  return tokens;
}
