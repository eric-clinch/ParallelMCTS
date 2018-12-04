#include "Board.h"
#include "BoardTest.h"
#include <assert.h>
#include <cstring>
#include <sstream>
#include <string>

const char Board::P0STONE = 'W';
const char Board::P1STONE = 'B';
const char Board::BLANK = '-';

Board::Board() {
  width = 19;
  height = 19;
  board = new char *[height];
  for (int i = 0; i < height; i++) {
    board[i] = new char[width];
    for (int j = 0; j < width; j++) {
      board[i][j] = BLANK;
    }
  }

  seenGrid = new bool[height * width];

  P0Stones = 0;
  P1Stones = 0;
  P0Captures = 0;
  P1Captures = 0;
}

Board::Board(int w, int h) {
  width = w;
  height = h;
  board = new char *[height];
  for (int i = 0; i < height; i++) {
    board[i] = new char[width];
    for (int j = 0; j < width; j++) {
      board[i][j] = BLANK;
    }
  }

  seenGrid = new bool[height * width];

  P0Stones = 0;
  P1Stones = 0;
  P0Captures = 0;
  P1Captures = 0;
}

Board::~Board() {
  for (int row = 0; row < height; row++) {
    delete[] board[row];
  }
  delete[] board;
}

void Board::seenZeroFill() {
  for (int i = 0; i < width * height; i++) seenGrid[i] = false;
}

Board Board::getCopy() const {
  Board result(width, height);
  for (int i = 0; i < height; i++) {
    std::memcpy(&result.board[i][0], &board[i][0], width * sizeof(char));
  }

  result.P0Stones = P0Stones;
  result.P1Stones = P1Stones;
  result.P0Captures = P0Captures;
  result.P1Captures = P1Captures;

  assert(result.isValid());
  return result;
}

void Board::copyInto(Board &result) const {
  for (int i = 0; i < height; i++) {
    std::memcpy(&result.board[i][0], &board[i][0], width * sizeof(char));
  }

  result.P0Stones = P0Stones;
  result.P1Stones = P1Stones;
  result.P0Captures = P0Captures;
  result.P1Captures = P1Captures;

  assert(result.isValid());
}

inline bool Board::gameIsOver() const {
  return (P0Stones == width * height) || (P1Stones == width * height);
}

inline int Board::getWidth() const { return width; }

inline int Board::getHeight() const { return height; }

bool Board::isLegal(const Move &move, Player playerID) const {
  int row = move.getRow();
  int col = move.getCol();
  if (board[row][col] != BLANK) return false;
  return true;
}

std::vector<Move> Board::getMoves() const {
  std::vector<Move> result;
  for (int row = 0; row < height; row++) {
    for (int col = 0; col < width; col++) {
      if (board[row][col] == BLANK) {
        result.push_back(Move(row, col));
      }
    }
  }

  return result;
}

int Board::makeMove(const Move &move, Player playerID) {
  assert(isValid());

  char stone;
  char enemyStone;
  if (playerID == P0) {
    stone = P0STONE;
    enemyStone = P1STONE;
  } else {
    stone = P1STONE;
    enemyStone = P0STONE;
  }

  int row = move.getRow();
  int col = move.getCol();
  // adding stone to board
  board[row][col] = stone;

  // remove surrounded islands
  int enemy_points = 0;
  int points = 0;

  seenZeroFill();

  // because current row, col is for stone, we need to check 4 adjacent squares
  // for islands of enemyStone that are completely surrounded
  if (capture(row, col, stone, enemyStone, 0)) {
    enemy_points = removeStones(row, col, stone);
  }

  seenZeroFill();

  if (row > 0 && board[row - 1][col] == enemyStone &&
      capture(row - 1, col, enemyStone, stone, 0)) {
    points += removeStones(row - 1, col, enemyStone);
  }

  seenZeroFill();

  if (row + 1 < height && board[row + 1][col] == enemyStone &&
      capture(row + 1, col, enemyStone, stone, 0)) {
    points += removeStones(row + 1, col, enemyStone);
  }

  seenZeroFill();

  if (col > 0 && board[row][col - 1] == enemyStone &&
      capture(row, col - 1, enemyStone, stone, 0)) {
    points += removeStones(row, col - 1, enemyStone);
  }

  seenZeroFill();

  if (col + 1 < width && board[row][col + 1] == enemyStone &&
      capture(row, col + 1, enemyStone, stone, 0)) {
    points += removeStones(row, col + 1, enemyStone);
  }

  if (playerID == P0) {
    P0Stones += 1 - enemy_points;
    P0Captures += points;
    P1Stones -= points;
    P1Captures += enemy_points;
  } else {
    P1Stones += 1 - enemy_points;
    P1Captures += points;
    P0Stones -= points;
    P0Captures += enemy_points;
  }

  assert(isValid());
  return points;
}

// check if current location is in an enclosed mass
bool Board::capture(int row, int col, char stone, char enemyStone, int iter) {
  if (seenGrid[row * width + col]) return true;
  seenGrid[row * width + col] = true;
  if (board[row][col] != P0STONE && board[row][col] != P1STONE) {
    return false;
  } else if (board[row][col] == enemyStone) {
    return true;
  }
  if (row > 0) {
    if (!capture(row - 1, col, stone, enemyStone, iter + 1)) {
      return false;
    }
  }
  if (col + 1 < width) {
    if (!capture(row, col + 1, stone, enemyStone, iter + 1)) {
      return false;
    }
  }
  if (row + 1 < height) {
    if (!capture(row + 1, col, stone, enemyStone, iter + 1)) {
      return false;
    }
  }
  if (col - 1 >= 0) {
    if (!capture(row, col - 1, stone, enemyStone, iter + 1)) {
      return false;
    }
  }
  return true;
}

// if current location is currently surrounded, remove all stones inside circle
int Board::removeStones(int row, int col, char stone) {
  if (board[row][col] != stone) return 0;
  board[row][col] = BLANK;
  int new_count = 1;
  if (row - 1 >= 0) new_count += removeStones(row - 1, col, stone);
  if (row + 1 < height) new_count += removeStones(row + 1, col, stone);
  if (col - 1 >= 0) new_count += removeStones(row, col - 1, stone);
  if (col + 1 < width) new_count += removeStones(row, col + 1, stone);
  return new_count;
}

unsigned int Board::playerScore(Player playerID) const {
  char playerStone = playerID == P0 ? P0STONE : P1STONE;
  return playerID == P0 ? P0Stones + P0Captures : P1Stones + P1Captures;
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

void Board::update(int row, int col, char stone) {
  board[row][col] = stone;
  if (stone == P0STONE) {
    P0Stones++;
  } else if (stone == P1STONE) {
    P1Stones++;
  }
}

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

bool Board::isValid() const {
  size_t P0StoneCount = 0;
  size_t P1StoneCount = 0;

  for (int row = 0; row < height; row++) {
    for (int col = 0; col < width; col++) {
      if (board[row][col] == P0STONE) {
        P0StoneCount++;
      } else if (board[row][col] == P1STONE) {
        P1StoneCount++;
      }
    }
  }

  return P0StoneCount == P0Stones && P1StoneCount == P1Stones;
}
