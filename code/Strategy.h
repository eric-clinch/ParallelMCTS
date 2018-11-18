#ifndef STRATEGY_h
#define STRATEGY_h

#include "Board.h"
#include "Move.h"

class Strategy {
 public:
  virtual const Move getMove(Board &board, Player playerID, Player enemyID) = 0;
  virtual string toString() { return "Strategy"; }
};

#endif
