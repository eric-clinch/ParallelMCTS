#include "Board.h"
#include <assert.h>
#include <cstring>
#include <sstream>
#include <string>

using namespace std;

Board::Board() {
  width = 19;
  height = 19;
  board = new char *[height];
  for (int i = 0; i < height; i++) {
    board[i] = new char[width];
    for (int j = 0; j < width; j++) {
      board[i][j] = '-';
    }
  }
  currPlayer = 0;
}

Board::Board(int w, int h) {
  width = w;
  height = h;
  board = new char *[height];
  for (int i = 0; i < height; i++) {
    board[i] = new char[width];
    for (int j = 0; j < width; j++) {
      board[i][j] = '-';
    }
  }
  currPlayer = 0;
}

Board::~Board() {
  for (int row = 0; row < height; row++) {
    delete[] board[row];
  }
  delete[] board;
}

Board Board::getCopy() const {
  Board result(width, height);
  for (int i = 0; i < height; i++) {
    std::memcpy(&result.board[i], &board[i], width * sizeof(char));
  }
  return result;
}

void Board::copyInto(Board &result) const {
  for (int i = 0; i < height; i++) {
    std::memcpy(&result.board[i][0], &board[i][0], width * sizeof(char));
  }
}

inline bool Board::gameIsOver() const {
  char init = '-';
  for (int i = 0; i < height; i++) {
    for (int j = 0; j < width; j++) {
      if (init == '-' && board[i][j] != '-') {
        init = board[i][j];
      }
      if (init != '-' && board[i][j] != '-' && board[i][j] != init) {
        return false;
      }
    }
  }
  return true;
}

inline int Board::getWidth() const { return width; }

inline int Board::getHeight() const { return height; }

bool Board::isLegal(const Move &move, Player playerID) const {
  int row = move.getRow();
  int col = move.getCol();
  if (board[row][col] != '-') return false;
  return true;
}

std::vector<Move> Board::getMoves() const {
  std::vector<Move> result;
  for (int row = 0; row < height; row++) {
    for (int col = 0; col < width; col++) {
      if (board[row][col] == '-') {
        result.push_back(Move(row, col));
      }
    }
  }

  return result;
}

int Board::makeMove(const Move &move, Player playerID) {
  char stone = '-';
  char enemyStone = '-';
  if (playerID == P0) {
    stone = 'W';
    enemyStone = 'B';
  } else {
    stone = 'B';
    enemyStone = 'W';
  }

  int row = move.getRow();
  int col = move.getCol();
  // adding stone to board
  board[row][col] = stone;

  // remove surrounded islands
  bool *seenGrid =
      reinterpret_cast<bool *>(malloc(height * width * sizeof(bool)));
  for (int i = 0; i < height * width; i++) {
    seenGrid[i] = false;
  }
  int points = 0;
  if (capture(row, col, stone, enemyStone, seenGrid)) {
    points = removeStones(row, col, stone);
  }
  currPlayer = !playerID;

  free(seenGrid);
  return points;
}

// check if current location is in an enclosed mass
bool Board::capture(int row, int col, char stone, char enemyStone,
                    bool *seenGrid) {
  if (seenGrid[row * width + col]) return true;
  seenGrid[row * width + col] = true;
  if (board[row][col] != 'W' && board[row][col] != 'B') {
    return false;
  } else if (board[row][col] == enemyStone) {
    return true;
  }
  if (row - 1 >= 0) {
    if (!capture(row - 1, col, stone, enemyStone, seenGrid)) {
      return false;
    }
  }
  if (col + 1 < width) {
    if (!capture(row, col + 1, stone, enemyStone, seenGrid)) {
      return false;
    }
  }
  if (row + 1 < height) {
    if (!capture(row + 1, col, stone, enemyStone, seenGrid)) {
      return false;
    }
  }
  if (col - 1 >= 0) {
    if (!capture(row, col - 1, stone, enemyStone, seenGrid)) {
      return false;
    }
  }

  return true;
}

// if current location is currently surrounded, remove all stones inside circle
int Board::removeStones(int row, int col, char stone) {
  if (board[row][col] != stone) return 0;
  board[row][col] = '-';
  int new_count = 1;
  if (row - 1 >= 0) new_count += removeStones(row - 1, col, stone);
  if (row + 1 < height) new_count += removeStones(row + 1, col, stone);
  if (col - 1 >= 0) new_count += removeStones(row, col - 1, stone);
  if (col + 1 < width) new_count += removeStones(row, col + 1, stone);
  return new_count;
}

unsigned int Board::playerScore(Player playerID) const {
  char playerStone = playerID == P0 ? 'W' : 'B';
  return stoneCount(playerStone);
}

unsigned int Board::stoneCount(char stone) const {
  unsigned int count = 0;
  for (int r = 0; r < height; r++) {
    for (int c = 0; c < width; c++) {
      if (board[r][c] == stone) count++;
    }
  }
  return count;
}

void Board::update(int row, int col, char stone) { board[row][col] = stone; }

std::string Board::toString() const {
  ostringstream stringStream;
  for (int row = 0; row < height; row++) {
    for (int col = 0; col < width; col++) {
      stringStream << board[row][col] << " ";
    }
    stringStream << "\n";
  }
  stringStream << "\n";
  return stringStream.str();
}
