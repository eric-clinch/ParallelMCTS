#include "Board.h"
#include <assert.h>
#include <cstring>
#include <sstream>
#include <string>

char Board::P0STONE = 'W';
char Board::P1STONE = 'B';
char Board::BLANK = '-';

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

  P0Stones = 0;
  P1Stones = 0;
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

  P0Stones = 0;
  P1Stones = 0;
}

Board::~Board() {
  for (int row = 0; row < height; row++) {
    delete[] board[row];
  }
  delete[] board;
}

/*void zeroFill(bool* seen, int length) {
    for (int i = 0; i < length; i++) seen[i] = false;
}*/

Board Board::getCopy() const {
  Board result(width, height);
  for (int i = 0; i < height; i++) {
    std::memcpy(&result.board[i][0], &board[i][0], width * sizeof(char));
  }

  result.P0Stones = P0Stones;
  result.P1Stones = P1Stones;

  return result;
}

void Board::copyInto(Board &result) const {
  for (int i = 0; i < height; i++) {
    std::memcpy(&result.board[i][0], &board[i][0], width * sizeof(char));
  }

  result.P0Stones = P0Stones;
  result.P1Stones = P1Stones;
}

inline bool Board::gameIsOver() const {
  char init = BLANK;
  for (int i = 0; i < height; i++) {
    for (int j = 0; j < width; j++) {
      if (init == BLANK && board[i][j] != BLANK) {
        init = board[i][j];
      }
      if (init != BLANK && board[i][j] != BLANK && board[i][j] != init) {
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
  bool *seenGrid =
      reinterpret_cast<bool *>(malloc(height * width * sizeof(bool)));
  for (int i = 0; i < height * width; i++) {
    seenGrid[i] = false;
  }
  int enemy_points = 0;
  int points = 0;
  // because current row, col is for stone, we need to check 4 adjacent squares
  // for islands of enemyStone that are completely surrounded
  if (capture(row, col, stone, enemyStone, seenGrid)) {
    enemy_points = removeStones(row, col, stone);
  }
  // zeroFill(seenGrid, width * height);
  if (row > 0 && board[row - 1][col] == enemyStone &&
      capture(row - 1, col, enemyStone, stone, seenGrid)) {
    points += removeStones(row - 1, col, enemyStone);
  }
  // zeroFill(seenGrid, widht * height);
  if (row + 1 < height && board[row + 1][col] == enemyStone &&
      capture(row + 1, col, enemyStone, stone, seenGrid)) {
    points += removeStones(row + 1, col, enemyStone);
  }
  if (col > 0 && board[row][col - 1] == enemyStone &&
      capture(row, col - 1, enemyStone, stone, seenGrid)) {
    points += removeStones(row, col - 1, enemyStone);
  }
  if (col + 1 < width && board[row][col + 1] == enemyStone &&
      capture(row, col + 1, enemyStone, stone, seenGrid)) {
    points += removeStones(row, col + 1, enemyStone);
  }
  free(seenGrid);
  return points;
}

// check if current location is in an enclosed mass
bool Board::capture(int row, int col, char stone, char enemyStone,
                    bool *seenGrid) {
  if (seenGrid[row * width + col]) return true;
  seenGrid[row * width + col] = true;
  if (board[row][col] != P0STONE && board[row][col] != P1STONE) {
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
