#include "MCTS.h"
#include <assert.h>
#include <pthread.h>
#include <chrono>
#include <iostream>
#include "UCB1.h"
#include "library/Tools.h"

std::random_device MCTS::rd;
std::mt19937 MCTS::rng(rd());
std::uniform_real_distribution<> MCTS::uni(0, 1);

MCTS::MCTS(int64_t msPerMove, unsigned int playoutThreads,
           unsigned int iterationThreads, double explorationConstant,
           double playoutPercent)
    : msPerMove(msPerMove),
      playoutThreads(playoutThreads),
      iterationThreads(iterationThreads),
      mab(new UCB1<Move>(explorationConstant)),
      explorationConstant(explorationConstant),
      playoutPercent(playoutPercent) {
  assert(0. <= playoutPercent && playoutPercent <= 1.);
}

MCTS::~MCTS() { delete mab; }

string MCTS::toString() {
  ostringstream stringStream;
  stringStream << "MCTS(" << msPerMove << ", " << playoutThreads << ", "
               << iterationThreads << ", " << explorationConstant << ", "
               << playoutPercent << ")";
  return stringStream.str();
}

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
  args.iterations = 0;
  args.mcts = this;
  pthread_t iterationWorkers[iterationThreads - 1];
  for (size_t i = 0; i < iterationThreads - 1; i++) {
    pthread_create(&iterationWorkers[i], NULL, getMoveHelper,
                   reinterpret_cast<void *>(&args));
  }

  getMoveHelper(reinterpret_cast<void *>(&args));

  for (size_t i = 0; i < iterationThreads - 1; i++) {
    pthread_join(iterationWorkers[i], NULL);
  }

  std::cout << toString() << " iterations: " << args.iterations
            << " playouts: " << args.iterations * playoutThreads << std::endl;

  confidence = root.getConfidence();
  std::cout << "confidence: " << confidence << std::endl;
  if (confidence < 0.01) {
    // if it is estimated that there is a < 10% chance of winning, concede
    std::cout << "conceding" << std::endl;
    return Move();
  } else {
    return root.getMostVisited();
  }
}

void *MCTS::getMoveHelper(void *arg) {
  size_t iterations = 0;
  mainArg *iterationArg = reinterpret_cast<mainArg *>(arg);
  const Board *board = iterationArg->board;
  unsigned int playoutThreads = iterationArg->workerThreads;
  Player playerID = iterationArg->playerID;
  Player enemyID = iterationArg->enemyID;
  TreeNode *root = iterationArg->node;
  int64_t msPerMove = iterationArg->ms;
  MCTS *mcts = iterationArg->mcts;

  pthread_t workers[playoutThreads - 1];
  workerArg groupInfo;
  groupInfo.playoutPercent = mcts->playoutPercent;
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
    std::unordered_map<Move, double> emptySearchPrior;
    mcts->MCTSIteration(copyBoard, playerID, enemyID, *root, &groupInfo,
                        emptySearchPrior);
    iterations++;
  }

  groupInfo.board = NULL;
  groupInfo.activeCount = 1;
  for (unsigned int i = 0; i < (unsigned int)(playoutThreads - 1); i++) {
    pthread_join(workers[i], NULL);
  }

  iterationArg->iterations.fetch_add(iterations);
  return NULL;
}

// perform one iteration of the MCTS algorithm starting from the given node
float MCTS::MCTSIteration(Board &board, Player playerID, Player enemyID,
                          TreeNode &node, workerArg *groupInfo,
                          std::unordered_map<Move, double> &searchPriorMap) {
  if (node.isLeaf()) {
    return performPlayouts(board, playerID, enemyID, groupInfo);
  }

  int moveIndex;
  TreeNode *child;
  bool childIsLeaf;

  std::tie(moveIndex, child, childIsLeaf) =
      node.getAndMakeMove(*mab, board, searchPriorMap);
  if (searchPriorMap.size() == 0) {
    searchPriorMap = node.getSearchPriorMap();
  }
  float result;
  if (childIsLeaf) {
    result = performPlayouts(board, playerID, enemyID, groupInfo);
  } else {
    result = 1. - MCTSIteration(board, enemyID, playerID, *child, groupInfo,
                                searchPriorMap);
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
  size_t result = playout(&board, playerID, enemyID, groupInfo->playoutPercent);

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
    while (groupInfo->activeCount == 0) {
    }

    if (groupInfo->board == NULL) {
      return NULL;  // the master has signaled us to exit
    }

    int result = playout(groupInfo->board, groupInfo->playerID,
                         groupInfo->enemyID, groupInfo->playoutPercent);

    if (result > 0) {
      groupInfo->winCount.fetch_add(result);
    }

    groupInfo->activeCount.fetch_sub(
        1);  // alert the master that we are complete

    while (groupInfo->barrierFlag == flagSense) {
    }  // wait for all other workers to complete
    flagSense = 1 - flagSense;
  }
}

int MCTS::playout(Board *originalBoard, Player playerID, Player enemyID,
                  double playoutPercent) {
  Board board = originalBoard->getCopy();

  Player currentPlayer = playerID;

  size_t iters = 0;
  size_t maxIters =
      static_cast<int>(playoutPercent * (board.getWidth() * board.getHeight()));
  std::vector<Move> moves;
  while (!board.gameIsOver() && iters < maxIters) {
    iters++;

    board.getContestedTerritoryMoves(moves);
    if (moves.size() == 0) {
      break;
    }
    const Move randomMove = sampleMove(moves);
    moves.clear();
    board.makeMove(randomMove, currentPlayer);

    currentPlayer = currentPlayer == playerID ? enemyID : playerID;
  }

  return board.playerScore(playerID) > board.playerScore(enemyID) ? 1 : 0;
}

double MCTS::getConfidence() const { return confidence; }
