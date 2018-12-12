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
  TreeNode(const Board &board, Player playerID, Player enemyID,
           bool trackThreads);

  ~TreeNode();

  std::tuple<int, TreeNode *, bool> getAndMakeMove(MAB<Move> &mab,
                                                   Board &board);

  void updateUtility(int moveIndex, float utility);

  const Move getMostVisited();
  float getConfidence();
  bool isLeaf();

  size_t getNumMoves();

  int getCount() {
    int s = 0;
    for (const UtilityNode<Move> node : moveUtilities) {
      s += node.numTrials;
    }
    return s;
  }

 private:
  Player playerID;
  Player enemyID;

  std::vector<UtilityNode<Move>> moveUtilities;
  std::vector<unsigned int> moveThreadCounts;
  std::vector<TreeNode *> children;

  mutable std::mutex node_mtx;
  size_t visits;

  bool trackThreads;
};

#endif  // TREENODE_H__
