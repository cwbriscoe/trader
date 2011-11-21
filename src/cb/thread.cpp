#include "thread.hpp"

#ifdef WIN32
  #include "winthread.cpp"
#else
  #include "stdthread.cpp"
#endif
