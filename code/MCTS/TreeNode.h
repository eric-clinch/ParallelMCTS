#ifndef TREENODE_H__
#define TREENODE_H__

#include <mutex>
#include <tuple>
#include <unordered_map>
#include <vector>
#include "Board/Board.h"
#include "Board/Move.h"
#include "MAB.h"
#include "TreeNode.h"
#include "UtilityNode.h"

class TreeNode {
 public:
  TreeNode(const Board &board, Player playerID, Player enemyID);

  ~TreeNode();

  std::tuple<int, TreeNode *, bool> getAndMakeMove(
      const MAB<Move> &mab, Board &board,
      const std::unordered_map<Move, double> &searchPriorMap);

  void updateUtility(int moveIndex, float utility);

  const Move getMostVisited() const;
  float getConfidence() const;
  bool isLeaf() const;

  size_t getNumMoves() const;

  std::unordered_map<Move, double> getSearchPriorMap() const;

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
};

#endif  // TREENODE_H__
