
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

  virtual Board getCopy() const;
  virtual void copyInto(Board &result) const;

  virtual void seenZeroFill();

  virtual bool gameIsOver() const;
  virtual unsigned int playerScore(Player playerID);
  virtual Player getWinner();

  virtual int getWidth() const;
  virtual int getHeight() const;

  virtual bool isLegal(const Move &move, Player playerID) const;

  virtual std::vector<Move> getMoves() const;
  virtual void getSmartMoves(std::vector<Move> &result);
  virtual int makeMove(const Move &move, Player playerID);
  virtual bool capture(int i, int j, char stone, char enemyStone, int iter);
  virtual int removeStones(int x, int y, char stone);

  virtual std::string toString();
  virtual void update(int i, int j, char stone);

  friend class BoardTest;

 public:
  static const char P0STONE;
  static const char P1STONE;
  static const char BLANK;

 private:
  virtual unsigned int stoneCount(char stone) const;
  virtual std::pair<unsigned int, unsigned int> getTerritories();
  virtual std::pair<Player, unsigned int> floodFillTerritories(int row,
                                                               int col);
  virtual std::pair<Player, bool> floodFillTerritories(
      int row, int col, std::vector<std::pair<int, int>> &cells);

  bool isValid() const;

 private:
  int width, height;
  char **board;
  bool *seenGrid;

  size_t P0Stones;
  size_t P1Stones;

  size_t P0Captures;  // the number of stones that P0 has captured
  size_t P1Captures;  // the number of stones that P1 has captured

  vector<Move> prevChanges; // used to enforce rule of Ko
  bool lastMovePassed;
  bool gameOver;
};

#endif
