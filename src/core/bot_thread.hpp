#ifndef BOT_THREAD_HPP
#define BOT_THREAD_HPP

#include "cb/thread.hpp"
#include "thread_interface.hpp"

namespace cb {

class BotThread : public Thread, Requester {
public:
  BotThread(Provider* ptr);
  virtual ~BotThread();

  void recv(const ResultPtr tran);

protected:
  void run();

  void sendRequest(RequestPtr ptr);
  void requestTicker(const string& symbol); 
    
private:
  void processRecvQueue();

  Provider* mpRouter;

  RequestQueue mSendQueue;
  ResultQueue mRecvQueue;

  Mutex mSendMutex;
  Mutex mRecvMutex;
};
}
#endif //BOT_THREAD_HPP
