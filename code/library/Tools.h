#ifndef TOOLS_H
#define TOOLS_H

#include <chrono>

namespace Tools {
inline int64_t getTime();
}  // namespace Tools

inline int64_t Tools::getTime() {
  std::chrono::milliseconds ms =
      std::chrono::duration_cast<std::chrono::milliseconds>(
          std::chrono::system_clock::now().time_since_epoch());
  return ms.count();
}

#endif