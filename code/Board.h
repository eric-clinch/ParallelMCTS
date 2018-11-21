
#ifndef BOARD_h
#define BOARD_h

#include <assert.h>
#include <math.h>
#include <iostream>
#include <vector>
#include "Move.h"

using namespace std;

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

  virtual bool gameIsOver() const;
  virtual unsigned int playerScore(Player playerID) const;

  virtual inline int getWidth() const;
  virtual inline int getHeight() const;

  virtual bool isLegal(const Move &move, Player playerID) const;

  virtual std::vector<Move> getMoves() const;
  virtual int makeMove(const Move &move, Player playerID);
  virtual bool capture(int i, int j, char stone, char enemyStone,
                       bool *seenGrid);
  virtual int removeStones(int x, int y, char stone);

  virtual std::string toString() const;
  virtual void update(int i, int j, char stone);

 private:
  virtual unsigned int stoneCount(char stone) const;

 private:
  int width, height;
  char **board;
  int currPlayer;
};

#endif
