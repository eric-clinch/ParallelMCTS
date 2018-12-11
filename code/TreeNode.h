#ifndef TREENODE_H__
#define TREENODE_H__

#include <mutex>
#include <tuple>
#include <vector>
#include "Board.h"
#include "MAB.h"
#include "Move.h"
#include "TreeNode.h"
#include "UtilityNode.h"

class TreeNode {
 public:
  TreeNode(const Board &board, Player playerID, Player enemyID);

  ~TreeNode();

  std::tuple<int, TreeNode *, bool> getAndMakeMove(MAB<Move> &mab,
                                                   Board &board);

  void updateUtility(int moveIndex, float utility);

  const Move getMostVisited();
  bool isLeaf();

  size_t getNumMoves();

  int getCount() {
    int s = 0;
    for (const UtilityNode<Move> node : moveUtilities) {
      s += node.numTrials;
    }
    return s;
  }

  const bool isAvailable(int moveIndex);

 private:
  Player playerID;
  Player enemyID;

  std::vector<UtilityNode<Move>> moveUtilities;
  std::vector<TreeNode *> children;
  // for varying MCTSIteration selection (bool vector that says if something is
  // available)
  std::vector<bool> movesAvailable;
  mutable std::mutex node_mtx;
  size_t visits;
};

#endif  // TREENODE_H__
