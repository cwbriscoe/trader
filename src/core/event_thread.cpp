#include "event_thread.hpp"
#include "cb/iostream.hpp"

using namespace cb;

EventThread::EventThread()
  : Thread() { 
}

EventThread::~EventThread() {
}

/******************************************************************************/
/** Main Event Loop                                                          **/
/******************************************************************************/
void EventThread::run() {
  while (this->canRun()) {
    this->processRecvQueue();
    this->sleep(0);
  }
}

/******************************************************************************/
/** Queueing & IO Methods                                                    **/
/******************************************************************************/
void EventThread::recv(const ResultPtr ptr) {
  Guard guard(mRecvMutex);
  mRecvQueue.push(ptr);
}

void EventThread::processRecvQueue() {
  while (!mRecvQueue.empty()) {
    ResultPtr tran;

    { //lock queue, pop one record and immediately unlock queue
      Guard guard(mRecvMutex);
      if (mRecvQueue.empty())
        return;

      tran = mRecvQueue.front();
      mRecvQueue.pop();
    }
    {
      //does not do anything now but put it in the outgoing queue for the qui
      //to pick up.  However, there could be filtering or other special processing
      //here later
      Guard guard(mSendMutex);
      mSendQueue.push(tran);
    }
  }
}

ResultPtr EventThread::get() {
  if (!mSendQueue.empty()) {
    ResultPtr tran;

    { //lock queue, pop one record and immediately unlock queue
      Guard guard(mSendMutex);
      if (mSendQueue.empty())
        return nullptr;

      tran = mSendQueue.front();
      mSendQueue.pop();
      return tran;
    }
  }
  return nullptr;
}

