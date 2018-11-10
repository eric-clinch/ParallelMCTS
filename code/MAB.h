#ifndef MAB_h
#define MAB_h

#include <string>
#include <vector>
#include "UtilityNode.h"

template <class T>
class MAB {
 public:
  // returns the index of the chosen element
  virtual int getChoice(const vector<UtilityNode<T>> &nodes, int numTrials) = 0;
  virtual int getChoice(const vector<UtilityNode<T> *> &nodes,
                        int numTrials) = 0;
  virtual string toString() = 0;
};

#endif  // MAB_h