#include "TreeNode.h"
#include <assert.h>

TreeNode::TreeNode(const Board &board, Player playerID, Player enemyID)
    : playerID(playerID), enemyID(enemyID), visits(0) {
  std::vector<Move> moves = board.getMoves();
  for (Move move : moves) {
    moveUtilities.push_back(UtilityNode<Move>(move));
    children.push_back(NULL);
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
  int moveIndex = mab.getChoice(moveUtilities, visits);
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
  moveUtilities[moveIndex].updateUtility(utility);
  visits++;
}

const Move TreeNode::getMostVisited() {
  assert(moveUtilities.size() > 0);

  const Move *result = NULL;
  unsigned int resultTrials = 0;

  for (const UtilityNode<Move> &moveUtility : moveUtilities) {
    if (result == NULL || moveUtility.numTrials > resultTrials) {
      result = &moveUtility.object;
      resultTrials = moveUtility.numTrials;
    }
  }

  assert(result != NULL);
  return *result;
}

bool TreeNode::isLeaf() { return moveUtilities.size() == 0; }

size_t TreeNode::getNumMoves() { return moveUtilities.size(); }
