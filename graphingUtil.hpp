//
// Created by kishanp on 11/15/18.
//

#ifndef PLOTSCRIPT_GRAPHINGUTIL_HPP
#define PLOTSCRIPT_GRAPHINGUTIL_HPP

#include <vector>
#include <string>
#include <sstream>
#include "expression.hpp"

class graphingUtil {

public:

  graphingUtil() = default;

  void setPositions(const std::vector<Expression> &points);

  void setScaleFactor();

  void scalePositions();

private:
  std::vector<double> m_xPositions;
  std::vector<double> m_yPositions;

  double m_textScale = 1;
  double m_yMax = 0, m_xMax = 0, m_xMin = 0, m_yMin = 0, m_xScaleFactor = 1, m_yScaleFactor = 1;

  std::string m_title, m_abscLabel, m_ordLabel;

  std::stringstream m_ss;
};

#endif //PLOTSCRIPT_GRAPHINGUTIL_HPP
