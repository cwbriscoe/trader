#ifndef STRING_HPP
#define STRING_HPP

#include <sstream>
#include <algorithm>

using std::string;
using std::stringstream;

//template class to easily convert numerics (and objects?) to std::string
template <class T>
inline string to_string (const T& t) {
  stringstream ss;
  ss << t;
  return ss.str();
}

template<typename RT, typename T, typename Trait, typename Alloc>
RT ss_atoi( const std::basic_string<T, Trait, Alloc>& the_string) {
  std::basic_istringstream< T, Trait, Alloc> temp_ss(the_string);
  RT num;
  temp_ss >> num;
  return num;
}

//trim from start
static inline std::string &ltrim(std::string &s) {
  s.erase(s.begin(), std::find_if(s.begin(),
    s.end(), std::not1(std::ptr_fun<int, int>(std::isspace))));
  return s;
}

//trim from end
static inline std::string &rtrim(std::string &s) {
  s.erase(std::find_if(s.rbegin(), s.rend(), 
    std::not1(std::ptr_fun<int, int>(std::isspace))).base(), s.end());
  return s;
}

//trim from both ends
static inline std::string &trim(std::string &s) {
  return ltrim(rtrim(s));
}
 
#endif //STRING_HPP

