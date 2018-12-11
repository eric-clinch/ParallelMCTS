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
 public:
  explicit UCB1(float confidenceConstant)
      : confidenceConstant(confidenceConstant) {
    assert(confidenceConstant >= 0.);
  }

  ~UCB1() {}

  int getChoice(const vector<bool> &availMoves,
                const vector<UtilityNode<T>> &nodes, int numTrials) {
    assert(nodes.size() > 0);
    float confidenceNumerator = log(numTrials + 1) * confidenceConstant;
    int bestNodeIndex = -1;
    float bestScore = -1;
    for (int i = 0; i < nodes.size(); i++) {
      if (nodes[i].numTrials == 0 && availMoves[i]) return i;
      float nodeScore = nodes[i].getAverageUtility() +
                        sqrt(confidenceNumerator / nodes[i].numTrials);
      if (nodeScore > bestScore && availMoves[i]) {
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

 private:
  float confidenceConstant;
};

#endif
