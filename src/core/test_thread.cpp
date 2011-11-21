#include "test_thread.hpp"
#include <iostream>

using namespace cb;

TestThread::TestThread() : Thread() {
  mCounter = 0;
}

TestThread::~TestThread() {
}

void TestThread::run() {
  while (this->canRun()) {
    ++mCounter;
    std::cout << mCounter << " Hello World from test thread" << std::endl;
    Sleep(500);
  }
}

