#ifndef THREAD_HPP
#define THREAD_HPP

#ifdef WIN32
  #include "winthread.hpp"
#else

#include <thread>
#include "nocopy.hpp"

namespace cb {

class Thread : public Nocopy {
public:
  Thread();
  virtual ~Thread();

  void start();
  void stop();
    
protected:
  virtual void loop() = 0;
    
private:
  void run();

  std::thread    mThread;
  bool           mStop;
};

}
#endif
#endif // THREAD_HPP
