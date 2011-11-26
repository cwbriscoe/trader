#ifndef THREAD_HPP
#define THREAD_HPP

#ifdef WIN32
  #include "winthread.hpp"
#else

#include <thread>
#include <atomic>
#include "nocopy.hpp"

namespace cb {
  
using std::thread;
using std::atomic;

typedef std::mutex              Mutex;
typedef std::timed_mutex        TimedMutex;
typedef std::lock_guard<Mutex>  Guard;
typedef std::unique_lock<Mutex> Lock;

class Thread : public Nocopy {
public:

	Thread();
	virtual ~Thread();

  thread::id threadID() const;
	void start();
	void stop();
	void join();
	void shutdown();
	void sleep(const unsigned ms) const;

protected:
	bool canRun() const;
	virtual void run() = 0;

private:
  void _run();
	static void threadFunc(void *args);

  thread       mThread;
  atomic<bool> mCanRun;
	atomic<bool> mIsRunning;
};
}
#endif
#endif //THREAD_HPP
