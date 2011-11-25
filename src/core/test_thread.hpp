#ifndef TEST_THREAD_HPP
#define TEST_THREAD_HPP

#include "cb/thread.hpp" 

namespace cb {

class TestThread : public Thread {
public:
    TestThread();
    virtual ~TestThread();
    
protected:
    virtual void run();
    
private:
    void execute();
    unsigned mCounter;
};

}
#endif //TEST_THREAD_HPP
