#ifndef DIV_ZERO_ANALYSIS_H
#define DIV_ZERO_ANALYSIS_H

#include "DataflowAnalysis.h"

namespace dataflow {
struct DivZeroAnalysis : public DataflowAnalysis {
  static char ID;
  DivZeroAnalysis() : DataflowAnalysis(ID) {}

protected:
  void transfer(Instruction *I, const Memory *In, Memory *NOut,
                PointerAnalysis *PA, SetVector<Value *> PointerSet) override;

  void doAnalysis(Function &F, PointerAnalysis *PA) override;

  void flowIn(Instruction *I, Memory *In);

  void flowOut(Instruction *I, Memory *In, Memory *NOut,  SetVector <Instruction *> &);


  bool check(Instruction *I) override;

  std::string getAnalysisName() override { return "DivZero"; }
};
} // namespace dataflow

#endif // REF_DIV_ZERO_ANALYSIS_H
