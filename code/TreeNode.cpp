#include "TreeNode.h"
#include <assert.h>

TreeNode::TreeNode(const Board &board, Player playerID, Player enemyID)
    : playerID(playerID), enemyID(enemyID), visits(0) {
  
  std::lock_guard<std::mutex> g(node_mtx);       
  
  std::vector<Move> moves = board.getMoves();
  for (Move move : moves) {
    moveUtilities.push_back(UtilityNode<Move>(move));
    children.push_back(NULL);
  }
  
  // creating array of moves that are 'available'
  std::vector<bool> movesAvailable;
  for (int i = 0; i < board.getWidth() * board.getHeight(); i++) {
    movesAvailable.push_back(false);
  }
  for (Move move: moves) {
    movesAvailable[move.getRow() * board.getWidth() + move.getCol()] = true;
  }
}

TreeNode::~TreeNode() {
  for (TreeNode *child : children) {
    if (child != NULL) {
      delete child;
    }
  }
}

std::tuple<int, TreeNode *, bool> TreeNode::getAndMakeMove(MAB<Move> &mab,
                                                           Board &board) {
  std::lock_guard<std::mutex> g(node_mtx);
  int moveIndex = mab.getChoice(movesAvailable, moveUtilities, visits);
  Move move = moveUtilities[moveIndex].object;
  board.makeMove(move, playerID);
  bool isLeaf = false;
  // std::lock_guard<std::mutex> g(node_mtx);

  if (children[moveIndex] == NULL) {
    children[moveIndex] = new TreeNode(board, enemyID, playerID);
    isLeaf = true;
  }

  return std::tuple<int, TreeNode *, bool>(moveIndex, children[moveIndex],
                                           isLeaf);
}

void TreeNode::updateUtility(int moveIndex, float utility) {
  std::lock_guard<std::mutex> g(node_mtx);
  moveUtilities[moveIndex].updateUtility(utility);
  visits++;
}

const Move TreeNode::getMostVisited() {
  assert(moveUtilities.size() > 0);

  const Move *result = NULL;
  unsigned int resultTrials = 0;
  std::lock_guard<std::mutex> g(node_mtx);
  for (const UtilityNode<Move> &moveUtility : moveUtilities) {
    if (result == NULL || moveUtility.numTrials > resultTrials) {
      result = &moveUtility.object;
      resultTrials = moveUtility.numTrials;
    }
  }

  assert(result != NULL);
  return *result;
}

bool TreeNode::isLeaf() { 
    std::lock_guard<std::mutex> g(node_mtx);
    return moveUtilities.size();
}

size_t TreeNode::getNumMoves() { 
  std::lock_guard<std::mutex> g(node_mtx);
  return moveUtilities.size(); 
}

const bool TreeNode::isAvailable(Board &board, Move move) {
  int r = move.getRow();
  int c = move.getCol();
  return movesAvailable[r * board.getWidth() + c];
}
