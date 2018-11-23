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

MCTS::MCTS(int64_t msPerMove, unsigned int playoutThreads)
    : msPerMove(msPerMove),
      playoutThreads(playoutThreads),
      mab(new UCB1<Move>(2.0)) {}

MCTS::~MCTS() { delete mab; }

const Move MCTS::getMove(Board &board, Player playerID, Player enemyID) {
  size_t iterations = 0;

  TreeNode root(board, playerID, enemyID);

  Board boardCopy(board.getWidth(), board.getHeight());
  int64_t startTime = Tools::getTime();
  for (int64_t currentTime = startTime; currentTime - startTime < msPerMove;
       currentTime = Tools::getTime()) {
    board.copyInto(boardCopy);
    MCTSIteration(boardCopy, playerID, enemyID, root);
    iterations++;
  }

  std::cout << "iterations: " << iterations << "\n";

  return root.getMostVisited();
}

// perform one iteration of the MCTS algorithm starting from the given node
float MCTS::MCTSIteration(Board &board, Player playerID, Player enemyID,
                          TreeNode &node) {
  if (node.isLeaf()) {
    return performPlayouts(board, playerID, enemyID);
  }

  int moveIndex;
  TreeNode *child;
  bool childIsLeaf;
  std::tie(moveIndex, child, childIsLeaf) = node.getAndMakeMove(*mab, board);

  float result;
  if (childIsLeaf) {
    result = performPlayouts(board, playerID, enemyID);
  } else {
    result = 1. - MCTSIteration(board, enemyID, playerID, *child);
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

float MCTS::performPlayouts(Board &board, Player playerID, Player enemyID) {
  pthread_t threads[playoutThreads - 1];

  playoutJob arg;
  arg.board = &board;
  arg.playerID = playerID;
  arg.enemyID = enemyID;

  for (int i = 0; i < playoutThreads - 1; i++) {
    pthread_create(&threads[i], NULL, parallelPlayout, &arg);
  }

  size_t sum = playout(&board, playerID, enemyID);

  for (int i = 0; i < playoutThreads - 1; i++) {
    int *result;
    pthread_join(threads[i], reinterpret_cast<void **>(&result));
    sum += (size_t)result;
  }

  return static_cast<float>(sum) / static_cast<float>(playoutThreads);
}

void *MCTS::parallelPlayout(void *arg) {
  playoutJob *playoutInfo = reinterpret_cast<playoutJob *>(arg);
  int result =
      playout(playoutInfo->board, playoutInfo->playerID, playoutInfo->enemyID);
  return reinterpret_cast<void *>(result);
}

int MCTS::playout(Board *originalBoard, Player playerID, Player enemyID) {
  Board board = originalBoard->getCopy();

  Player currentPlayer = playerID;
  while (!board.gameIsOver()) {
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
