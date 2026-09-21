#include "BuildDictionary.h"

#include "llvm/ADT/APInt.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/IR/Argument.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/DataLayout.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/GlobalVariable.h"
#include "llvm/IR/InstrTypes.h"
#include "llvm/IR/Instruction.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Operator.h"
#include "llvm/IR/Type.h"
#include "llvm/IR/User.h"
#include "llvm/IR/Value.h"
#include "llvm/Support/Casting.h"
#include "llvm/Support/raw_ostream.h"

#include <algorithm>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <limits>
#include <optional>
#include <string>
#include <utility>
#include <vector>

using namespace llvm;

namespace {

constexpr size_t kMaxEntrySize = 1024;

}  // namespace

namespace instrument {

void BuildDictionaryPass::addToDictionary(
    const std::string& bytes, std::optional<uint64_t> positionHint) {
  if (!bytes.empty() && bytes.size() <= kMaxEntrySize) {
    dictionary.insert({bytes, positionHint});
  }
}

std::optional<std::string> BuildDictionaryPass::serializeInteger(
    const ConstantInt& constant, const DataLayout& layout) const {
  // TODO: Implement.
  (void)constant;
  (void)layout;
  return std::nullopt;
}

std::optional<std::string> BuildDictionaryPass::readFlatGlobal(
    const GlobalVariable& global, bool stripTrailingNull) const {
  // TODO: Implement.
  (void)global;
  (void)stripTrailingNull;
  return std::nullopt;
}

std::optional<std::string> BuildDictionaryPass::resolveGlobalSlice(
    Value* value, const DataLayout& layout) const {
  // TODO: Implement.
  (void)value;
  (void)layout;
  return std::nullopt;
}

std::optional<BuildDictionaryPass::LocalByte> BuildDictionaryPass::inferLocalByte(
    Value* value, const DataLayout& layout) const {
  // TODO: Implement.
  (void)value;
  (void)layout;
  return std::nullopt;
}

std::optional<uint64_t> BuildDictionaryPass::inferLocalPosition(
    Value* value, const DataLayout& layout) const {
  // TODO: Implement.
  (void)value;
  (void)layout;
  return std::nullopt;
}

void BuildDictionaryPass::extractFromGlobalVariable(
    GlobalVariable& global, const DataLayout& layout) {
  // TODO: Implement.
  (void)global;
  (void)layout;
}

void BuildDictionaryPass::extractFromConstant(
    Constant& constant, const DataLayout& layout) {
  // TODO: Extract this constant, if it's a supported integer.
  (void)constant;
  (void)layout;
}

void BuildDictionaryPass::extractFromICmp(ICmpInst& compare, const DataLayout& layout) {
  // TODO: Extract useful constants from this comparison, with a position
  // hint where possible.
  (void)compare;
  (void)layout;
}

void BuildDictionaryPass::extractFromSwitch(
    SwitchInst& switchInst, const DataLayout& layout) {
  // TODO: Implement.
  (void)switchInst;
  (void)layout;
}

void BuildDictionaryPass::extractFromComparisonCall(
    CallBase& call, const DataLayout& layout) {
  // TODO: Extract tokens from direct string/memory comparison calls.
  (void)call;
  (void)layout;
}

void BuildDictionaryPass::extractFromFunction(
    Function& function, const DataLayout& layout) {
  for (BasicBlock& block : function) {
    for (Instruction& instruction : block) {
      if (auto* compare = dyn_cast<ICmpInst>(&instruction)) {
        extractFromICmp(*compare, layout);
      } else if (auto* switchInst = dyn_cast<SwitchInst>(&instruction)) {
        extractFromSwitch(*switchInst, layout);
      } else if (auto* call = dyn_cast<CallBase>(&instruction)) {
        extractFromComparisonCall(*call, layout);
      }
    }
  }
}

void BuildDictionaryPass::writeDictionary(Module& M) {
  (void)M;
  const std::filesystem::path outputDirectory = "fuzzing_dict";
  std::error_code error;
  std::filesystem::remove_all(outputDirectory, error);
  if (error) {
    errs() << "Error: Could not clear dictionary directory\n";
    return;
  }
  std::filesystem::create_directories(outputDirectory, error);
  if (error) {
    errs() << "Error: Could not create dictionary directory\n";
    return;
  }

  size_t entryNumber = 0;
  for (const DictionaryEntry& entry : dictionary) {
    std::string filename = "entry_" + std::to_string(entryNumber++);
    if (entry.positionHint.has_value()) {
      filename += "@" + std::to_string(*entry.positionHint);
    }
    std::filesystem::path path = outputDirectory / filename;
    std::ofstream output(path, std::ios::binary | std::ios::trunc);
    if (!output.is_open()) {
      errs() << "Error: Could not write a dictionary entry\n";
      continue;
    }
    output.write(entry.bytes.data(), static_cast<std::streamsize>(entry.bytes.size()));
  }

  outs() << "Fuzzing dictionary written with " << dictionary.size() << " entries\n";
}

PreservedAnalyses BuildDictionaryPass::run(Module& M, ModuleAnalysisManager& AM) {
  (void)AM;
  dictionary.clear();
  const DataLayout& layout = M.getDataLayout();

  for (GlobalVariable& global : M.globals()) {
    extractFromGlobalVariable(global, layout);
  }
  for (Function& function : M) {
    if (!function.isDeclaration()) {
      extractFromFunction(function, layout);
    }
  }
  writeDictionary(M);
  return PreservedAnalyses::all();
}

}  // namespace instrument
