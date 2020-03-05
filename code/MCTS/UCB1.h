#ifndef UCB1_h
#define UCB1_h

#include <assert.h>
#include <math.h>
#include <limits>
#include <sstream>
#include <string>
#include <unordered_map>
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

  double getSearchPrior(
      const T &obj, const std::unordered_map<T, double> &searchPriorMap) const {
    return searchPriorMap.count(obj) == 0 ? 0. : searchPriorMap.at(obj);
  }

  int getChoice(const vector<unsigned int> &moveThreadCounts,
                const vector<UtilityNode<T>> &nodes,
                const std::unordered_map<T, double> &searchPriorMap,
                int numTrials) const {
    assert(nodes.size() > 0);
    float priorSum = 0.;
    for (unsigned int i = 0; i < nodes.size(); i++) {
      priorSum += nodes[i].priorWeight +
                  getSearchPrior(nodes[i].object, searchPriorMap);
    }

    float confidenceNumerator = log(numTrials + 1) * confidenceConstant;
    float priorNumerator =
        0.25 * priorSum * sqrt(log(numTrials + 1) / (numTrials + 1));
    float threadDiversionDenominator = sqrt(numTrials + 1);
    int bestNodeIndex = -1;
    float bestScore = -std::numeric_limits<float>::max();
    for (unsigned int i = 0; i < nodes.size(); i++) {
      unsigned int nodeTrials = nodes[i].numTrials + moveThreadCounts[i];
      if (nodeTrials == 0) return i;
      float uncertaintyScore =
          sqrt(confidenceNumerator / nodeTrials) -
          (moveThreadCounts[i] / threadDiversionDenominator);
      float priorScore =
          priorNumerator / (nodes[i].priorWeight +
                            getSearchPrior(nodes[i].object, searchPriorMap));
      float nodeScore =
          nodes[i].getAverageUtility() + uncertaintyScore - priorScore;
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

 private:
  float confidenceConstant;
};

#endif
