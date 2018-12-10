
#include <assert.h>
#include <atomic>
#include <cstring>
#include <iostream>
#include "Board.h"
#include "BoardTest.h"
#include "Game.h"
#include "MCTS.h"
#include "Move.h"
#include "Strategy.h"
#include "UserPlayer.h"

// option parsing code taken from 418 course starter code
static int _argc;
static const char** _argv;

const char* get_option_string(const char* option_name,
                              const char* default_value) {
  for (int i = _argc - 2; i >= 0; i -= 2) {
    if (strcmp(_argv[i], option_name) == 0) return _argv[i + 1];
  }
  return default_value;
}

int get_option_int(const char* option_name, int default_value) {
  for (int i = _argc - 2; i >= 0; i -= 2) {
    if (strcmp(_argv[i], option_name) == 0) return atoi(_argv[i + 1]);
  }
  return default_value;
}

float get_option_float(const char* option_name, float default_value) {
  for (int i = _argc - 2; i >= 0; i -= 2) {
    if (strcmp(_argv[i], option_name) == 0)
      return static_cast<float>(atof(_argv[i + 1]));
  }
  return default_value;
}

int main(int argc, const char* argv[]) {
  _argc = argc - 1;
  _argv = argv + 1;

  int msPerMove = 1000 * get_option_int("-t", 1);
  int threads = get_option_int("-p", 1);

  BoardTest::test();

  Strategy* S0 = new MCTS(msPerMove, 1);
  Strategy* S1 = new MCTS(msPerMove, 2);
  Strategy* S2 = new MCTS(msPerMove, 3);
  std::vector<Strategy*> strategies({S0, S1, S2});

  Strategy* user = new UserPlayer();

  // s0_wins = Game::runGame(S0, S1);
  // std::cout << "S0 won " << s0_wins << std::endl;

  Game::runTournament(strategies, 9, 81, -1);

  delete S0;
  delete S1;
  delete S2;

  return 0;
}