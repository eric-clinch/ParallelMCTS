
#ifdef _DEBUG
#include <crtdbg.h>
#include <stdlib.h>
#define _CRTDBG_MAP_ALLOC
#define new new (_NORMAL_BLOCK, __FILE__, __LINE__)
// Replace _NORMAL_BLOCK with _CLIENT_BLOCK if you want the
// allocations to be of _CLIENT_BLOCK type
#else
#define NDEBUG
#endif

#ifndef BOARD_h
#define BOARD_h

#include <assert.h>
#include <math.h>
#include <vector>
#include "Move.h"

using namespace std;

enum Player { P0 = 1, P1 = 2 };

class Board {
 private:
  int width, height;
  char **board;

 public:
  Board();
  Board(int width, int height);
  ~Board();
  bool operator==(const Board &other) const;

  virtual Board getCopy() const;
  virtual void copyInto(Board &result) const;

  virtual inline bool gameIsOver() const;
  virtual inline int getWidth() const;
  virtual inline int getHeight() const;

  virtual bool isLegal(const Move &move, Player playerID) const;
  virtual std::vector<Move> getMoves() const;
  virtual void makeMove(const Move &move, Player playerID);

  virtual string toString() const;
};

#endif
