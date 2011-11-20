#ifndef THREAD_HPP
#define THREAD_HPP

//namespace std { class type_info; }

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
#endif // THREAD_HPP
