#ifndef MOVE_h
#define MOVE_h

#include <string>

class Move {
 private:
 public:
  virtual std::string toString() const;
  virtual bool operator==(const Move &other) const;
};

#endif
