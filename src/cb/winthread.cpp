#include "thread.hpp"

using namespace cb;

#include "thread.hpp"

Thread::Thread() {
	mThread = 0;
	mThreadId = 0;
	mCanRun = true;
	mStopped = true;
	mThread = reinterpret_cast<HANDLE>(_beginthreadex(0, 0, threadFunc
                                    ,this, CREATE_SUSPENDED, &mThreadId));
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
  mStopped = true;
  this->start();
	CloseHandle(mThread);
  this->join();
  //mThread = 0;
}

void Thread::start() {
  Lock guard(mMutex);
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

unsigned Thread::threadId() const {
	return mThreadId;
}

//unsigned __stdcall Thread::threadFunc(void *args) {
unsigned Thread::threadFunc(void *args) {
	Thread* pThread = reinterpret_cast<Thread*>(args);
	
	if (pThread)
		pThread->run();

	//_endthreadex(0);
  ExitThread(0);
	return 0;
}

