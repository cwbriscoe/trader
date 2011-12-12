#ifndef EVENT_THREAD_HPP
#define EVENT_THREAD_HPP

#include "cb/thread.hpp"
#include "thread_interface.hpp"

namespace cb {

class EventThread : public Thread, public Requester {
public:
  EventThread();
  virtual ~EventThread();

  void recv(const ResultPtr tran);
  ResultPtr get();

protected:
  virtual void run();
    
private:
  void processRecvQueue();

  ResultQueue mRecvQueue;
  ResultQueue mSendQueue;
  Mutex mRecvMutex;
  Mutex mSendMutex;
};
}
#endif //EVENT_THREAD_HPP
