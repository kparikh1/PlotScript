//
// Created by kishanp on 12/3/18.
//

#ifndef PLOTSCRIPT_CONSUMER_HPP
#define PLOTSCRIPT_CONSUMER_HPP

#include "MessageQueue.hpp"
#include "expression.hpp"
#include <string>
#include <istream>

// define the mailbox as out thread-safe message queue
typedef MessageQueue<std::istringstream> IncomingMail;
typedef MessageQueue<Expression> OutgoingMail;

class Consumer {

 public:

  Consumer() = default;
  Consumer(IncomingMail *i_MB, OutgoingMail *o_MB, std::size_t ConsumerID)
      : incomingMB(i_MB), outgoingMB(o_MB), id(ConsumerID) {}

  void run(bool ContinueRun);

 private:
  std::size_t id;
  IncomingMail *incomingMB;
  OutgoingMail *outgoingMB;
};

#endif //PLOTSCRIPT_CONSUMER_HPP
