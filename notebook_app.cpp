//
// Created by kishanp on 10/27/18.
//
#include "notebook_app.hpp"
notebook_app::notebook_app() {

  input = new input_widget();
  output = new output_widget();

  auto layout = new QVBoxLayout();

  layout->addWidget(input);
  layout->addWidget(output);

  setLayout(layout);

}
