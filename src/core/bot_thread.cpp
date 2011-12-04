#include "bot_thread.hpp"

using namespace cb;

using std::cout;
using std::endl;

BotThread::BotThread(Provider* ptr)
  : Thread() 
	, mpRouter(ptr) {
}

BotThread::~BotThread() {
}

/******************************************************************************/
/** Main Event Loop                                                          **/
/******************************************************************************/

void BotThread::run() {
  this->requestTicker("MSFT");
  this->requestTicker("GOOG");
  this->requestTicker("YHOO");
  this->requestTicker("DELL");
  this->requestTicker("AAPL");
  this->requestTicker("INTC");
  this->requestTicker("IBM");
  this->requestTicker("CSC");
  this->requestTicker("GE");
  this->requestTicker("F");
  this->requestTicker("F");
  this->requestTicker("F");

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
        //cout << "tickPrice: " << "id:" << ptr->mTickerId << " type:"
        //     << ptr->mFieldType << " price:" << ptr->mValue << " auto:"
        //     << ptr->mCanAutoExecute << endl;
        break;
      }
      case InRslt::TickSize:
        break;
      case InRslt::TickString:
        break;
      default:
        break;
    }
  }
}

/******************************************************************************/
/** Request Methods                                                          **/
/******************************************************************************/
void BotThread::sendRequest(RequestPtr ptr) {
  ptr->mpRequester = this;
  mpRouter->send(ptr);
}

void BotThread::requestTicker(const string& symbol) {
  TickRqstPtr ptr = TickRqst::create();
  ptr->mSymbol = symbol;;
  //mpRouter->send(ptr);
  this->sendRequest(ptr);
}
