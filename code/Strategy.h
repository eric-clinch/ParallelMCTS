#ifndef STRATEGY_h
#define STRATEGY_h

#include "Move.h"
#include "Board.h"

class Strategy {
public:
	virtual Move getMove(Board &board, Player playerID, Player enemyID) = 0;
	virtual string toString() { return "Strategy"; }
};

#endif
