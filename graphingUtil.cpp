//
// Created by kishanp on 11/15/18.
//

#include "graphingUtil.hpp"
#include "semantic_error.hpp"

void graphingUtil::setPositions(const std::vector<Expression> &points) {

  bool init = false;
  for (auto point:points) {
    if (!point.isList())
      throw SemanticError("Error: Invalid list of plot-points to discrete-plot");
    m_xPositions.emplace_back(point.getTail().cbegin()->head().asNumber());
    m_yPositions.emplace_back((point.getTail().cbegin() + 1)->head().asNumber());
    if (!init) {
      m_yMax = (point.getTail().cbegin() + 1)->head().asNumber();
      m_yMin = (point.getTail().cbegin() + 1)->head().asNumber();
      m_xMax = point.getTail().cbegin()->head().asNumber();
      m_xMin = point.getTail().cbegin()->head().asNumber();
      init = true;
    }
    if (m_xMax < point.getTail().cbegin()->head().asNumber())
      m_xMax = point.getTail().cbegin()->head().asNumber();
    if (m_xMin > point.getTail().cbegin()->head().asNumber())
      m_xMin = point.getTail().cbegin()->head().asNumber();
    if (m_yMax < (point.getTail().cbegin() + 1)->head().asNumber())
      m_yMax = (point.getTail().cbegin() + 1)->head().asNumber();
    if (m_yMin > (point.getTail().cbegin() + 1)->head().asNumber())
      m_yMin = (point.getTail().cbegin() + 1)->head().asNumber();
  }

}
void graphingUtil::setScaleFactor() {

  m_yScaleFactor = 20 / (m_yMax - m_yMin);
  m_xScaleFactor = 20 / (m_xMax - m_xMin);
}
void graphingUtil::scalePositions() {

  for (std::size_t i = 0; i < m_xPositions.size(); i++) {
    m_xPositions.at(i) = m_xPositions.at(i) * m_xScaleFactor;
    m_yPositions.at(i) = m_yPositions.at(i) * -m_yScaleFactor;
  }

  m_yMax = -m_yMax;
  m_yMin = -m_yMin;

}
