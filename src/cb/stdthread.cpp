using namespace cb;

Thread::Thread() {
	mCanRun = false;
  mIsRunning = false;
}

Thread::~Thread() {
  mCanRun = false;
  this->join();
}

thread::id Thread::threadID() const {
  return mThread.get_id();
}

void Thread::shutdown() {
  this->stop();
  this->join();
}

void Thread::join() {
  if (mThread.joinable())
	  mThread.join();
}

void Thread::sleep(const unsigned ms) const {
  std::this_thread::sleep_for(std::chrono::milliseconds(ms));
}

void Thread::start() {
  if (!mIsRunning) {
    mIsRunning = true;
    mCanRun = true;
    mThread = std::thread(Thread::threadFunc, this);
  }
}

void Thread::stop() {
  mCanRun = false;
}

bool Thread::canRun() const {
	return mCanRun;
}

void Thread::_run() {
  this->run();
  mIsRunning = false;
}

void Thread::threadFunc(void *args) {
	Thread* pThread = reinterpret_cast<Thread*>(args);

	if (pThread)
		pThread->_run();
}

