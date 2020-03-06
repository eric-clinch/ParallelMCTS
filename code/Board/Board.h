
#ifndef BOARD_h
#define BOARD_h

#define NDEBUG

#include <assert.h>
#include <math.h>
#include <iostream>
#include <mutex>
#include <vector>
#include "Move.h"

using namespace std;

class BoardTest;

enum Player { P0 = 0, P1 = 1 };

class Board {
 public:
  Board();
  Board(int width, int height);
  ~Board();
  bool operator==(const Board &other) const;

  int getWidth() { return width; }
  int getHeight() { return height; }

  Board getCopy() const;
  void copyInto(Board &result) const;

  void seenZeroFill() const;

  bool gameIsOver() const;
  unsigned int playerScore(Player playerID);
  Player getWinner();

  int getWidth() const;
  int getHeight() const;

  bool isLegal(const Move &move) const;

  std::vector<Move> getMoves() const;
  std::vector<Move> priorityOrderedMoves() const;
  std::vector<std::pair<Move, float>> getMovesAndWeights() const;
  void getContestedTerritoryMoves(std::vector<Move> &result);
  int makeMove(const Move &move, Player playerID);
  bool capture(int i, int j, char stone, char enemyStone, int iter);
  int removeStones(int x, int y, char stone);

  std::string toString();
  void update(int i, int j, char stone);

  std::vector<std::pair<char, char>> blackStones() const;
  std::vector<std::pair<char, char>> whiteStones() const;

  friend class BoardTest;

 public:
  static const char P0STONE;
  static const char P1STONE;
  static const char BLANK;

 private:
  unsigned int stoneCount(char stone) const;
  std::pair<unsigned int, unsigned int> getTerritories();
  std::pair<Player, unsigned int> floodFillTerritories(int row, int col);
  std::pair<Player, bool> floodFillTerritories(
      int row, int col, std::vector<std::pair<int, int>> &cells);

  bool isValid() const;

  // Returns the number of stones neighboring the given position
  int neighborCount(int row, int col) const;

 private:
  int width, height;
  char **board;
  volatile bool *seenGrid;

  size_t P0Stones;
  size_t P1Stones;

  size_t P0Captures;  // the number of stones that P0 has captured
  size_t P1Captures;  // the number of stones that P1 has captured

  bool lastMovePassed;
  bool gameOver;
  std::pair<int, int> prevCaptured;  // used to enforce rule of Ko
};

#endif
