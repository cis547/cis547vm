#ifndef POINTER_ANALYSIS_H
#define POINTER_ANALYSIS_H

#include "llvm/IR/Function.h"
#include <set>

using namespace llvm;

namespace dataflow {
//===----------------------------------------------------------------------===//
// Pointer Analysis
//===----------------------------------------------------------------------===//

using PointsToSet = std::set<std::string>;
using PointsToInfo = std::map<std::string, PointsToSet>;
class PointerAnalysis {
public:
  PointerAnalysis(Function &F);
  bool alias(std::string &Ptr1, std::string &Ptr2) const;

private:
  PointsToInfo PointsTo;
};
}; // namespace dataflow

#endif // POINTER_ANALYSIS_H
