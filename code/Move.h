#ifndef MOVE_h
#define MOVE_h

#include <assert.h>
#include <string>

class Move {
 public:
  char row;
  char col;

 public:
  Move() : row(-1), col(-1) {}  // the pass move
  Move(char row, char col) : row(row), col(col) {}
  Move(const Move& other) : row(other.row), col(other.col) {}

  int getRow() const { return row; }

  int getCol() const { return col; }

  bool isPass() const { return row == -1 && col == -1; }

  std::string toString() const;

  bool operator==(const Move& other) const {
    return this->row == other.row && this->col == other.col;
  }
};

namespace std {
template <>
struct hash<Move> {
  size_t operator()(const Move& k) const { return k.row + 19 * k.col; }
};
}  // namespace std

#endif
