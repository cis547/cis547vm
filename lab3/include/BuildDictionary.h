#pragma once

#include "Dictionary.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Constant.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/DataLayout.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/GlobalVariable.h"
#include "llvm/IR/InstrTypes.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/PassManager.h"
#include "llvm/IR/Value.h"

#include <cstdint>
#include <optional>
#include <string>

namespace instrument {

std::string getDictionaryFilename(llvm::Module& M);

class BuildDictionaryPass : public llvm::PassInfoMixin<BuildDictionaryPass> {
 private:
  struct LocalByte {
    llvm::Value* base;
    uint64_t offset;
  };

  Dictionary dictionary;

  void addToDictionary(
      const std::string& bytes, std::optional<uint64_t> positionHint = std::nullopt);
  void writeDictionary(llvm::Module& M);

  std::optional<std::string> serializeInteger(
      const llvm::ConstantInt& constant, const llvm::DataLayout& layout) const;
  std::optional<std::string> readFlatGlobal(
      const llvm::GlobalVariable& global, bool stripTrailingNull) const;
  std::optional<std::string> resolveGlobalSlice(
      llvm::Value* value, const llvm::DataLayout& layout) const;
  std::optional<LocalByte> inferLocalByte(
      llvm::Value* value, const llvm::DataLayout& layout) const;
  std::optional<uint64_t> inferLocalPosition(
      llvm::Value* value, const llvm::DataLayout& layout) const;

  void extractFromGlobalVariable(
      llvm::GlobalVariable& global, const llvm::DataLayout& layout);
  void extractFromConstant(llvm::Constant& constant, const llvm::DataLayout& layout);
  void extractFromICmp(llvm::ICmpInst& compare, const llvm::DataLayout& layout);
  void extractFromSwitch(llvm::SwitchInst& switchInst, const llvm::DataLayout& layout);
  void extractFromComparisonCall(llvm::CallBase& call, const llvm::DataLayout& layout);
  void extractFromFunction(llvm::Function& function, const llvm::DataLayout& layout);

 public:
  llvm::PreservedAnalyses run(llvm::Module& M, llvm::ModuleAnalysisManager& AM);
};

}  // namespace instrument
