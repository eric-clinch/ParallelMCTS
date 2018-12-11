#ifndef MCTS_H__
#define MCTS_H__

#include <atomic>
#include <random>
// #include <mutex>
#include "Board.h"
#include "MAB.h"
#include "Move.h"
#include "Strategy.h"
#include "TreeNode.h"

struct workerArg {
  Board *board;
  Player playerID;
  Player enemyID;
  size_t workers;

  volatile std::atomic<size_t> activeCount;
  std::atomic<size_t> winCount;
  volatile char barrierFlag;  // sense-reversing flag
};

// for the purposes of iteration parallelization
struct mainArg {
  void *mctsObj;
  const Board *board;
  Player playerID;
  Player enemyID;
  TreeNode *node;
  unsigned int workerThreads;
  unsigned int iterationThreads;
  int64_t ms;
};

class MCTS : public Strategy {
 public:
  MCTS(int64_t msPerMove, unsigned int playoutThreads,
       unsigned int iterationThreads);
  ~MCTS();
  virtual const Move getMove(const Board &board, Player playerID,
                             Player enemyID);

 private:
  // perform one iteration of the MCTS algorithm starting from the given node
  static void *getMoveHelper(void *arg);
  float MCTSIteration(Board &board, Player playerID, Player enemyID,
                      TreeNode &node, workerArg *groupInfo);

  // static void *MCTSIterationWorker(void *args);
  float performPlayouts(Board &board, Player playerID, Player enemyID,
                        workerArg *groupInfo);
  static int playout(Board *board, Player playerID, Player enemyID);
  static void *playoutWorker(void *arg);
  static const Move sampleMove(std::vector<Move> &moves);

 private:
  int64_t msPerMove;
  unsigned int playoutThreads;
  unsigned int iterationThreads;
  MAB<Move> *mab;

  static std::random_device rd;
  static std::mt19937 rng;
  static std::uniform_real_distribution<> uni;
};

#endif  // MCTS_H__
