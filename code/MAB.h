#ifndef MAB_h
#define MAB_h

#include <string>
#include <vector>
#include "UtilityNode.h"

template <class T>
class MAB {
 public:
  virtual ~MAB() {}

  // returns the index of the chosen element
  virtual int getChoice(const vector<bool> &availMoves,
                        const vector<UtilityNode<T>> &nodes, int numTrials) = 0;
  virtual string toString() = 0;
};

#endif  // MAB_h
