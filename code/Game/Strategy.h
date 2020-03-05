#ifndef STRATEGY_h
#define STRATEGY_h

#include "Board/Board.h"
#include "Board/Move.h"

class Strategy {
 public:
  virtual ~Strategy() {}
  virtual const Move getMove(const Board &board, Player playerID,
                             Player enemyID) = 0;

  virtual string toString() { return "Strategy"; }
};

#endif
