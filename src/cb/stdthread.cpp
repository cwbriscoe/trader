#include "thread.h"

namespace cb {

Thread::Thread() : mStop() {
}

Thread::~Thread() {
  this->stop();
}

void Thread::start() {
  mThread = std::thread(&Thread::Run, *this);
}

void Thread::stop() {
  mStop = true;
  mThread.join();
}

void Thread::run() {
  while (!mStop) {
    this->loop();
  }
}
}
