#ifndef MAB_h
#define MAB_h

#include <string>
#include <unordered_map>
#include <vector>
#include "UtilityNode.h"

template <class T>
class MAB {
 public:
  virtual ~MAB() {}

  // returns the index of the chosen element
  virtual int getChoice(const vector<unsigned int> &moveThreadCounts,
                        const vector<UtilityNode<T>> &nodes,
                        const std::unordered_map<T, double> &searchPriorMap,
                        int numTrials) const = 0;
  virtual string toString() = 0;
};

#endif  // MAB_h
