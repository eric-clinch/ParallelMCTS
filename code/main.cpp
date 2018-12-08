
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
  int playout_threads = get_option_int("-p", 1);
  int iter_threads = get_option_int("-i", 1);
  bool* seen = new bool[19 * 19];
  zeroFill(seen, 19 * 19);

  Board board0 = test0();
  bool answer = board0.capture(1, 1, 'B', 'W', seen, 0);
  assert(answer == false);

  // std::cout << board0.toString();
  Move move(2, 1);

  board0.makeMove(move, P0);
  // std::cout << board0.toString();

  zeroFill(seen, 19 * 19);
  Board board1 = test1();
  answer = board1.capture(1, 1, 'B', 'W', seen, 0);
  assert(answer == true);
  assert(1 == board1.removeStones(1, 1, 'B'));

  zeroFill(seen, 19 * 19);
  Board board2 = test2();
  answer = board2.capture(1, 1, 'B', 'W', seen, 0);
  assert(answer == false);

  zeroFill(seen, 19 * 19);
  Board board3 = test3();
  answer = board3.capture(1, 1, 'B', 'W', seen, 0);
  assert(answer == false);

  zeroFill(seen, 19 * 19);
  Board board4 = test4();
  answer = board4.capture(2, 2, 'B', 'W', seen, 0);
  assert(answer == false);

  zeroFill(seen, 19 * 19);
  Board board5 = test5();
  answer = board5.capture(0, 0, 'W', 'B', seen, 0);
  assert(answer == true);
  assert(3 == board5.removeStones(0, 0, 'W'));

  zeroFill(seen, 19 * 19);
  Board board6 = test6();
  answer = board6.capture(0, 0, 'W', 'B', seen, 0);
  assert(answer == false);
  zeroFill(seen, 19 * 19);
  bool answer2 = board6.capture(1, 0, 'B', 'W', seen, 0);
  assert(answer2 == false);
  assert(4 == board6.removeStones(0, 0, 'W'));
  zeroFill(seen, 19 * 19);

  zeroFill(seen, 19 * 19);
  Board board7 = test7();
  answer = board7.capture(7, 6, 'W', 'B', seen, 0);
  assert(answer == true);
  assert(11 == board7.removeStones(7, 6, 'W'));
  std::cout << "Test cases passed!\n";
  delete[] seen;

  Strategy* S0 = new MCTS(msPerMove, playout_threads, iter_threads);
  Strategy* user = new UserPlayer();

  Game* G = new Game(S0, user, 1);
  int s0_wins = G->runGame();
  std::cout << "S0 won " << s0_wins << std::endl;

  delete S0;
  // delete S1;

  return 0;
}
