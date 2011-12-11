#include "client_thread.hpp"
#include "cb/iostream.hpp" 
#include "twsapi/EPosixClientSocket.h"
#include "twsapi/Contract.h"
#include "twsapi/Order.h"

using namespace cb;

const int PING_DEADLINE = 2; // seconds
const int SLEEP_BETWEEN_PINGS = 30; // seconds

ClientThread::ClientThread(Requester* ptr)
  : Thread() 
	, mpClient(new EPosixClientSocket(this))
  , mConnected(false)
	, mState(ST_CONNECT)
	, mSleepDeadline(0)
	, mOrderId(0)
	, mpRouter(ptr) {
}

ClientThread::~ClientThread() {
}

/******************************************************************************/
/** Main Event Loop                                                          **/
/******************************************************************************/

void ClientThread::run() {

  while (this->canRun()) {
    if (!mConnected)
      this->connect("127.0.0.1", 4001, 1);
    else {
	    this->processMessages();
      this->sleep(0);
    }
  }
  this->disconnect();
}

/******************************************************************************/
/** Queueing & IO Methods                                                    **/
/******************************************************************************/

void ClientThread::send(const RequestPtr tran) {
  Guard guard(mSendMutex);
  mSendQueue.push(tran);
}

void ClientThread::processSendQueue() {
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
        tickRequest(std::static_pointer_cast<TickRqst>(tran));
        break;
      default:
        cout << "clientthread: invalid request - " << (int)tran->mRqstType << endl;
        break;
    }
  }
}

void ClientThread::recvSocket() {
	fd_set readSet, errorSet;

	if(mpClient->fd() < 0) 
    return;

  FD_ZERO(&readSet);
  FD_ZERO(&errorSet);
  FD_SET(mpClient->fd(), &readSet);
  FD_CLR(mpClient->fd(), &errorSet);

  //make timeout non-blocking with a zero timeout
  struct timeval timeout; timeout.tv_sec = 0; timeout.tv_usec = 0;

  //read socket input in a non-blocking fashion
  int ret = select(mpClient->fd() + 1, &readSet, NULL, &errorSet, &timeout);

  if (ret == 0)   // timeout
    return;
  
  if (ret < 0) {	// error
    this->disconnect();
    return;
  }

  if (mpClient->fd() < 0)
    return;

  if (FD_ISSET(mpClient->fd(), &errorSet)) 
    mpClient->onError();

  if (mpClient->fd() < 0)
    return;

  if (FD_ISSET(mpClient->fd(), &readSet))
    mpClient->onReceive();
}

void ClientThread::sendSocket() {
	fd_set writeSet, errorSet;

	if(mpClient->fd() < 0) 
    return;

  FD_ZERO(&writeSet);
  FD_ZERO(&errorSet);
  FD_SET(mpClient->fd(), &writeSet);
  FD_CLR(mpClient->fd(), &errorSet);

  //make timeout non-blocking with a zero timeout
  struct timeval timeout; timeout.tv_sec = 0; timeout.tv_usec = 0;

  //read socket input in a non-blocking fashion
  int ret = select(mpClient->fd() + 1, NULL, &writeSet, &errorSet, &timeout);

  if (ret == 0)   // timeout
    return;
  
  if (ret < 0) {	// error
    this->disconnect();
    return;
  }

  if (mpClient->fd() < 0)
    return;

  if (FD_ISSET(mpClient->fd(), &errorSet)) 
    mpClient->onError();

  if (mpClient->fd() < 0)
    return;

  if (FD_ISSET(mpClient->fd(), &writeSet))
    mpClient->onSend();
}

/******************************************************************************/
/** Methods                                                                  **/
/******************************************************************************/

bool ClientThread::connect(const char *host, unsigned int port, int clientId) {
  cout << "client #" << clientId << " attempting to connect to " << host
       << ":" << port << endl;

  while (!mConnected && this->canRun()) {
	  mConnected = mpClient->eConnect(host, port, clientId);
    if (!mConnected)
      this->sleep(1000); //1sec
  }

  if (mConnected)
    cout << "connected" << endl;

  return true;
}

void ClientThread::disconnect() {
  if (mConnected) {
	  mpClient->eDisconnect();
    mConnected = false;
    cout << "disconnected" << endl;
  }
}

bool ClientThread::isConnected() const {
	return mpClient->isConnected();
}

void ClientThread::processMessages() {
	struct timeval tval;
	tval.tv_usec = 0;
	tval.tv_sec = 0;

	time_t now = time(NULL);

	switch (mState) {
		case ST_TICKREQ:
			//tickRequest();
			break;
		case ST_PLACEORDER:
			placeOrder();
			break;
		case ST_PLACEORDER_ACK:
			break;
		case ST_CANCELORDER:
			cancelOrder();
			break;
		case ST_CANCELORDER_ACK:
			break;
		case ST_PING:
			reqCurrentTime();
			break;
		case ST_PING_ACK:
			if(mSleepDeadline < now) {
				disconnect();
				return;
			}
			break;
		case ST_IDLE:
			if(mSleepDeadline < now) {
				mState = ST_PING;
				return;
			}
			break;
    default:
      break;
	}

	if(mSleepDeadline > 0) {
		// initialize timeout with m_sleepDeadline - now
		tval.tv_sec = mSleepDeadline - now;
	}

  this->processSendQueue();
  this->sendSocket();
  this->recvSocket();
}

void ClientThread::reqCurrentTime() {
  cout << "requesting current time" << endl;

	// set ping deadline to "now + n seconds"
	mSleepDeadline = time( NULL) + PING_DEADLINE;

	mState = ST_PING_ACK;

	mpClient->reqCurrentTime();
}

void ClientThread::tickRequest(const TickRqstPtr req) {
	Contract contract;
  const char* stock_tick_types = "100,101,104,105,106,107,125,165,166,225,232,221,233,236,258,47,291,293,294,295,318,370,370,377,381,384,384,387,388,391,407,411";

	//contract.symbol = "MSFT";
  contract.symbol = req->mSymbol;
	contract.secType = "STK";
	contract.exchange = "SMART";
	contract.currency = "USD";

  cout << "requesting market data for " << contract.symbol << endl;

	mState = ST_PLACEORDER;
  mpClient->reqMktData(req->mTickerId, contract, stock_tick_types, false);
}

void ClientThread::placeOrder() {
	Contract contract;
	Order order;

	contract.symbol = "MSFT";
	contract.secType = "STK";
	contract.exchange = "SMART";
	contract.currency = "USD";

	order.action = "BUY";
	order.totalQuantity = 1000;
	order.orderType = "LMT";
	order.lmtPrice = 0.01;

	printf( "Placing Order %ld: %s %ld %s at %f\n", mOrderId, order.action.c_str(), order.totalQuantity, contract.symbol.c_str(), order.lmtPrice);

	mState = ST_PLACEORDER_ACK;
	mpClient->placeOrder(mOrderId, contract, order);
}

void ClientThread::cancelOrder() {
  cout << "cancelling order id " << mOrderId << endl;
	mState = ST_CANCELORDER_ACK;
	mpClient->cancelOrder(mOrderId);
}

/******************************************************************************/
/** Implemented EWrapper Events                                              **/
/******************************************************************************/
void ClientThread::orderStatus( OrderId orderId, const IBString &status,
int filled, int remaining, double avgFillPrice, int permId, int parentId,
double lastFillPrice, int clientId, const IBString& whyHeld) {
	if (orderId == mOrderId) {
		if (mState == ST_PLACEORDER_ACK && (status == "PreSubmitted" || status == "Submitted"))
			mState = ST_CANCELORDER;
		if (mState == ST_CANCELORDER_ACK && status == "Cancelled")
			mState = ST_PING;
		printf( "Order: id=%ld, status=%s\n", orderId, status.c_str());
	}
}

void ClientThread::nextValidId(OrderId orderId) {
	mOrderId = orderId;
	mState = ST_TICKREQ;
}

void ClientThread::currentTime(long time) {
	if (mState == ST_PING_ACK) {
		time_t t = (time_t)time;
		struct tm * timeinfo = localtime (&t);
		printf("The current date/time is: %s", asctime(timeinfo));

		time_t now = ::time(NULL);
		mSleepDeadline = now + SLEEP_BETWEEN_PINGS;

		mState = ST_IDLE;
	}
}

void ClientThread::error(const int id, const int errorCode, const IBString errorString) {
  cout << "ERROR " << errorCode << ": " << errorString << endl;
	if(id == -1 && errorCode == 1100) // if "Connectivity between IB and TWS has been lost"
		disconnect();
}

/******************************************************************************/
/** Unimplemented EWrapper Events                                            **/
/******************************************************************************/
void ClientThread::tickPrice(TickerId tickerId, TickType field, double price, int canAutoExecute) {
  //cout << "tickPrice: " << "id:" << tickerId << " type:" << field
  //     << " price:" << price << " auto:" << canAutoExecute << endl;
  auto ptr = TickPriceRslt::create();
  ptr->mTickerId       = tickerId;
  ptr->mFieldType      = field;
  ptr->mValue          = price;
  ptr->mCanAutoExecute = canAutoExecute;
  mpRouter->recv(ptr);
}

void ClientThread::tickSize(TickerId tickerId, TickType field, int size) {
  //cout << "tickSize: " << "id:" << tickerId << " type:" << field
  //     << " size:" << size << endl;
  auto ptr = TickSizeRslt::create();
  ptr->mTickerId       = tickerId;
  ptr->mFieldType      = field;
  ptr->mSize           = size;
  mpRouter->recv(ptr);
}

void ClientThread::tickOptionComputation(TickerId tickerId, TickType tickType,
double impliedVol, double delta, double optPrice, double pvDividend,
double gamma, double vega, double theta, double undPrice) {
  cout << "tickOptionComputation:" << endl;
}

void ClientThread::tickGeneric(TickerId tickerId, TickType tickType, double value) {
  //cout << "tickGeneric:" << endl;
}

void ClientThread::tickString(TickerId tickerId, TickType field, const IBString& value) {
  //cout << "tickString: " << "id:" << tickerId << " type:" << tickType
  //     << " value:" << value << endl;
  auto ptr = TickStringRslt::create();
  ptr->mTickerId       = tickerId;
  ptr->mFieldType      = field;
  ptr->mValue          = value;
  mpRouter->recv(ptr);
}

void ClientThread::tickEFP(TickerId tickerId, TickType tickType, double basisPoints,
const IBString& formattedBasisPoints, double totalDividends, int holdDays,
const IBString& futureExpiry, double dividendImpact, double dividendsToExpiry) {
  cout << "tickEFP:" << endl;
}

void ClientThread::openOrder(OrderId orderId, const Contract&, const Order&, const OrderState&) {
  cout << "openOrder:" << endl;
}

void ClientThread::openOrderEnd() {
  cout << "openOrderEnd:" << endl;
}

void ClientThread::winError(const IBString &str, int lastError) {
  cout << "winError:" << endl;
}

void ClientThread::connectionClosed() {
  cout << "connectionClosed:" << endl;
}

void ClientThread::updateAccountValue(const IBString& key, const IBString& val, 
const IBString& currency, const IBString& accountName) {
  cout << "updateAccountValue:" << endl;
}

void ClientThread::updatePortfolio(const Contract& contract, int position,
double marketPrice, double marketValue, double averageCost,
double unrealizedPNL, double realizedPNL, const IBString& accountName) {
  cout << "updatePortfolio:" << endl;
}

void ClientThread::updateAccountTime(const IBString& timeStamp) {
  cout << "updateAccountTime:" << endl;
}

void ClientThread::accountDownloadEnd(const IBString& accountName) {
  cout << "accountDownloadEnd:" << endl;
}

void ClientThread::contractDetails(int reqId, const ContractDetails& contractDetails) {
  cout << "contractDetails:" << endl;
}

void ClientThread::bondContractDetails(int reqId, const ContractDetails& contractDetails) {
  cout << "bondContractDetails:" << endl;
}

void ClientThread::contractDetailsEnd(int reqId) {
  cout << "contractDetailsEnd:" << endl;
}

void ClientThread::execDetails(int reqId, const Contract& contract, const Execution& execution) {
  cout << "execDetails:" << endl;
}

void ClientThread::execDetailsEnd(int reqId) {
  cout << "execDetailsEnd:" << endl;
}

void ClientThread::updateMktDepth(TickerId id, int position, int operation, int side,
double price, int size) {
  cout << "updateMktDepth:" << endl;
}

void ClientThread::updateMktDepthL2(TickerId id, int position, IBString marketMaker,
int operation, int side, double price, int size) {
  cout << "updateMktDepthL2:" << endl;
}

void ClientThread::updateNewsBulletin(int msgId, int msgType, const IBString& newsMessage,
const IBString& originExch) {
  cout << "updateNewsBulletin:" << endl;
}

void ClientThread::managedAccounts(const IBString& accountsList) {
  cout << "managedAccounts:" << endl;
}

void ClientThread::receiveFA(faDataType pFaDataType, const IBString& cxml) {
  cout << "receiveFA:" << endl;
}

void ClientThread::historicalData(TickerId reqId, const IBString& date, double open,
double high, double low, double close, int volume, int barCount, double WAP,
int hasGaps) {
  cout << "historicalData:" << endl;
}

void ClientThread::scannerParameters(const IBString &xml) {
  cout << "scannerParameters:" << endl;
}

void ClientThread::scannerData(int reqId, int rank, const ContractDetails &contractDetails,
const IBString &distance, const IBString &benchmark, const IBString &projection,
const IBString &legsStr) {
  cout << "scannerData:" << endl;
}

void ClientThread::scannerDataEnd(int reqId) {
  cout << "scannerDataEnd:" << endl;
}

void ClientThread::realtimeBar(TickerId reqId, long time, double open, double high,
double low, double close, long volume, double wap, int count) {
  cout << "realtimeBar:" << endl;
}

void ClientThread::fundamentalData(TickerId reqId, const IBString& data) {
  cout << "fundamentalData:" << endl;
}

void ClientThread::deltaNeutralValidation(int reqId, const UnderComp& underComp) {
  cout << "deltaNeutralValidation:" << endl;
}

void ClientThread::tickSnapshotEnd(int reqId) {
  cout << "tickSnapshotEnd:" << endl;
}

