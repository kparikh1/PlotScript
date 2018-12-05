//
// Created by kishanp on 10/27/18.
//
#include "notebook_app.hpp"

NotebookApp::NotebookApp() {

  startBtn = new ButtonWidget();
  startBtn->setObjectName("start");
  startBtn->setText("Start Kernel");

  stopBtn = new ButtonWidget();
  stopBtn->setObjectName("stop");
  stopBtn->setText("Stop Kernel");

  resetBtn = new ButtonWidget();
  resetBtn->setObjectName("reset");
  resetBtn->setText("Reset Kernel");

  interruptBtn = new ButtonWidget();
  interruptBtn->setObjectName("interrupt");
  interruptBtn->setText("Interrupt");

  auto btnLayout = new QHBoxLayout();
  btnLayout->addWidget(startBtn);
  btnLayout->addWidget(stopBtn);
  btnLayout->addWidget(resetBtn);
  btnLayout->addWidget(interruptBtn);

  input = new InputWidget();
  input->setObjectName("input");
  input->init();

  output = new OutputWidget();
  output->setObjectName("output");

  connect(input, &InputWidget::exceptionThrown, output, &OutputWidget::printText);
  connect(input, &InputWidget::sendResult, output, &OutputWidget::outputExpression);

  auto layout = new QVBoxLayout();

  layout->addLayout(btnLayout);
  layout->addWidget(input);
  layout->addWidget(output);

  setLayout(layout);

}
