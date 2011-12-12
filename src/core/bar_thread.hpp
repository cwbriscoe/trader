#ifndef BAR_THREAD_HPP
#define BAR_THREAD_HPP

#include "cb/thread.hpp"
#include "thread_interface.hpp"

namespace cb {

class BarThread : public Thread, public Requester, public Provider {
public:
  BarThread(Provider* router);
  virtual ~BarThread();

  void send(const RequestPtr tran);
  void recv(const ResultPtr tran);

protected:
  virtual void run();
    
private:
  void processSendQueue();
  void processRecvQueue();

  Provider* mpRouter;

  RequestQueue mSendQueue;
  ResultQueue mRecvQueue;

  Mutex mSendMutex;
  Mutex mRecvMutex;
};
}
#endif //BAR_THREAD_HPP
