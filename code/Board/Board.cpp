#include "Board.h"
#include <assert.h>
#include <cstring>
#include <sstream>
#include <stack>
#include <string>
#include <unordered_set>
#include "BoardTest.h"

const char Board::P0STONE = 'B';
const char Board::P1STONE = 'W';
const char Board::BLANK = '-';

Board::Board()
    : width(19),
      height(19),
      lastMovePassed(false),
      gameOver(false),
      prevCaptured(-1, -1) {
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

Board::Board(int w, int h)
    : width(w),
      height(h),
      lastMovePassed(false),
      gameOver(false),
      prevCaptured(-1, -1) {
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

  delete[] seenGrid;
}

void Board::seenZeroFill() const {
  for (int i = 0; i < width * height; i++) seenGrid[i] = false;
}

Board Board::getCopy() const {
  mutex board_lock;
  board_lock.lock();
  Board result(width, height);
  for (int i = 0; i < height; i++) {
    std::memcpy(&result.board[i][0], &board[i][0], width * sizeof(char));
  }
  board_lock.unlock();

  result.P0Stones = P0Stones;
  result.P1Stones = P1Stones;
  result.P0Captures = P0Captures;
  result.P1Captures = P1Captures;
  result.prevCaptured = prevCaptured;

  result.lastMovePassed = lastMovePassed;
  result.gameOver = gameOver;

  assert(result.isValid());
  return result;
}

void Board::copyInto(Board &result) const {
  mutex board_mtx;
  board_mtx.lock();
  for (int i = 0; i < height; i++) {
    std::memcpy(&result.board[i][0], &board[i][0], width * sizeof(char));
  }
  board_mtx.unlock();
  result.P0Stones = P0Stones;
  result.P1Stones = P1Stones;
  result.P0Captures = P0Captures;
  result.P1Captures = P1Captures;
  result.prevCaptured = prevCaptured;

  result.lastMovePassed = lastMovePassed;
  result.gameOver = gameOver;

  assert(result.isValid());
}

bool Board::gameIsOver() const {
  return gameOver || (P0Stones == (unsigned int)(width * height)) ||
         (P1Stones == (unsigned int)(width * height));
}

int Board::getWidth() const { return width; }

int Board::getHeight() const { return height; }

bool Board::isLegal(const Move &move) const {
  if (move.isPass()) {
    return true;  // the pass move is always legal
  } else if (prevCaptured.first == move.getRow() &&
             prevCaptured.second == move.getCol()) {
    return false;
  }
  int row = move.getRow();
  int col = move.getCol();
  return board[row][col] == BLANK;
}

std::vector<Move> Board::getMoves() const {
  std::vector<Move> result;
  result.push_back(Move());  // add the pass move
  for (int row = 0; row < height; row++) {
    for (int col = 0; col < width; col++) {
      if (isLegal(Move(row, col))) {
        result.push_back(Move(row, col));
      }
    }
  }

  return result;
}

std::vector<std::pair<char, char>> Board::blackStones() const {
  std::vector<std::pair<char, char>> stones;
  for (int row = 0; row < height; row++) {
    for (int col = 0; col < width; col++) {
      if (board[row][col] == P0STONE) {
        stones.push_back({row, col});
      }
    }
  }
  return stones;
}

std::vector<std::pair<char, char>> Board::whiteStones() const {
  std::vector<std::pair<char, char>> stones;
  for (int row = 0; row < height; row++) {
    for (int col = 0; col < width; col++) {
      if (board[row][col] == P1STONE) {
        stones.push_back({row, col});
      }
    }
  }
  return stones;
}

void Board::getContestedTerritoryMoves(std::vector<Move> &result) {
  if (P0Stones == 0 || P1Stones == 0) {
    // this is a bit of a degenerate case. Just return all the legal moves
    result = getMoves();
    return;
  }

  seenZeroFill();

  // Get all moves that aren't in another player's territory
  std::vector<std::pair<int, int>> territoryCells;
  for (int row = 0; row < height; row++) {
    for (int col = 0; col < width; col++) {
      if (board[row][col] == BLANK && !seenGrid[row * width + col]) {
        std::pair<Player, bool> player_controlled =
            floodFillTerritories(row, col, territoryCells);
        bool controlled = player_controlled.second;
        if (!controlled) {
          for (const std::pair<int, int> &cell : territoryCells) {
            const Move move(cell.first, cell.second);
            if (isLegal(move)) {
              result.push_back(move);
            }
          }
        }
        territoryCells.clear();
      }
    }
  }

  if (result.size() == 0) {
    // add the pass move if there are no other reasonable moves to make
    result.push_back(Move());
  }
}

int Board::neighborCount(int row, int col) const {
  int result = 0;
  if (row > 0 && board[row - 1][col] != BLANK) result++;
  if (col > 0 && board[row][col - 1] != BLANK) result++;
  if (row + 1 < height && board[row + 1][col] != BLANK) result++;
  if (col + 1 < width && board[row][col + 1] != BLANK) result++;
  return result;
}

std::vector<Move> Board::priorityOrderedMoves() const {
  seenZeroFill();
  std::vector<Move> moves;
  // Add all the moves that neighbor an existing piece
  for (int row = 0; row < height; row++) {
    for (int col = 0; col < width; col++) {
      if (board[row][col] == BLANK && neighborCount(row, col) > 0) {
        Move move(row, col);
        if (isLegal(move)) {
          moves.push_back(move);
          seenGrid[row * width + col] = true;
        }
      }
    }
  }

  // Add all the remaining moves
  for (int row = 0; row < height; row++) {
    for (int col = 0; col < width; col++) {
      if (board[row][col] == BLANK && !seenGrid[row * width + col]) {
        Move move(row, col);
        if (isLegal(move)) {
          moves.push_back(move);
          seenGrid[row * width + col] = true;
        }
      }
    }
  }

  return moves;
}

std::vector<std::pair<Move, float>> Board::getMovesAndWeights() const {
  std::vector<Move> moves(getMoves());
  std::vector<std::pair<Move, float>> moveWeights(moves.size());
  for (int i = 0; i < moves.size(); i++) {
    Move &move = moves[i];
    float weight = 1.;

    if (move.row >= 2 && move.col >= 2 && width - move.col > 2 &&
        height - move.row > 2) {
      weight += 1.;
      if (move.row == 2 || move.col == 2 || width - move.col == 3 ||
          height - move.row == 3) {
        weight += 1.;
      }
    }

    if (move.isPass()) {
      weight = 0.1;
    } else {
      int neighbors = neighborCount(move.row, move.col);
      if (1 <= neighbors && neighbors < 4) {
        weight += 1;
      }
    }

    moveWeights[i] = std::pair<Move, float>(move, weight);
  }

  return moveWeights;
}

int Board::makeMove(const Move &move, Player playerID) {
  assert(isValid());
  assert(isLegal(move));
  if (gameOver) {
    return 0;  // the game is over, moves shouldn't be made
  }

  if (move.isPass()) {
    if (lastMovePassed) {
      gameOver =
          true;  // both players passed consecutively, so the game is over
    } else {
      lastMovePassed = true;
    }
    return 0;
  } else {
    lastMovePassed = false;  // this is not a pass move
  }
  prevCaptured = {-1, -1};
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

  // because current row, col is for stone, we need to check 4 adjacent squares
  // for islands of enemyStone that are completely surrounded
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

  seenZeroFill();

  if (capture(row, col, stone, enemyStone, 0)) {
    enemy_points = removeStones(row, col, stone);
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
  if (prevCaptured.first == -1 && prevCaptured.second == -1) {
    prevCaptured.first = row;
    prevCaptured.second = col;
  } else {
    prevCaptured.first = -1;
    prevCaptured.second = -1;
  }
  // prevChanges.push_back(std::pair<int, int>(row, col));
  int new_count = 1;
  if (row - 1 >= 0) new_count += removeStones(row - 1, col, stone);
  if (row + 1 < height) new_count += removeStones(row + 1, col, stone);
  if (col - 1 >= 0) new_count += removeStones(row, col - 1, stone);
  if (col + 1 < width) new_count += removeStones(row, col + 1, stone);
  return new_count;
}

// scored by Chinese rules (area scoring)
unsigned int Board::playerScore(Player playerID) {
  unsigned int score;
  std::pair<unsigned int, unsigned int> territories = getTerritories();
  if (playerID == P0) {
    score = territories.first + P0Captures + P0Stones;
  } else {
    score = territories.second + P1Captures + P1Stones;
  }

  return score;
}

Player Board::getWinner() {
  unsigned int P0Score = playerScore(P0);
  unsigned int P1Score = playerScore(P1);
  return P0Score > P1Score ? P0 : P1;
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

std::pair<unsigned int, unsigned int> Board::getTerritories() {
  seenZeroFill();

  unsigned int P0Territory = 0;
  unsigned int P1Territory = 0;

  for (int row = 0; row < height; row++) {
    for (int col = 0; col < width; col++) {
      if (!seenGrid[row * width + col] && board[row][col] == BLANK) {
        std::pair<Player, unsigned int> player_territory =
            floodFillTerritories(row, col);
        Player player = player_territory.first;
        unsigned int territory = player_territory.second;
        if (player == P0) {
          P0Territory += territory;
        } else {
          P1Territory += territory;
        }
      }
    }
  }

  return std::pair<unsigned int, unsigned int>(P0Territory, P1Territory);
}

// given a coordinate on the board, runs flood fill from that location to
// determine what "territory" that cell falls in. if only one player's stones
// are seen during this process, then that player controls this territory. In
// that case it returns the pair (P, T) where P is the player that controls the
// territory and T is the size of the territory. If neither player controls the
// territory, it simply returns (P0, 0).
std::pair<Player, unsigned int> Board::floodFillTerritories(int row, int col) {
  std::stack<std::pair<int, int>> cellStack;
  cellStack.push(std::pair<int, int>(row, col));
  seenGrid[row * width + col] = true;

  unsigned int territory = 0;
  bool seenP0 = false;
  bool seenP1 = false;

  std::pair<int, int> directions[] = {{1, 0}, {0, 1}, {-1, 0}, {0, -1}};

  while (!cellStack.empty()) {
    territory++;

    std::pair<int, int> row_col = cellStack.top();
    cellStack.pop();
    row = row_col.first;
    col = row_col.second;

    for (std::pair<int, int> direction : directions) {
      int newRow = row + direction.first;
      int newCol = col + direction.second;

      if (0 <= newRow && newRow < height && 0 <= newCol && newCol < width) {
        if (board[newRow][newCol] == P0STONE) {
          seenP0 = true;
        } else if (board[newRow][newCol] == P1STONE) {
          seenP1 = true;
        } else if (!seenGrid[newRow * width + newCol]) {
          seenGrid[newRow * width + newCol] = true;
          cellStack.push(std::pair<int, int>(newRow, newCol));
        }
      }
    }
  }

  if (seenP0 && !seenP1) {
    return std::pair<Player, unsigned int>(
        P0, territory);  // P0 controls this territory
  } else if (seenP1 && !seenP0) {
    return std::pair<Player, unsigned int>(
        P1, territory);  // P1 controls this territory
  } else {
    return std::pair<Player, unsigned int>(
        P0, 0);  // neither player controls this territory
  }
}

// same as the floodFillTerritories above, except that it accepts a vector
// and appends to it every (row, col) pair that is in this territory. Also
// instead of returning the territory size, returns a bool of whether or
// not the area is controlled by the given player
std::pair<Player, bool> Board::floodFillTerritories(
    int row, int col, std::vector<std::pair<int, int>> &cells) {
  assert(cells.size() == 0);

  cells.push_back(std::pair<int, int>(row, col));

  seenGrid[row * width + col] = true;

  bool seenP0 = false;
  bool seenP1 = false;

  std::pair<int, int> directions[] = {{1, 0}, {0, 1}, {-1, 0}, {0, -1}};

  for (unsigned int i = 0; i < cells.size(); i++) {
    std::pair<int, int> row_col = cells[i];
    row = row_col.first;
    col = row_col.second;

    for (std::pair<int, int> direction : directions) {
      int newRow = row + direction.first;
      int newCol = col + direction.second;

      if (0 <= newRow && newRow < height && 0 <= newCol && newCol < width) {
        if (board[newRow][newCol] == P0STONE) {
          seenP0 = true;
        } else if (board[newRow][newCol] == P1STONE) {
          seenP1 = true;
        } else if (!seenGrid[newRow * width + newCol]) {
          seenGrid[newRow * width + newCol] = true;
          cells.push_back(std::pair<int, int>(newRow, newCol));
        }
      }
    }
  }

  if (seenP0 && !seenP1) {
    return std::pair<Player, unsigned int>(P0,
                                           true);  // P0 controls this territory
  } else if (seenP1 && !seenP0) {
    return std::pair<Player, unsigned int>(P1,
                                           true);  // P1 controls this territory
  } else {
    return std::pair<Player, unsigned int>(
        P0, false);  // neither player controls this territory
  }
}

void Board::update(int row, int col, char stone) {
  board[row][col] = stone;
  if (stone == P0STONE) {
    P0Stones++;
  } else if (stone == P1STONE) {
    P1Stones++;
  }
}

std::string Board::toString() {
  ostringstream stringStream;
  for (int row = 0; row < height; row++) {
    for (int col = 0; col < width; col++) {
      stringStream << board[row][col] << " ";
    }
    stringStream << "\n";
  }

  std::pair<unsigned int, unsigned int> territories = getTerritories();
  stringStream << P0STONE << "'s territory: " << territories.first << ", ";
  stringStream << P1STONE << "'s territory: " << territories.second
               << std::endl;

  stringStream << P0STONE << "'s captures: " << P0Captures << ", ";
  stringStream << P1STONE << "'s captures: " << P1Captures << std::endl;

  stringStream << std::endl;

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
