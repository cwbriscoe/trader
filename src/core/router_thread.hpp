#ifndef ROUTER_THREAD_HPP
#define ROUTER_THREAD_HPP

#include "cb/thread.hpp"
#include "thread_interface.hpp"

namespace cb {

class ClientThread;
class BarThread;
class BotThread;

typedef std::unique_ptr<BotThread> BotThreadPtr;

class RouterThread : public Thread, Requester, Provider {
public:
  RouterThread();
  virtual ~RouterThread();

  void send(const RequestPtr tran);
  void recv(const ResultPtr tran);

  void addBot(BotThread* bot);

protected:
  virtual void run();
    
private:
  void processSendQueue();
  void processRecvQueue();

  long mLastTickerId;

  BarThread* mpBarMaker;
  ClientThread* mpServer;
  BotThreadPtr mpBot;

  RequestQueue mSendQueue;
  ResultQueue mRecvQueue;

  Mutex mSendMutex;
  Mutex mRecvMutex;
};
}
#endif //ROUTER_THREAD_HPP
