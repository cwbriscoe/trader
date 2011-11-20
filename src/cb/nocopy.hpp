#ifndef NOCOPY_HPP
#define NOCOPY_HPP

class Nocopy {
public:
  Nocopy & operator=(const Nocopy&) = delete;
  Nocopy(const Nocopy&) = delete;
  Nocopy() = default;
};

#endif //NOCOPY_HPP
