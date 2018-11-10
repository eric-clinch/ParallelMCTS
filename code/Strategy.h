#ifndef STRATEGY_h
#define STRATEGY_h

#include "Move.h"
#include "Board.h"

class Strategy {
public:
	virtual Move getMove(Board &board, Player playerID, Player enemyID, int totalTime, int timePerMove, int round) = 0;
	virtual void cleanUp() {} // needs to be called at the end of each game to allow freeing of memory
	virtual string toString() { return "Strategy"; }
};

#endif