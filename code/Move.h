#ifndef MOVE_h
#define MOVE_h

#include <assert.h>
#include <stdio.h>
#include <iostream>
#include <sstream>
#include "Coordinate.h"
#include "Enums.h"

class Move {
 private:
 public:
  virtual string toString() const;
  virtual bool operator==(const Move &other) const;
};

#endif