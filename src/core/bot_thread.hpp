#ifndef BOT_THREAD_HPP
#define BOT_THREAD_HPP

#include "cb/thread.hpp"
#include "transactions.hpp"

namespace cb {

class RouterThread;

class BotThread : public Thread {
public:
  BotThread(RouterThread* ptr);
  virtual ~BotThread();

  void recv(const ResultPtr tran);

protected:
  void run();

  void requestTicker(const string& symbol); 
    
private:
  void processRecvQueue();


  RouterThread* mpRouter;

  RequestQueue mSendQueue;
  ResultQueue mRecvQueue;

  Mutex mSendMutex;
  Mutex mRecvMutex;
};
}
#endif //BOT_THREAD_HPP
