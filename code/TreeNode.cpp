#include "TreeNode.h"
#include <assert.h>
#include <algorithm>

TreeNode::TreeNode(const Board &board, Player playerID, Player enemyID)
    : playerID(playerID), enemyID(enemyID), visits(0) {
  // std::vector<Move> moves = board.priorityOrderedMoves();
  std::vector<std::pair<Move, float>> moveAndWeights =
      board.getMovesAndWeights();

  for (const std::pair<Move, float> &moveWeight : moveAndWeights) {
    moveUtilities.push_back(
        UtilityNode<Move>(moveWeight.first, moveWeight.second));
    children.push_back(NULL);
    moveThreadCounts.push_back(0);
  }
}

TreeNode::~TreeNode() {
  for (TreeNode *child : children) {
    if (child != NULL) {
      delete child;
    }
  }
}

std::tuple<int, TreeNode *, bool> TreeNode::getAndMakeMove(
    const MAB<Move> &mab, Board &board,
    const std::unordered_map<Move, double> &searchPriorMap) {
  std::lock_guard<std::mutex> g(node_mtx);
  int moveIndex =
      mab.getChoice(moveThreadCounts, moveUtilities, searchPriorMap, visits);
  moveThreadCounts[moveIndex] += 1;
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

std::unordered_map<Move, double> TreeNode::getSearchPriorMap() const {
  std::unordered_map<Move, double> search_prior_map;
  double visit_min = std::numeric_limits<double>::max();
  double visit_sum = 0.;

  std::lock_guard<std::mutex> g(node_mtx);
  for (const UtilityNode<Move> &moveUtility : moveUtilities) {
    double visits = static_cast<double>(moveUtility.numTrials);
    visit_min = std::min(visits, visit_min);
    visit_sum += visits;
  }

  for (const UtilityNode<Move> &moveUtility : moveUtilities) {
    double visits = static_cast<double>(moveUtility.numTrials);
    double search_prior = visit_min * (visits / visit_sum);
    search_prior_map.insert({moveUtility.object, search_prior});
  }

  return search_prior_map;
}

void TreeNode::updateUtility(int moveIndex, float utility) {
  std::lock_guard<std::mutex> g(node_mtx);
  assert(moveThreadCounts[moveIndex] > 0);
  moveUtilities[moveIndex].updateUtility(utility);
  moveThreadCounts[moveIndex] -= 1;
  visits++;
}

const Move TreeNode::getMostVisited() const {
  assert(moveUtilities.size() > 0);

  const Move *result = NULL;
  unsigned int resultTrials = 0;
  std::lock_guard<std::mutex> g(node_mtx);
  for (const UtilityNode<Move> &moveUtility : moveUtilities) {
    if (result == NULL || moveUtility.numTrials > resultTrials) {
      result = &moveUtility.object;
      resultTrials = moveUtility.numTrials;
    }
  }

  assert(result != NULL);
  return *result;
}

// returns how confident the AI is that it is in a winning position.
// Confidence is computed as the win rate of the move with the most visits
float TreeNode::getConfidence() const {
  assert(moveUtilities.size() > 0);

  unsigned int resultTrials = 0;
  float result = 0.;
  std::lock_guard<std::mutex> g(node_mtx);
  for (const UtilityNode<Move> &moveUtility : moveUtilities) {
    if (moveUtility.numTrials > resultTrials) {
      result = moveUtility.getAverageUtility();
      resultTrials = moveUtility.numTrials;
    }
  }

  return result;
}

bool TreeNode::isLeaf() const {
  std::lock_guard<std::mutex> g(node_mtx);
  return moveUtilities.size() == 0;
}

size_t TreeNode::getNumMoves() const {
  std::lock_guard<std::mutex> g(node_mtx);
  return moveUtilities.size();
}
