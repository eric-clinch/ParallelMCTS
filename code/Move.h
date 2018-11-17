#ifndef MOVE_h
#define MOVE_h

#include <assert.h>
#include <string>
class Move {
 private:
  char row;
  char col;

 public:
  Move() {
    row = 0;
    col = 0;
  }

  Move(char row, char col) : row(row), col(col) {}

  int getRow() const { return row; }

  int getCol() const { return col; }

  std::string toString() const;

  bool operator==(const Move &other) const {
    return this->row == other.row && this->col == other.col;
  }
};

#endif
