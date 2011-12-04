#include "bar_thread.hpp"

using namespace cb;

using std::cout;
using std::endl;

BarThread::BarThread(Provider* router)
  : Thread() 
  , mpRouter(router) {
}

BarThread::~BarThread() {
}

/******************************************************************************/
/** Main Event Loop                                                          **/
/******************************************************************************/

void BarThread::run() {

  while (this->canRun()) {
    this->processSendQueue();
    this->processRecvQueue();
    this->sleep(0);
  }
}

/******************************************************************************/
/** Queueing & IO Methods                                                    **/
/******************************************************************************/

void BarThread::send(const RequestPtr ptr) {
  Guard guard(mSendMutex);
  mSendQueue.push(ptr);
}

void BarThread::recv(const ResultPtr ptr) {
  Guard guard(mRecvMutex);
  mRecvQueue.push(ptr);
}

void BarThread::processSendQueue() {
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
        break;
      }
      default:
        break;
    }
    mpRouter->send(tran);
  }
}

void BarThread::processRecvQueue() {
  while (!mRecvQueue.empty()) {
    ResultPtr tran;

    { //lock queue, pop one record and immediately unlock queue
      Guard guard(mRecvMutex);
      if (mRecvQueue.empty())
        return;

      tran = mRecvQueue.front();
      mRecvQueue.pop();
    }
  }
}

/******************************************************************************/
/** Methods                                                                  **/
/******************************************************************************/

