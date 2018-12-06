//
// Created by kishanp on 10/27/18.
//
#include "notebook_app.hpp"

NotebookApp::NotebookApp() {

  interrupt = false;

  startBtn = new QPushButton();
  startBtn->setObjectName("start");
  startBtn->setText("Start Kernel");

  stopBtn = new QPushButton();
  stopBtn->setObjectName("stop");
  stopBtn->setText("Stop Kernel");

  resetBtn = new QPushButton();
  resetBtn->setObjectName("reset");
  resetBtn->setText("Reset Kernel");

  interruptBtn = new QPushButton();
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

  /// Button Signals
  connect(startBtn, &QPushButton::released, input, &InputWidget::startInterpreter);
  connect(stopBtn, &QPushButton::released, input, &InputWidget::stopInterpreter);
  connect(resetBtn, &QPushButton::released, input, &InputWidget::resetInterpreter);
  connect(interruptBtn, &QPushButton::released, input, &InputWidget::interruptInterpreter);

  auto layout = new QVBoxLayout();

  layout->addLayout(btnLayout);
  layout->addWidget(input);
  layout->addWidget(output);

  setLayout(layout);

}
