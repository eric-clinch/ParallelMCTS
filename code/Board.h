
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
#include <algorithm>
#include <sstream>
#include <vector>
#include "Coordinate.h"
#include "Enums.h"
#include "Move.h"

using namespace std;

class Board {
 private:
  int width, height;
  char **board;

  virtual inline char charToRepr(char c);
  virtual inline char reprToChar(char c);

  virtual inline char getNextCellStatus(unsigned char cellX,
                                        unsigned char cellY);

  virtual inline void updateCellStatus(unsigned char row, unsigned char col,
                                       int cellIndex);
  virtual inline void updateRegionStatus(unsigned char row, unsigned char col,
                                         Board &lastRoundBoard);

  virtual inline void deleteBoard();
  virtual inline void copyBoard(Board &blankBoard);

 public:
  Board();
  Board(int width, int height);
  ~Board();
  bool operator==(Board &other);

  virtual Board *getCopy();
  virtual void copyInto(Board &result);

  virtual inline bool gameIsOver();
  virtual inline int getWidth();
  virtual inline int getHeight();

  virtual bool isLegal(const Move &move, Player playerID);
  virtual void makeMove(const Move &move, Player playerID);

  virtual string toString();

  virtual vector<Coordinate> GetCells(char type);
  virtual inline char getCoordinateType(Coordinate &c);
  virtual inline char getCoordinateType(int x, int y);
};

#endif
