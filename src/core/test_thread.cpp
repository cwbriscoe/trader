#include "test_thread.hpp"
#include <iostream>
#include "twsapi/PosixTestClient.h"

using namespace cb;

using std::cout;
using std::endl;

TestThread::TestThread() : Thread() {
  mCounter = 0;
}

TestThread::~TestThread() {
}

void TestThread::run() {
  /*while (this->canRun()) {
    ++mCounter;
    std::cout << mCounter << " Hello World from test thread" << std::endl;
    this->sleep(500);
  }*/
  this->execute();
}

const unsigned MAX_ATTEMPTS = 10;
const unsigned SLEEP_TIME = 1000;

void TestThread::execute() {
	//const char* host = argc > 1 ? argv[1] : "";
  const char* host = "127.0.0.1";
	const unsigned port = 7496;
	int clientId = 0;
	unsigned attempt = 0;

	for (;;) {
		++attempt;
    cout << "attempting to connect (try #" << attempt << ")" << endl;

		PosixTestClient client;

		client.connect(host, port, clientId);

		while(client.isConnected()) {
			client.processMessages();
		}

		if(attempt >= MAX_ATTEMPTS) {
			break;
		}

    cout << "sleeping for " << SLEEP_TIME/1000 << " seconds before next attempt" << endl;

		this->sleep(SLEEP_TIME);
	}

  cout << "end of posix socket client test" << endl;
}

