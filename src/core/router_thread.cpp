#include <utility>
#include "router_thread.hpp"
#include "client_thread.hpp"
#include "bar_thread.hpp"
#include "event_thread.hpp"
#include "bot_thread.hpp"

using namespace cb;

using std::cout;
using std::endl;
using std::pair;

RouterThread::RouterThread()
  : Thread() 
  , mLastTickerId(0)
	, mpBarMaker(nullptr)
	, mpServer(nullptr) 
	, mpEvents(nullptr)
	, mpBot(nullptr) {

  mpServer = new ClientThread(this);
  mpServer->start();

  mpBarMaker = new BarThread(this);
  mpBarMaker->start();

  mpEvents = new EventThread();
  mpEvents->start();

  mpBot = BotThreadPtr(new BotThread((Provider*)this, (Provider*)mpBarMaker, (Requester*)mpEvents));
  mpBot->start();
}

RouterThread::~RouterThread() {
  if (mpBot)
    mpBot->shutdown();

  mpBarMaker->shutdown();
  delete mpBarMaker;

  mpEvents->shutdown();
  delete mpEvents;

  mpServer->shutdown();
  delete mpServer;

  for (SymbolMap::iterator it = mSymbolMap.begin(); it != mSymbolMap.end(); ++it)
    cout << "  [" << (*it).first << ", " << (long)(*it).second << "]" << endl;

  for (TickMap::iterator it = mTickMap.begin(); it != mTickMap.end(); ++it)
    cout << "  [" << (*it).first << ", " << (*it).second << "]" << endl;
}

/******************************************************************************/
/** Main Event Loop                                                          **/
/******************************************************************************/

void RouterThread::run() {
  //TickRqstPtr ptr = TickRqst::create();
  //ptr->mSymbol = "MSFT";
  //mSendQueue.push(ptr);

  while (this->canRun()) {
    this->processSendQueue();
    this->processRecvQueue();
    this->sleep(0);
  }
}

/******************************************************************************/
/** Queueing & IO Methods                                                    **/
/******************************************************************************/

void RouterThread::send(const RequestPtr ptr) {
  Guard guard(mSendMutex);
  mSendQueue.push(ptr);
}

void RouterThread::recv(const ResultPtr ptr) {
  Guard guard(mRecvMutex);
  mRecvQueue.push(ptr);
}

void RouterThread::processSendQueue() {
  while (!mSendQueue.empty()) {
    RequestPtr tran;

    { //lock queue, pop one record and immediately unlock queue
      Guard guard(mSendMutex);
      if (mSendQueue.empty())
        return;

      tran = mSendQueue.front();
      mSendQueue.pop();
    }

    switch (tran->mRqstType) {
      case OutRqst::Tick: 
      case OutRqst::Bar: 
        this->processTickRequest(tran);
        break;
      default:
        cout << "routerthread: invalid request - " << (int)tran->mRqstType << endl;
        break;
    }
  }
}

void RouterThread::processRecvQueue() {
  while (!mRecvQueue.empty()) {
    ResultPtr tran;

    { //lock queue, pop one record and immediately unlock queue
      Guard guard(mRecvMutex);
      if (mRecvQueue.empty())
        return;

      tran = mRecvQueue.front();
      mRecvQueue.pop();
    }

    switch (tran->mRsltType) {
      case InRslt::TickPrice: 
      case InRslt::TickSize:
      case InRslt::TickString:
        this->processTickResults(tran);
        break;
      default:
        break;
    }
  }
}

/******************************************************************************/
/** Methods                                                                  **/
/******************************************************************************/
void RouterThread::processTickRequest(RequestPtr tran) {
  auto ptr = std::static_pointer_cast<TickRqst>(tran);

  //find symbol in the symbol map
  ptr->mTickerId = mSymbolMap[ptr->mSymbol];
  //cout << "tickerid=" << ptr->mTickerId << endl;

  //if not found, add it to the symbol map
  bool newTickRqst = false;
  if (!ptr->mTickerId) {
    ptr->mTickerId = ++mLastTickerId;
    //cout << "inserting " << ptr->mSymbol << " " << ptr->mTickerId << endl;
    mSymbolMap[ptr->mSymbol] = ptr->mTickerId;
    newTickRqst = true;
  }
  
  //find requestor in the tick map
  pair<TickMap::iterator, TickMap::iterator> rng;
  rng = mTickMap.equal_range(ptr->mTickerId);
  for (auto it = rng.first; it != rng.second; ++it) {
    if (it->second == tran->mpRequester) {
      cout << "duplicate tick request for " << ptr->mSymbol << endl;
      return;
    }
  }

  //if not found, add to tick map and send request to server
  mTickMap.insert(TickMap::value_type(ptr->mTickerId, tran->mpRequester));
  if (newTickRqst) mpServer->send(tran);
}

void RouterThread::processTickResults(ResultPtr tran) {
  auto ptr = std::static_pointer_cast<TickRslt>(tran);
  //loop through all requestors for specified symbol and send tick results
  pair<TickMap::iterator, TickMap::iterator> rng;
  rng = mTickMap.equal_range(ptr->mTickerId);
  for (auto it = rng.first; it != rng.second; ++it)
    it->second->recv(tran);
}

void RouterThread::addBot(BotThread* bot) {
  mpBot = BotThreadPtr(bot);
  mpBot->start();
}

