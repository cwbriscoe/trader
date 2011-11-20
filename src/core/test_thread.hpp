#ifndef TEST_THREAD_HPP
#define TEST_THREAD_HPP

//namespace std { class type_info; }

#include "cb/thread.hpp" 

namespace cb {
class TestThread : public Thread {
public:
    TestThread();
    virtual ~TestThread();

    void start();
    void stop();
    
protected:
    virtual void loop();
    
private:
};
}
#endif //TEST_THREAD_HPP
