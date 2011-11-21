#ifndef WINTHREAD_HPP
#define WINTHREAD_HPP
 
#include <windows.h>
#include <process.h>
#include "nocopy.hpp"

namespace cb {

class Mutex : public Nocopy {
public:
	Mutex() {InitializeCriticalSection(&mMutex);}
	~Mutex() {DeleteCriticalSection(&mMutex);}

	void acquire() {EnterCriticalSection(&mMutex);}
	void release() {LeaveCriticalSection(&mMutex);}

private:
	CRITICAL_SECTION mMutex;
};

class Lock {
public:
	Lock(Mutex &mutex) : mMutex(mutex) {mMutex.acquire();}
	~Lock() {mMutex.release();}

private:
	Mutex &mMutex;
};

class Thread : public Nocopy {
public:

	Thread();
	virtual ~Thread();

	void start();
	void stop();
	void join();
	void shutdown();

	unsigned int threadId() const;
	
protected:
	bool canRun();
	virtual void run() = 0;

private:
	static unsigned __stdcall threadFunc(void *args);
	
	HANDLE mThread;
	unsigned mThreadId;
	volatile bool mCanRun;
	volatile bool mStopped;
	Mutex mMutex;
};

}
#endif //WINTHREAD_HPP
