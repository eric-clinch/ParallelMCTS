#ifndef TREENODE_H__
#define TREENODE_H__

#include <vector>
#include "Board.h"
#include "MAB.h"
#include "Move.h"
#include "TreeNode.h"
#include "UtilityNode.h"

class TreeNode {
 public:
  TreeNode(const Board &board, Player playerID, Player enemyID)
      : playerID(playerID), enemyID(enemyID) {
    std::vector<Move> moves = board.getMoves();
    for (Move move : moves) {
      moveUtilities.push_back(UtilityNode<Move>(move));
      children.push_back(NULL);
    }
  }

  ~TreeNode() {
    for (TreeNode *child : children) {
      if (child != NULL) {
        delete child;
      }
    }
  }

  std::pair<int, TreeNode &> getAndMakeMove(MAB<Move> &mab, Board &board) {
    int moveIndex = mab.getChoice(moveUtilities, visits);
    Move move = moveUtilities[moveIndex].object;
    board.makeMove(move, playerID);

    if (children[moveIndex] == NULL) {
      children[moveIndex] = new TreeNode(board, enemyID, playerID);
    }

    return std::pair<int, TreeNode &>(moveIndex, *children[moveIndex]);
  }

  void updateUtility(int moveIndex, float utility) {
    moveUtilities[moveIndex].updateUtility(utility);
    visits++;
  }

  const Move getMostVisited() {
    assert(moveUtilities.size() > 0);

    const Move *result = NULL;
    unsigned int resultTrials = 0;

    for (UtilityNode<Move> &moveUtility : moveUtilities) {
      if (moveUtility.numTrials > resultTrials) {
        result = &moveUtility.object;
        resultTrials = moveUtility.numTrials;
      }
    }

    assert(result != NULL);
    return *result;
  }

  bool isLeaf() { return visits == 0 || moveUtilities.size() == 0; }

  size_t getNumMoves() { return moveUtilities.size(); }

 private:
  Player playerID;
  Player enemyID;

  std::vector<UtilityNode<Move>> moveUtilities;
  std::vector<TreeNode *> children;
  size_t visits;
};

#endif  // TREENODE_H__