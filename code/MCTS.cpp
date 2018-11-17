#include "MCTS.h"
#include "UCB1.h"

MCTS::MCTS() : mab(new UCB1<Move>(2.0)) {
  std::random_device rd;
  std::mt19937 rng(rd());
  std::uniform_real_distribution<> uni(0, 1);
}

MCTS::~MCTS() { delete mab; }

const Move MCTS::getMove(Board &board, Player playerID, Player enemyID) {
  size_t iterations = 1000;

  TreeNode root(board, playerID, enemyID);

  for (int i = 0; i < iterations; i++) {
    Board boardCopy = board.getCopy();
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

  std::pair<int, TreeNode &> moveIndex_newNode =
      node.getAndMakeMove(*mab, board);
  int moveIndex = moveIndex_newNode.first;
  TreeNode &newNode = moveIndex_newNode.second;

  int result = 1. - MCTSIteration(board, enemyID, playerID, newNode);
  node.updateUtility(moveIndex, result);

  return result;
}

const Move MCTS::sampleMove(std::vector<Move> moves) {
  assert(moves.size() > 0);
  double sample = uni(rng);
  int resultIndex = static_cast<int>(sample * moves.size());
  return moves[resultIndex];
}

float MCTS::playout(Board &board, Player playerID, Player enemyID) {
  Player currentPlayer = playerID;
  while (!board.gameIsOver()) {
    const Move randomMove = sampleMove(board.getMoves());
    board.makeMove(randomMove, currentPlayer);

    currentPlayer = currentPlayer == playerID ? enemyID : playerID;
  }

  return 1.0;  // TODO: return the appropriate value after board.won() is
               // written
}
