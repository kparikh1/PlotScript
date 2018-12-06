//
// Created by kishanp on 12/3/18.
//

#ifndef PLOTSCRIPT_CONSUMER_HPP
#define PLOTSCRIPT_CONSUMER_HPP

#include "MessageQueue.hpp"
#include "expression.hpp"
#include <string>

// define the mailbox as out thread-safe message queue
typedef MessageQueue<std::string> IncomingMail;
typedef MessageQueue<Expression> OutgoingMail;

class Consumer {

 public:

  Consumer() = default;
  Consumer(IncomingMail *i_MB, OutgoingMail *o_MB, std::size_t ConsumerID)
      : incomingMB(i_MB), outgoingMB(o_MB), id(ConsumerID) {}

  void run();

 private:

  IncomingMail *incomingMB;
  OutgoingMail *outgoingMB;
  std::size_t id;
};

#endif //PLOTSCRIPT_CONSUMER_HPP
