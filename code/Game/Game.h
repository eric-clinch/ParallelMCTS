#ifndef GAME_h
#define GAME_h

#include <assert.h>
#include <iostream>
#include <random>
#include <vector>
#include "Board/Board.h"
#include "Strategy.h"

class Game {
 private:
  static std::random_device rd;
  static std::mt19937 rng;
  static std::uniform_real_distribution<> uni;

 public:
  Game() {}

  // plays a game between strategy S0 and S1 on a board of the given board size
  // for a max rounds of maxRounds. If maxRounds < 0, then the game will be
  // played until completion. Returns 0 if S0 won and 1 if S1 won.
  static int runGame(Strategy* S0, Strategy* S1, int boardSize = 19,
                     int maxRounds = -1, bool verbose = true);

  // returns a random int in the given range, inclusive of the
  // lower bound and exclusive of the upper bound
  static int randint(int lower, int upper);

  static void runTournament(const std::vector<Strategy*>& strategies,
                            int boardSize = 19, int maxGameRounds = -1,
                            int maxTournamentRounds = -1);
};

#endif
