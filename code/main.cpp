
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
  for (int i = _argc - 2; i >= 0; i -= 1) {
    if (strcmp(_argv[i], option_name) == 0) return _argv[i + 1];
  }
  return default_value;
}

int get_option_int(const char* option_name, int default_value) {
  for (int i = _argc - 2; i >= 0; i -= 1) {
    if (strcmp(_argv[i], option_name) == 0) return atoi(_argv[i + 1]);
  }
  return default_value;
}

float get_option_float(const char* option_name, float default_value) {
  for (int i = _argc - 2; i >= 0; i -= 1) {
    if (strcmp(_argv[i], option_name) == 0)
      return static_cast<float>(atof(_argv[i + 1]));
  }
  return default_value;
}

bool get_flag(const char* flag_name) {
  for (int i = _argc - 1; i >= 0; i -= 1) {
    if (strcmp(_argv[i], flag_name) == 0) {
      return true;
    }
  }
  return false;
}

int main(int argc, const char* argv[]) {
  _argc = argc - 1;
  _argv = argv + 1;

  int msPerMove = 1000 * get_option_int("-t", 1);
  int playout_threads = get_option_int("-p", 1);
  int iter_threads = get_option_int("-i", 1);
  int board_size = get_option_int("-s", 9);
  bool against_user = get_flag("-u");

  BoardTest::test();

  if (against_user) {
    Strategy* S = new MCTS(msPerMove, playout_threads, iter_threads, 2.0, 1.0);
    Strategy* user = new UserPlayer();
    int result = Game::runGame(S, user, board_size, -1, false);
    if (result == 0) {
      std::cout << "USER LOST\n";
    } else {
      std::cout << "USER WON\n";
    }

    delete S;
    delete user;
  } else {
    std::vector<Strategy*> strategies;
    for (double p = 0.2; p < 1.01; p += 0.2) {
      Strategy* S = new MCTS(msPerMove, 1, 16, 2.0, p);
      strategies.push_back(S);
    }

    Game::runTournament(strategies, board_size, board_size * board_size);

    for (Strategy* S : strategies) {
      delete S;
    }
  }

  return 0;
}
