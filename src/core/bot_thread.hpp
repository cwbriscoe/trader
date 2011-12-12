#ifndef BOT_THREAD_HPP
#define BOT_THREAD_HPP

#include "cb/thread.hpp"
#include "thread_interface.hpp"

namespace cb {

class BotThread : public Thread, public Requester {
public:
  BotThread();
  virtual ~BotThread();

  void init(Provider* router, Provider* barmaker, Requester* events);
  void recv(const ResultPtr tran);

  void requestTicker(const string& symbol); 
  void requestBars(const string& symbol, const unsigned secs);

protected:
  void run();

  void sendRequest(Provider* provider, RequestPtr ptr);
    
private:
  void processRecvQueue();

  Provider*  mpRouter;
  Provider*  mpBarMaker;
  Requester* mpEvents;

  RequestQueue mSendQueue;
  ResultQueue mRecvQueue;

  Mutex mSendMutex;
  Mutex mRecvMutex;
};
}
#endif //BOT_THREAD_HPP
