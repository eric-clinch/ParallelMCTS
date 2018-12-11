#include "MCTS.h"
#include <assert.h>
#include <pthread.h>
#include <chrono>
#include <iostream>
#include "Tools.h"
#include "UCB1.h"

std::random_device MCTS::rd;
std::mt19937 MCTS::rng(rd());
std::uniform_real_distribution<> MCTS::uni(0, 1);

MCTS::MCTS(int64_t msPerMove, unsigned int playoutThreads,
           unsigned int iterationThreads)
    : msPerMove(msPerMove),
      playoutThreads(playoutThreads),
      iterationThreads(iterationThreads),
      mab(new UCB1<Move>(2.0)) {}

MCTS::~MCTS() { delete mab; }

const Move MCTS::getMove(const Board &board, Player playerID, Player enemyID) {
  TreeNode root(board, playerID, enemyID);

  mainArg args;
  args.board = &board;
  args.playerID = playerID;
  args.enemyID = enemyID;
  args.node = &root;
  args.workerThreads = playoutThreads;
  args.iterationThreads = iterationThreads;
  args.ms = msPerMove;
  args.mctsObj = reinterpret_cast<void *>(this);
  pthread_t iterationWorkers[iterationThreads - 1];
  for (size_t i = 0; i < iterationThreads - 1; i++) {
    pthread_create(&iterationWorkers[i], NULL, getMoveHelper,
                   reinterpret_cast<void *>(&args));
  }

  getMoveHelper(reinterpret_cast<void *>(&args));

  for (size_t i = 0; i < iterationThreads - 1; i++) {
    pthread_join(iterationWorkers[i], NULL);
  }
  return root.getMostVisited();
}

void *MCTS::getMoveHelper(void *arg) {
  size_t iterations = 0;
  mainArg *iterationArg = reinterpret_cast<mainArg *>(arg);
  const Board *board = iterationArg->board;
  int playoutThreads = iterationArg->workerThreads;
  Player playerID = iterationArg->playerID;
  Player enemyID = iterationArg->enemyID;
  TreeNode *root = iterationArg->node;
  int64_t msPerMove = iterationArg->ms;
  MCTS *mctsObject = reinterpret_cast<MCTS *>(iterationArg->mctsObj);

  pthread_t workers[playoutThreads - 1];
  workerArg groupInfo;
  groupInfo.workers = playoutThreads - 1;
  groupInfo.activeCount = 0;
  groupInfo.winCount = 0;
  groupInfo.barrierFlag = 0;
  for (unsigned int i = 0; i < (unsigned int)playoutThreads - 1; i++) {
    pthread_create(&workers[i], NULL, playoutWorker,
                   reinterpret_cast<void *>(&groupInfo));
  }
  Board copyBoard(board->getWidth(), board->getHeight());
  int64_t startTime = Tools::getTime();

  for (int64_t currentTime = startTime; currentTime - startTime < msPerMove;
       currentTime = Tools::getTime()) {
    board->copyInto(copyBoard);
    mctsObject->MCTSIteration(copyBoard, playerID, enemyID, *root, &groupInfo);
    iterations++;
  }

  groupInfo.board = NULL;
  groupInfo.activeCount = 1;
  for (unsigned int i = 0; i < playoutThreads - 1; i++) {
    pthread_join(workers[i], NULL);
  }

  std::cout << "iterations: " << iterations << "\n";
  return NULL;
}

// perform one iteration of the MCTS algorithm starting from the given node
float MCTS::MCTSIteration(Board &board, Player playerID, Player enemyID,
                          TreeNode &node, workerArg *groupInfo) {
  if (node.isLeaf()) {
    return performPlayouts(board, playerID, enemyID, groupInfo);
  }
  int moveIndex;
  TreeNode *child;
  bool childIsLeaf;

  std::tie(moveIndex, child, childIsLeaf) = node.getAndMakeMove(*mab, board);
  float result;
  if (childIsLeaf) {
    result = performPlayouts(board, playerID, enemyID, groupInfo);
  } else {
    result = 1. - MCTSIteration(board, enemyID, playerID, *child, groupInfo);
  }
  node.updateUtility(moveIndex, result);
  return result;
}

const Move MCTS::sampleMove(std::vector<Move> &moves) {
  assert(moves.size() > 0);
  double sample = uni(rng);
  int resultIndex = static_cast<int>(sample * moves.size());
  return moves[resultIndex];
}

float MCTS::performPlayouts(Board &board, Player playerID, Player enemyID,
                            workerArg *groupInfo) {
  groupInfo->board = &board;
  groupInfo->playerID = playerID;
  groupInfo->enemyID = enemyID;
  groupInfo->winCount = 0;
  groupInfo->activeCount =
      groupInfo->workers;  // alert all workers to begin playouts
  size_t result = playout(&board, playerID, enemyID);

  // wait for workers to complete
  while (groupInfo->activeCount > 0) {
  }

  groupInfo->barrierFlag = 1 - groupInfo->barrierFlag;
  groupInfo->winCount += result;
  float winCount = static_cast<float>(groupInfo->winCount);

  return winCount / static_cast<float>(playoutThreads);
}

void *MCTS::playoutWorker(void *arg) {
  workerArg *groupInfo = reinterpret_cast<workerArg *>(arg);
  char flagSense = 0;

  while (true) {
    // wait for a playout job
    while (groupInfo->activeCount == 0)
      ;

    if (groupInfo->board == NULL) {
      return NULL;  // the master has signaled us to exit
    }

    int result =
        playout(groupInfo->board, groupInfo->playerID, groupInfo->enemyID);

    if (result > 0) {
      groupInfo->winCount.fetch_add(result);
    }

    groupInfo->activeCount.fetch_sub(
        1);  // alert the master that we are complete

    // size_t temp(groupInfo->activeCount);

    while (groupInfo->barrierFlag == flagSense) {
    }  // wait for all other workers to complete
    flagSense = 1 - flagSense;
  }
}

int MCTS::playout(Board *originalBoard, Player playerID, Player enemyID) {
  Board board = originalBoard->getCopy();

  Player currentPlayer = playerID;

  size_t iters = 0;
  while (!board.gameIsOver() && iters < 300) {
    iters++;
    // std::vector<Move> moves(board.getSmartMoves(playerID, enemyID));
    std::vector<Move> moves(board.getMoves());
    if (moves.size() == 0) {
      break;
    }
    const Move randomMove = sampleMove(moves);
    board.makeMove(randomMove, currentPlayer);

    currentPlayer = currentPlayer == playerID ? enemyID : playerID;
  }

  return board.playerScore(playerID) > board.playerScore(enemyID) ? 1 : 0;
}
