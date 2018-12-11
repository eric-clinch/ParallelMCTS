
#ifndef USERPLAYER_H__
#define USERPLAYER_H__

#include "Board.h"
#include "Move.h"
#include "Strategy.h"

class UserPlayer : public Strategy {
 public:
  UserPlayer();
  virtual const Move getMove(const Board &board, Player playerID,
                             Player enemyID);
};

#endif  // USERPLAYER_H__