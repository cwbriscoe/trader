#ifndef BOT_THREAD_HPP
#define BOT_THREAD_HPP

#include "cb/thread.hpp"
#include "thread_interface.hpp"

namespace cb {

class BotThread : public Thread, Requester {
public:
  BotThread(Provider* router, Provider* barmaker);
  virtual ~BotThread();

  void recv(const ResultPtr tran);

protected:
  void run();

  void sendRequest(Provider* provider, RequestPtr ptr);
  void requestTicker(const string& symbol); 
  void requestBars(const string& symbol, const unsigned secs);
    
private:
  void processRecvQueue();

  Provider* mpRouter;
  Provider* mpBarMaker;

  RequestQueue mSendQueue;
  ResultQueue mRecvQueue;

  Mutex mSendMutex;
  Mutex mRecvMutex;
};
}
#endif //BOT_THREAD_HPP
