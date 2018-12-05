//
// Created by kishanp on 12/4/18.
//

#include "Consumer.hpp"
#include "catch.hpp"
#include <thread>

TEST_CASE("Test Constructor", "[consumer]") {

  MessageQueue<std::string> in;
  MessageQueue<Expression> out;
  Consumer worker(&in, &out, 1);

}

TEST_CASE("Test Run", "[consumer]") {

  MessageQueue<std::string> in;
  MessageQueue<Expression> out;
  Consumer worker(&in, &out, 1);

  std::thread th1(&Consumer::run, worker);

  in.push("(+ 1 2)");

  Expression result;
  out.wait_and_pop(result);

  in.push("%stop");
  th1.join();

  CHECK(result == Expression(3.));

}