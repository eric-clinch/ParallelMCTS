#ifndef BOARDTEST_H__
#define BOARDTEST_H__

class Board;

class BoardTest {
 public:
  static void test() {
    Board board0 = test0();
    board0.seenZeroFill();
    bool answer = board0.capture(1, 1, 'B', 'W', 0);
    assert(answer == false);

    Board board1 = test1();
    board1.seenZeroFill();
    answer = board1.capture(1, 1, 'B', 'W', 0);
    assert(answer == true);
    assert(1 == board1.removeStones(1, 1, 'B'));

    Board board2 = test2();
    board2.seenZeroFill();
    answer = board2.capture(1, 1, 'B', 'W', 0);
    assert(answer == false);

    Board board3 = test3();
    board3.seenZeroFill();
    answer = board3.capture(1, 1, 'B', 'W', 0);
    assert(answer == false);

    Board board4 = test4();
    board4.seenZeroFill();
    answer = board4.capture(2, 2, 'B', 'W', 0);
    assert(answer == false);

    Board board5 = test5();
    board5.seenZeroFill();
    answer = board5.capture(0, 0, 'W', 'B', 0);
    assert(answer == true);
    assert(3 == board5.removeStones(0, 0, 'W'));

    Board board6 = test6();
    // std::cout << board6.toString();

    board6.seenZeroFill();
    answer = board6.capture(0, 0, 'W', 'B', 0);
    assert(answer == false);
    bool answer2 = board6.capture(1, 0, 'B', 'W', 0);
    assert(answer2 == false);
    assert(4 == board6.removeStones(0, 0, 'W'));

    Board board7 = test7();
    board7.seenZeroFill();
    answer = board7.capture(7, 6, 'W', 'B', 0);
    assert(answer == true);
    assert(11 == board7.removeStones(7, 6, 'W'));
  }

 private:
  static Board test0() {
    Board board(19, 19);
    board.update(0, 1, 'W');
    board.update(1, 1, 'B');
    board.update(1, 0, 'W');
    return board;
  }

  static Board test1() {
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

  static Board test2() {
    Board board;
    board.update(0, 1, 'W');
    board.update(1, 1, 'B');
    board.update(2, 1, 'W');
    board.update(1, 0, 'W');
    return board;
  }

  static Board test3() {
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

  static Board test4() {
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

  static Board test5() {
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

  static Board test6() {
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

  static Board test7() {
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
};

#endif  // BOARDTEST_H__k
