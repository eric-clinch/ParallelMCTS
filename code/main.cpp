
#include <assert.h>
#include <atomic>
#include <cstring>
#include <iostream>
#include "Board.h"
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
    if (strcmp(_argv[i], option_name) == 0) return (float)atof(_argv[i + 1]);
  }
  return default_value;
}

void zeroFill(bool* seen, int length) {
  for (int i = 0; i < length; i++) seen[i] = false;
}

Board test0() {
  Board board(19, 19);
  board.update(0, 1, 'W');
  board.update(1, 1, 'B');
  // board.update(2, 1, 'W');
  board.update(1, 0, 'W');
  return board;
}

Board test1() {
  Board board;
  board.update(0, 0, 'B');
  board.update(0, 1, 'W');
  board.update(0, 2, 'B');
  board.update(1, 0, 'W');
  board.update(1, 1, 'B');
  board.update(1, 2, 'W');
  board.update(2, 0, 'B');
  board.update(2, 1, 'W');
  board.update(2, 2, 'B');
  return board;
}

Board test2() {
  Board board;
  board.update(0, 1, 'W');
  board.update(1, 1, 'B');
  board.update(2, 1, 'W');
  board.update(1, 0, 'W');
  return board;
}

Board test3() {
  Board board;
  board.update(0, 1, 'W');
  board.update(0, 2, 'W');
  board.update(1, 0, 'W');
  board.update(1, 1, 'B');
  board.update(1, 2, 'B');
  board.update(1, 3, 'W');
  board.update(2, 0, 'W');
  board.update(2, 1, 'B');
  board.update(2, 2, 'B');
  board.update(2, 3, 'W');
  return board;
}

Board test4() {
  Board board;
  board.update(0, 1, 'W');
  board.update(0, 2, 'W');
  board.update(0, 3, 'W');
  board.update(1, 0, 'W');
  board.update(1, 1, 'B');
  board.update(1, 2, 'B');
  board.update(1, 3, 'B');
  board.update(1, 4, 'W');
  board.update(2, 0, 'W');
  board.update(2, 1, 'B');
  board.update(2, 2, '-');
  board.update(2, 3, 'B');
  board.update(2, 4, 'W');
  board.update(3, 0, 'W');
  board.update(3, 1, 'B');
  board.update(3, 2, 'B');
  board.update(3, 3, 'B');
  board.update(3, 4, 'W');

  board.update(4, 1, 'W');
  board.update(4, 2, 'W');
  board.update(4, 3, 'W');
  return board;
}

Board test5() {
  Board board;
  board.update(0, 0, 'W');
  board.update(0, 1, 'W');
  board.update(0, 2, 'W');
  board.update(1, 0, 'B');
  board.update(1, 1, 'B');
  board.update(1, 2, 'B');
  board.update(0, 3, 'B');
  return board;
}

Board test6() {
  Board board;
  board.update(0, 0, 'W');
  board.update(0, 1, 'W');
  board.update(1, 1, 'W');
  board.update(0, 2, 'W');
  board.update(1, 0, 'B');
  board.update(1, 2, 'B');
  board.update(0, 3, 'B');
  return board;
}

Board test7() {
  Board board;
  board.update(5, 5, 'W');
  board.update(6, 5, 'W');
  board.update(7, 5, 'W');
  board.update(9, 5, 'W');

  board.update(6, 6, 'W');
  board.update(7, 6, 'W');
  board.update(8, 6, 'W');
  board.update(9, 6, 'W');

  board.update(7, 7, 'W');
  board.update(8, 7, 'W');

  board.update(8, 8, 'W');

  board.update(4, 5, 'B');
  board.update(5, 4, 'B');
  board.update(6, 4, 'B');
  board.update(7, 4, 'B');
  board.update(8, 5, 'B');
  board.update(9, 4, 'B');

  board.update(5, 6, 'B');
  board.update(6, 7, 'B');
  board.update(7, 8, 'B');
  board.update(8, 9, 'B');
  board.update(9, 7, 'B');
  board.update(9, 8, 'B');
  board.update(10, 5, 'B');
  board.update(10, 6, 'B');
  return board;
}

int main(int argc, const char* argv[]) {
  _argc = argc - 1;
  _argv = argv + 1;

  int secondsPerMove = 1000 * get_option_int("-t", 1);
  int threads = get_option_int("-p", 1);

  bool* seen = new bool[19 * 19];
  zeroFill(seen, 19 * 19);

  Board board0 = test0();
  bool answer = board0.capture(1, 1, 'B', 'W', seen, 0);
  assert(answer == false);

  std::cout << board0.toString();
  Move move(2, 1);
  
  board0.makeMove(move, P0);
  std::cout << board0.toString();

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
  delete[] seen;

  Strategy* S0 = new MCTS(secondsPerMove, threads);
  Strategy* S1 = new MCTS(secondsPerMove, threads);
  Strategy* user = new UserPlayer();

  Game* G = new Game(S0, user, 1);
  int s0_wins = G->runGame();
  std::cout << "S0 won " << s0_wins << std::endl;

  delete S0;
  delete S1;

  return 0;
}
