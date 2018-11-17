#include <iostream>
#include "Board.h"
// #include <assert.h>

void zeroFill(bool* seen, int length) {
  for (int i = 0; i < length; i++) seen[i] = false;
}

Board* test0() {
  Board* board = new Board();
  board->update(0, 1, 'W');
  board->update(1, 1, 'B');
  board->update(2, 1, 'W');
  board->update(1, 0, 'W');
  // board->update(1, 2, 'W');
  return board;
}

Board* test1() {
  Board* board = new Board();
  board->update(0, 0, 'B');
  board->update(0, 1, 'W');
  board->update(0, 2, 'B');
  board->update(1, 0, 'W');
  board->update(1, 1, 'B');
  board->update(1, 2, 'W');
  board->update(2, 0, 'B');
  board->update(2, 1, 'W');
  board->update(2, 2, 'B');
  return board;
}

Board* test2() {
  Board* board = new Board();
  board->update(0, 1, 'W');
  board->update(1, 1, 'B');
  board->update(2, 1, 'W');
  board->update(1, 0, 'W');
  return board;
}

Board* test3() {
  Board* board = new Board();
  board->update(0, 1, 'W');
  board->update(0, 2, 'W');
  board->update(1, 0, 'W');
  board->update(1, 1, 'B');
  board->update(1, 2, 'B');
  board->update(1, 3, 'W');
  board->update(2, 0, 'W');
  board->update(2, 1, 'B');
  board->update(2, 2, 'B');
  board->update(2, 3, 'W');
  return board;
}

Board* test4() {
  Board* board = new Board();
  board->update(0, 1, 'W');
  board->update(0, 2, 'W');
  board->update(0, 3, 'W');
  board->update(1, 0, 'W');
  board->update(1, 1, 'B');
  board->update(1, 2, 'B');
  board->update(1, 3, 'B');
  board->update(1, 4, 'W');
  board->update(2, 0, 'W');
  board->update(2, 1, 'B');
  board->update(2, 2, '-');
  board->update(2, 3, 'B');
  board->update(2, 4, 'W');
  board->update(3, 0, 'W');
  board->update(3, 1, 'B');
  board->update(3, 2, 'B');
  board->update(3, 3, 'B');
  board->update(3, 4, 'W');

  board->update(4, 1, 'W');
  board->update(4, 2, 'W');
  board->update(4, 3, 'W');
  return board;
}

Board* test5() {
  Board* board = new Board();
  board->update(0, 0, 'W');
  board->update(0, 1, 'W');
  board->update(0, 2, 'W');
  board->update(1, 0, 'B');
  board->update(1, 1, 'B');
  board->update(1, 2, 'B');
  board->update(0, 3, 'B');
  return board;
}

Board* test6() {
  Board* board = new Board();
  board->update(0, 0, 'W');
  board->update(0, 1, 'W');
  board->update(1, 1, 'W');
  board->update(0, 2, 'W');
  board->update(1, 0, 'B');
  board->update(1, 2, 'B');
  board->update(1, 2, 'B');
  board->update(0, 3, 'B');
  return board;
}

Board* test7() {
  Board* board = new Board();
  board->update(5, 5, 'W');
  board->update(6, 5, 'W');
  board->update(7, 5, 'W');
  board->update(9, 5, 'W');

  board->update(6, 6, 'W');
  board->update(7, 6, 'W');
  board->update(8, 6, 'W');
  board->update(9, 6, 'W');

  board->update(7, 7, 'W');
  board->update(8, 7, 'W');

  board->update(8, 8, 'W');

  board->update(4, 5, 'B');
  board->update(5, 4, 'B');
  board->update(6, 4, 'B');
  board->update(7, 4, 'B');
  board->update(8, 5, 'B');
  board->update(9, 4, 'B');

  board->update(5, 6, 'B');
  board->update(6, 7, 'B');
  board->update(7, 8, 'B');
  board->update(8, 9, 'B');
  board->update(9, 7, 'B');
  board->update(9, 8, 'B');
  board->update(10, 5, 'B');
  board->update(10, 6, 'B');
  return board;
}

int main() {
  bool* seen = (bool*)malloc(19 * 19 * sizeof(bool));
  Board* board0 = test0();
  bool answer = board0->capture(1, 1, 'B', 'W', seen);
  assert(answer == false);

  zeroFill(seen, 19 * 19);
  Board* board1 = test1();
  answer = board1->capture(1, 1, 'B', 'W', seen);
  assert(answer == true);
  assert(1 == board1->removeStones(1, 1, 'B'));

  zeroFill(seen, 19 * 19);
  Board* board2 = test2();
  answer = board2->capture(1, 1, 'B', 'W', seen);
  assert(answer == false);

  zeroFill(seen, 19 * 19);
  Board* board3 = test3();
  answer = board3->capture(1, 1, 'B', 'W', seen);
  assert(answer == false);

  zeroFill(seen, 19 * 19);
  Board* board4 = test4();
  answer = board4->capture(2, 2, 'B', 'W', seen);
  assert(answer == false);

  zeroFill(seen, 19 * 19);
  Board* board5 = test5();
  answer = board5->capture(0, 0, 'W', 'B', seen);
  assert(answer == true);
  // std::cout << board5->removeStones(0, 0, 'W', 0) << std::endl;
  assert(3 == board5->removeStones(0, 0, 'W'));

  zeroFill(seen, 19 * 19);
  Board* board6 = test6();
  answer = board6->capture(1, 0, 'W', 'B', seen);
  assert(answer == true);
  assert(4 == board6->removeStones(0, 1, 'W'));
  zeroFill(seen, 19 * 19);

  Board* board7 = test7();
  answer = board7->capture(7, 6, 'W', 'B', seen);
  assert(answer == true);
  assert(11 == board7->removeStones(7, 6, 'W'));
  std::cout << answer << std::endl;
  return 0;
}
