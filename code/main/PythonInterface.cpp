#include "Board/Board.h"
#include "Board/Move.h"
#include "Game/Game.h"
#include "Game/Strategy.h"
#include "MCTS/MCTS.h"

typedef struct Stone {
  int row;
  int col;
  char stone_type;
} stone_t;

typedef struct StoneList {
  int length;
  stone_t* stones;
} stone_list_t;

typedef struct MCTSMove {
  int row;
  int col;
  double confidence;
} mcts_move_t;

extern "C" {

Board* NewBoard(int size) { return new Board(size, size); }

int BoardSize(Board* board) { return board->getWidth(); }

bool LegalMove(Board* board, int row, int col) {
  Move move(row, col);
  return board->isLegal(move);
}

void MakeMove(Board* board, int row, int col, int player) {
  Move move(row, col);
  Player move_player = player == 0 ? Player::P0 : Player::P1;
  board->makeMove(move, move_player);
}

stone_list_t StoneList(Board* board) {
  std::vector<std::pair<char, char>> black_stones = board->blackStones();
  std::vector<std::pair<char, char>> white_stones = board->whiteStones();
  stone_list_t result;
  result.length = black_stones.size() + white_stones.size();
  result.stones = new stone_t[result.length];

  int idx = 0;
  while (idx < black_stones.size()) {
    const std::pair<char, char> location = black_stones[idx];
    result.stones[idx].row = location.first;
    result.stones[idx].col = location.second;
    result.stones[idx].stone_type = Board::P0STONE;
    idx++;
  }
  while (idx - black_stones.size() < white_stones.size()) {
    int w_idx = idx - black_stones.size();
    const std::pair<char, char> location = white_stones[w_idx];
    result.stones[idx].row = location.first;
    result.stones[idx].col = location.second;
    result.stones[idx].stone_type = Board::P1STONE;
    idx++;
  }

  return result;
}

int PlayerScore(Board* board, int player_i) {
  Player player = player_i == 0 ? Player::P0 : Player::P1;
  return board->playerScore(player);
}

mcts_move_t GetMCTSMove(Board* board, int threads, int msPerMove,
                        int player_i) {
  MCTS mcts(msPerMove, 1, threads, 0.2, 0.75);
  Player player = player_i == 0 ? Player::P0 : Player::P1;
  Player opponent = player == Player::P0 ? Player::P1 : Player::P0;
  Move move = mcts.getMove(*board, player, opponent);
  mcts_move_t result;
  result.row = move.row;
  result.col = move.col;
  result.confidence = mcts.getConfidence();
  return result;
}
}
