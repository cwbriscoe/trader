#ifndef TRANACTION_H
#define TRANACTION_H

#include <atomic>
#include <queue>
#include "cb/memory.hpp"
#include "cb/string.hpp"

namespace cb {

using std::atomic;
using std::queue;

class Requester;

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
enum class OutRqst {Tick, Bar};

struct Request : public Transaction {
  Request(const OutRqst rqst) : Transaction(), mRqstType(rqst) {}

  Requester* mpRequester;
  OutRqst mRqstType;
};
typedef shared_ptr<Request> RequestPtr;
typedef queue<RequestPtr> RequestQueue;

/******************************************************************************/
/** Result definitions                                                       **/
/******************************************************************************/
enum class InRslt {TickPrice, TickSize, TickString};

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
  TickRqst(const OutRqst rqst) : Request(rqst) {}

  static TickRqstPtr create() {return TickRqstPtr(new TickRqst);}

  long mTickerId;
  string mSymbol;
};

struct BarRqst;
typedef shared_ptr<BarRqst> BarRqstPtr;

struct BarRqst : public TickRqst {
  BarRqst() : TickRqst(OutRqst::Bar) {}

  static BarRqstPtr create() {return BarRqstPtr(new BarRqst);}

  unsigned mBarSize;
};

/******************************************************************************/
/** Results/Incoming transactions                                            **/
/******************************************************************************/
struct TickRslt;
typedef shared_ptr<TickRslt> TickRsltPtr;

struct TickRslt : public Result {
  TickRslt(const InRslt rslt) : Result(rslt) {}

  long     mTickerId;
};

struct TickPriceRslt;
typedef shared_ptr<TickPriceRslt> TickPriceRsltPtr;

struct TickPriceRslt : public TickRslt {
  TickPriceRslt() : TickRslt(InRslt::TickPrice) {}

  static TickPriceRsltPtr create() {return TickPriceRsltPtr(new TickPriceRslt);}

  unsigned mFieldType;
  double   mValue;
  unsigned mCanAutoExecute;
};

struct TickSizeRslt;
typedef shared_ptr<TickSizeRslt> TickSizeRsltPtr;

struct TickSizeRslt : public TickRslt {
  TickSizeRslt() : TickRslt(InRslt::TickSize) {}

  static TickSizeRsltPtr create() {return TickSizeRsltPtr(new TickSizeRslt);}

  unsigned mFieldType;
  int      mSize;
};

struct TickStringRslt;
typedef shared_ptr<TickStringRslt> TickStringRsltPtr;

struct TickStringRslt : public TickRslt {
  TickStringRslt() : TickRslt(InRslt::TickString) {}

  static TickStringRsltPtr create() {return TickStringRsltPtr(new TickStringRslt);}

  unsigned mFieldType;
  string   mValue;
};

}
#endif //TRANACTION_H
