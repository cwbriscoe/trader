#ifndef TRANACTION_H
#define TRANACTION_H

#include <atomic>
#include <memory>
#include <queue>
#include "cb/string.hpp"

namespace cb {

using std::atomic;
using std::shared_ptr;
using std::queue;

/******************************************************************************/
/** Transaction base class                                                   **/
/******************************************************************************/
struct Transaction;
typedef shared_ptr<Transaction> TranPtr;
struct Transaction {
  Transaction() {
    mTranId = mNextId++;
  }
  virtual ~Transaction() {};

  static atomic<unsigned>   mNextId;
  unsigned                  mTranId;
};

/******************************************************************************/
/** Request definitions                                                      **/
/******************************************************************************/
enum class OutRqst {Tick};

struct Request : public Transaction {
  Request(const OutRqst rqst) : Transaction(), mRqstType(rqst) {}

  OutRqst mRqstType;
};
typedef shared_ptr<Request> RequestPtr;
typedef queue<RequestPtr> RequestQueue;

/******************************************************************************/
/** Result definitions                                                       **/
/******************************************************************************/
enum class InRslt {Tick};

struct Result : public Transaction {
  Result(const InRslt rslt) : Transaction(), mRsltType(rslt) {}

  InRslt mRsltType;
};
typedef shared_ptr<Result> ResultPtr;
typedef queue<ResultPtr> ResultQueue;

/******************************************************************************/
/** Request/Outgoing transactions                                            **/
/******************************************************************************/
struct TickRqst;
typedef shared_ptr<TickRqst> TickRqstPtr;

struct TickRqst : public Request {
  TickRqst() : Request(OutRqst::Tick) {}

  static TickRqstPtr create() {return TickRqstPtr(new TickRqst);}

  string symbol;
};
}
#endif //TRANACTION_H
