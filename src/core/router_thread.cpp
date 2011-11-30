#include "router_thread.hpp"
#include "client_thread.hpp"
#include "bot_thread.hpp"

using namespace cb;

using std::cout;
using std::endl;

RouterThread::RouterThread()
  : Thread() 
  , mLastTickerId(0)
	, mpServer(nullptr) {
  mpServer = new ClientThread(this);
  mpServer->start();
}

RouterThread::~RouterThread() {
  if (mpBot)
    mpBot->shutdown();
  mpServer->shutdown();
  delete mpServer;
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
      case OutRqst::Tick: {
        auto ptr = std::static_pointer_cast<TickRqst>(tran);
        ptr->mTickerId = ++mLastTickerId;
        //tickRequest(std::static_pointer_cast<TickRqst>(tran));
        break;
      }
      default:
        break;
    }
    mpServer->send(tran);
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

    if (mpBot)
      mpBot->recv(tran);

    /*switch (tran->mRsltType) {
      case InRslt::TickPrice: {
        auto ptr = std::static_pointer_cast<TickPriceRslt>(tran);
        cout << "tickPrice: " << "id:" << ptr->mTickerId << " type:"
             << ptr->mFieldType << " price:" << ptr->mValue << " auto:"
             << ptr->mCanAutoExecute << endl;
        break;
      }
      case InRslt::TickSize:
        break;
      case InRslt::TickString:
        break;
      default:
        break;
    }*/
  }
}

/******************************************************************************/
/** Methods                                                                  **/
/******************************************************************************/

void RouterThread::addBot(BotThread* bot) {
  mpBot = BotThreadPtr(bot);
  mpBot->start();
}

