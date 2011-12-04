#ifndef THREAD_INTERFACE_HPP
#define THREAD_INTERFACE_HPP

#include "transactions.hpp"

namespace cb {

class Requester {
public:
  Requester() {};
  virtual ~Requester() {};

  virtual void recv(const ResultPtr tran) = 0;
};

class Provider {
public:
  Provider() {};
  virtual ~Provider() {};

  virtual void send(const RequestPtr tran) = 0;
};

}
#endif //THREAD_INTERFACE_HPP
