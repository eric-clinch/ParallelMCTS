#ifndef UTILITYNODE_h
#define UTILITYNODE_h

#include <assert.h>
#include <math.h>
#include <limits>

using namespace std;

template <class T>
class UtilityNode {
 public:
  float totalUtility;
  const T object;
  unsigned int numTrials;
  const float priorWeight;

  UtilityNode() {}

  UtilityNode(const T &object, float priorWeight)
      : totalUtility(0.),
        object(object),
        numTrials(0),
        priorWeight(priorWeight) {}

  explicit UtilityNode(const T &object) : UtilityNode(object, 1.) {}

  float getAverageUtility() const {
    if (numTrials == 0) {
      return numeric_limits<float>::infinity();
    }
    return totalUtility / numTrials;
  }

  void updateUtility(float trialUtility) {
    totalUtility += trialUtility;
    numTrials += 1;
  }
};

#endif
