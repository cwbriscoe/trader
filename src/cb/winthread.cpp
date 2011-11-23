#include "thread.hpp"

using namespace cb;

#include "thread.hpp"

Thread::Thread() {
	mThread = 0;
	mThreadId = 0;
	mCanRun = true;
	mStopped = true;
  mIsRunning = false;
}

Thread::~Thread() {
  if (mThread)
    this->shutdown();	
}

void Thread::join() {
	WaitForSingleObject(mThread, INFINITE);
}

void Thread::shutdown() {
  Lock guard(mMutex);
  mCanRun  = false;
  mStopped = false;
  ResumeThread(mThread);
	CloseHandle(mThread);
  this->join();
}

void Thread::start() {
  Lock guard(mMutex);
  if (!mIsRunning) {
    mStopped = false;
    mIsRunning = true;
	  mThread = reinterpret_cast<HANDLE>(_beginthreadex(0, 0, threadFunc
            ,this, 0, &mThreadId));
  }
  if (mStopped) {
    ResumeThread(mThread);
    mStopped = false;
  }
}

void Thread::stop() {
  Lock guard(mMutex);
  if (!mStopped) {
    SuspendThread(mThread);
    mStopped = true;
  }
}

bool Thread::canRun() {
	Lock guard(mMutex);
	return mCanRun;
}

unsigned Thread::threadId() {
	Lock guard(mMutex);
	return mThreadId;
}

void Thread::_run() {
  this->run();
  mIsRunning = false;
  CloseHandle(mThread);
  _endthreadex(0);
}

unsigned __stdcall Thread::threadFunc(void *args) {
//unsigned Thread::threadFunc(void *args) {
	Thread* pThread = reinterpret_cast<Thread*>(args);

	if (pThread)
		pThread->_run();

	return 0;
}

