#ifndef MCTS_H__
#define MCTS_H__

#include <random>
#include "Board.h"
#include "MAB.h"
#include "Move.h"
#include "Strategy.h"
#include "TreeNode.h"

class MCTS : public Strategy {
 public:
  MCTS();
  ~MCTS();
  const Move getMove(Board &board, Player playerID, Player enemyID);

 private:
  // perform one iteration of the MCTS algorithm starting from the given node
  float MCTSIteration(Board &board, Player playerID, Player enemyID,
                      TreeNode &node);

  const Move sampleMove(std::vector<Move> moves);
  float playout(Board &board, Player playerID, Player enemyID);

 private:
  MAB<Move> *mab;

  std::mt19937 rng;
  std::uniform_real_distribution<> uni;
};

#endif  // MCTS_H__