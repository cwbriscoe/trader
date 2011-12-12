#include "bot_thread.hpp"
#include "cb/iostream.hpp"

using namespace cb;

BotThread::BotThread()
  : Thread() 
	, mpRouter(nullptr) 
	, mpBarMaker(nullptr)
	, mpEvents(nullptr) {
}

BotThread::~BotThread() {
}

void BotThread::init(Provider* router, Provider* barmaker, Requester* events) {
  mpRouter   = router;
  mpBarMaker = barmaker;
  mpEvents   = events;
}

/******************************************************************************/
/** Main Event Loop                                                          **/
/******************************************************************************/

void BotThread::run() {
  /*this->requestTicker("MSFT");
  this->requestTicker("GOOG");
  this->requestTicker("YHOO");
  this->requestTicker("DELL");
  this->requestTicker("AAPL");
  this->requestTicker("INTC");
  this->requestTicker("IBM");
  this->requestTicker("CSC");
  this->requestTicker("GE");
  this->requestTicker("F");
  this->requestBars("GOOG",1);
  this->requestBars("GOOG",2);
  this->requestBars("GOOG",3);*/

  while (this->canRun()) {
    this->processRecvQueue();
    this->sleep(0);
  }
}

/******************************************************************************/
/** Queueing & IO Methods                                                    **/
/******************************************************************************/

void BotThread::recv(const ResultPtr ptr) {
  Guard guard(mRecvMutex);
  mRecvQueue.push(ptr);
}

void BotThread::processRecvQueue() {
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
      case InRslt::TickPrice: {
        auto ptr = std::static_pointer_cast<TickPriceRslt>(tran);
        auto p = ptr->mFieldType;
        if (p!=4 && p!=1 && p!=2)
        cout << "id:" << ptr->mTickerId << " type:"
             << ptr->mFieldType << " price:" << ptr->mValue << " auto:"
             << ptr->mCanAutoExecute << endl;
        break;
      }
      case InRslt::TickSize: {
        auto ptr = std::static_pointer_cast<TickSizeRslt>(tran);
        auto p = ptr->mFieldType;
        if (p!=5 && p!=0 && p!=3 && p!=8)
        cout << "id:" << ptr->mTickerId << " type:" << ptr->mFieldType
             << " size:" << ptr->mSize << endl;
        break;
      }
      case InRslt::TickString: {
        auto ptr = std::static_pointer_cast<TickStringRslt>(tran);
        auto p = ptr->mFieldType;
        if (p!=45)
        cout << "id:" << ptr->mTickerId << " type:" << ptr->mFieldType
             << " string:" << ptr->mValue << endl;
        break;
      }
      default:
        break;
    }
    mpEvents->recv(tran);
  }
}

/******************************************************************************/
/** Request Methods                                                          **/
/******************************************************************************/
void BotThread::sendRequest(Provider* provider, RequestPtr ptr) {
  ptr->mpRequester = this;
  provider->send(ptr);
}

void BotThread::requestTicker(const string& symbol) {
  TickRqstPtr ptr = TickRqst::create();
  ptr->mSymbol = symbol;;
  this->sendRequest(mpRouter, ptr);
}

void BotThread::requestBars(const string& symbol, const unsigned secs) {
  BarRqstPtr ptr = BarRqst::create();
  ptr->mSymbol = symbol;;
  ptr->mBarSize = secs;
  this->sendRequest(mpBarMaker, ptr);
}

