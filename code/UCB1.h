#ifndef UCB1_h
#define UCB1_h

#include <assert.h>
#include <math.h>
#include <sstream>
#include <string>
#include <vector>
#include "MAB.h"

using namespace std;

template <class T>
class UCB1 : public MAB<T> {
 private:
  float confidenceConstant;

 public:
  explicit UCB1(float confidenceConstant) {
    this->confidenceConstant = confidenceConstant;
  }

  int getChoice(const vector<UtilityNode<T>> &nodes, int numTrials) {
    float confidenceNumerator = log(numTrials + 1) * confidenceConstant;
    assert(nodes.size() > 0);
    int bestNodeIndex = 0;
    float bestScore = nodes[0].getAverageUtility() +
                      sqrt(confidenceNumerator / nodes[0].numTrials);
    for (int i = 1; i < nodes.size(); i++) {
      float nodeScore = nodes[i].getAverageUtility() +
                        sqrt(confidenceNumerator / nodes[i].numTrials);
      if (nodeScore > bestScore) {
        bestScore = nodeScore;
        bestNodeIndex = i;
      }
    }
    return bestNodeIndex;
  }

  int getChoice(const vector<UtilityNode<T> *> &nodes, int numTrials) {
    float confidenceNumerator = log(numTrials) * confidenceConstant;
    assert(nodes.size() > 0);
    int bestNodeIndex = 0;
    float bestScore = nodes[0]->getAverageUtility() +
                      sqrt(confidenceNumerator / nodes[0]->numTrials);
    for (int i = 1; i < nodes.size(); i++) {
      float nodeScore = nodes[i]->getAverageUtility() +
                        sqrt(confidenceNumerator / nodes[i]->numTrials);
      if (nodeScore > bestScore) {
        bestScore = nodeScore;
        bestNodeIndex = i;
      }
    }
    return bestNodeIndex;
  }

  string toString() {
    ostringstream stringStream;
    stringStream << "UCB1(" << confidenceConstant << ")";
    return stringStream.str();
  }
};

#endif
