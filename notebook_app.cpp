//
// Created by kishanp on 10/27/18.
//
#include "notebook_app.hpp"

NotebookApp::NotebookApp() {

  input = new InputWidget();
  input->setObjectName("input");

  output = new OutputWidget();
  output->setObjectName("output");

  connect(input, &InputWidget::exceptionThrown, output, &OutputWidget::printText);
  connect(input, &InputWidget::sendResult, output, &OutputWidget::outputExpression);

  auto layout = new QVBoxLayout();

  layout->addWidget(input);
  layout->addWidget(output);

  setLayout(layout);

}
