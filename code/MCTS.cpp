#include "MCTS.h"
#include <assert.h>
#include <iostream>
#include "UCB1.h"

MCTS::MCTS() : mab(new UCB1<Move>(2.0)), rd(), rng(rd()), uni(0, 1) {}

MCTS::~MCTS() { delete mab; }

const Move MCTS::getMove(Board &board, Player playerID, Player enemyID) {
  size_t iterations = 10000;

  TreeNode root(board, playerID, enemyID);

  Board boardCopy(board.getWidth(), board.getHeight());
  for (size_t i = 0; i < iterations; i++) {
    board.copyInto(boardCopy);
    MCTSIteration(boardCopy, playerID, enemyID, root);
  }

  return root.getMostVisited();
}

// perform one iteration of the MCTS algorithm starting from the given node
float MCTS::MCTSIteration(Board &board, Player playerID, Player enemyID,
                          TreeNode &node) {
  if (node.isLeaf()) {
    return playout(board, playerID, enemyID);
  }

  int moveIndex;
  TreeNode *child;
  bool childIsLeaf;
  std::tie(moveIndex, child, childIsLeaf) = node.getAndMakeMove(*mab, board);

  float result;
  if (childIsLeaf) {
    result = playout(board, playerID, enemyID);
  } else {
    result = 1. - MCTSIteration(board, enemyID, playerID, *child);
  }
  node.updateUtility(moveIndex, result);

  return result;
}

const Move MCTS::sampleMove(std::vector<Move> &moves) {
  assert(moves.size() > 0);
  double sample = uni(rng);
  int resultIndex = static_cast<int>(sample * moves.size());
  return moves[resultIndex];
}

float MCTS::playout(Board &board, Player playerID, Player enemyID) {
  Player currentPlayer = playerID;
  while (!board.gameIsOver()) {
    std::vector<Move> moves(board.getMoves());
    if (moves.size() == 0) {
      break;
    }
    const Move randomMove = sampleMove(moves);
    board.makeMove(randomMove, currentPlayer);

    currentPlayer = currentPlayer == playerID ? enemyID : playerID;
  }

  return board.playerScore(playerID) > board.playerScore(enemyID) ? 1. : 0.;
}
