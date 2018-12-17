#include "TreeNode.h"
#include <assert.h>

TreeNode::TreeNode(const Board &board, Player playerID, Player enemyID)
    : playerID(playerID), enemyID(enemyID), visits(0) {
  std::vector<Move> moves = board.getMoves();
  for (Move move : moves) {
    moveUtilities.push_back(UtilityNode<Move>(move));
    children.push_back(NULL);
  }

  for (unsigned int i = 0; i < moves.size(); i++) {
    moveThreadCounts.push_back(0);
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
  int moveIndex = mab.getChoice(moveThreadCounts, moveUtilities, visits);
  moveThreadCounts[moveIndex] += 1;
  Move move = moveUtilities[moveIndex].object;
  board.makeMove(move, playerID);
  bool isLeaf = false;

  if (children[moveIndex] == NULL) {
    children[moveIndex] = new TreeNode(board, enemyID, playerID);
    isLeaf = true;
  }

  return std::tuple<int, TreeNode *, bool>(moveIndex, children[moveIndex],
                                           isLeaf);
}

void TreeNode::updateUtility(int moveIndex, float utility) {
  std::lock_guard<std::mutex> g(node_mtx);
  assert(moveThreadCounts[moveIndex] > 0);
  moveUtilities[moveIndex].updateUtility(utility);
  moveThreadCounts[moveIndex] -= 1;
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

// returns how confident the AI is that it is in a winning position.
// Confidence is computed as the win rate of the move with the highest utility
float TreeNode::getConfidence() {
  assert(moveUtilities.size() > 0);

  float result = 0.;
  std::lock_guard<std::mutex> g(node_mtx);
  for (const UtilityNode<Move> &moveUtility : moveUtilities) {
    float averageUtility = moveUtility.getAverageUtility();
    if (averageUtility > result) {
      result = averageUtility;
    }
  }

  return result;
}

bool TreeNode::isLeaf() {
  std::lock_guard<std::mutex> g(node_mtx);
  return moveUtilities.size() == 0;
}

size_t TreeNode::getNumMoves() {
  std::lock_guard<std::mutex> g(node_mtx);
  return moveUtilities.size();
}
