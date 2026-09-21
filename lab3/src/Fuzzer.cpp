/**
 * NOTE: You should feel free to manipulate any content in this .cpp file.
 * This means that if you want you can change almost everything,
 * as long as the fuzzer runs with the same cli interface. Preserve the public
 * applyDictionaryEntry interface in DictionaryMutation.h/DictionaryMutation.cpp
 * and use it when implementing dictionary mutations.
 * Preserve PowerSchedule, PowerScheduler, SeedInputs, MutationFns, RunInfo's
 * fields, and the fuzzOneBatch entry point used by the deterministic scheduler
 * checker. The checker supplies corpus entries, mutation functions, and the
 * target interfaces declared in Utils.h.
 * This also means that if you're happy with some of the provided default
 * implementation, you don't have to modify it.
 */

#include "DictionaryMutation.h"
#include "PowerSchedule.h"
#include "Utils.h"
#include <sys/stat.h>
#include <sys/types.h>

#include <algorithm>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <iostream>
#include <iterator>
#include <map>
#include <set>
#include <stdio.h>
#include <string>
#include <time.h>
#include <unistd.h>
#include <utility>
#include <vector>

#define ARG_EXIST_CHECK(Name, Arg)                                                       \
  {                                                                                      \
    struct stat Buffer;                                                                  \
    if (stat(Arg, &Buffer)) {                                                            \
      fprintf(stderr, "%s not found\n", Arg);                                            \
      return 1;                                                                          \
    }                                                                                    \
  }                                                                                      \
  std::string Name(Arg);

#define DBG std::cout << "Hit F::" << __FILE__ << " ::L" << __LINE__ << std::endl

/**
 * @brief Type Signature of Mutation Function.
 * MutationFn takes a string as input and returns a string.
 *
 * MutationFn: string -> string
 */
typedef std::string MutationFn(std::string);

/**
 * Struct that holds useful information about
 * one run of the program.
 *
 * @param Passed       did the program run without crashing?
 * @param Mutation     mutation function used for this run.
 * @param Input        parent input used for generating input for this run.
 * @param MutatedInput input string for this run.
 */
struct RunInfo {
  bool Passed;
  MutationFn* Mutation;
  std::string Input, MutatedInput;
};

/************************************************/
/*            Global state variables            */
/************************************************/
/**
 * Preserve the scheduler interfaces listed above. Other globals can change
 * depending on what you want to keep track of during fuzzing.
 */
// Collection of strings used to generate inputs
std::vector<std::string> SeedInputs;

// Scheduling metadata uses the complete input bytes as a stable seed identity.
instrument::PowerSchedule PowerScheduler;

// Variable to store coverage related information.
std::vector<std::string> CoverageState;

// Coverage related information from previous step.
std::vector<std::string> PrevCoverageState;

/**
 * @brief Variable to keep track of some Mutation related state.
 * Feel free to change/ignore this if you want to.
 */
int MutationState = 0;

/**
 * @brief Variable to keep track of some state related to strategy selection.
 * Feel free to change/ignore this if you want to.
 */
int StrategyState = 0;

/************************************************/
/*    Implement your select input algorithm     */
/************************************************/

/**
 * @brief Select a string that will be mutated to generate a new input.
 * Sample code picks the first input string from SeedInputs.
 *
 * TODO: Implement your logic for selecting a input to mutate.
 * If you require, you can use the Info variable to help make a
 * decision while selecting a Seed but it is not necessary for the lab.
 *
 * @param RunInfo struct with information about the previous run.
 * @return Pointer to a string.
 */
std::string selectInput(RunInfo Info) {
  int Index = 0;

  return SeedInputs[Index];
}

/*********************************************/
/*       Implement mutation startegies       */
/*********************************************/

const char ALPHA[] = "abcdefghijklmnopqrstuvwxyz\n\0";
const int LENGTH_ALPHA = sizeof(ALPHA);

/**
 * Here we provide a two sample mutation functions
 * that take as input a string and returns a string.
 */

/**
 * @brief Mutation Strategy that does nothing.
 *
 * @param Original Original input string.
 * @return std::string mutated string.
 */
std::string mutationA(std::string Original) {
  return Original;
}

/**
 * @brief Mutation Strategy that inserts a random
 * alpha numeric char at a random location in Original.
 *
 * @param Original Original input string.
 * @return std::string mutated string.
 */
std::string mutationB(std::string Original) {
  if (Original.length() <= 0) {
    return Original;
  }

  int Index = rand() % Original.length();
  return Original.insert(Index, 1, ALPHA[rand() % LENGTH_ALPHA]);
}

/**
 * TODO: Add your own mutation functions below.
 * Make sure to update the MutationFns vector to include your functions.
 *
 * Some ideas: swap adjacent chars, increment/decrement a char.
 *
 * Get creative with your strategies.
 */

instrument::Dictionary FuzzingDict;

std::string mutationDictDeterministic(std::string Original) {
  // TODO: Insert/overwrite entries at multiple indices

  return Original;
}

std::string mutationDictRandom(std::string Original) {
  // TODO: Implement a mutation that randomly:
  // - inserts a dictionary entry into `Original`, or
  // - overwrites some of `Original` with a dictionary entry
  return Original;
}

/**
 * @brief Vector containing all the available mutation functions
 *
 * TODO: Update the definition to include any mutations you implement.
 * For example if you implement mutationC then update it to be:
 * std::vector<MutationFn *> MutationFns = {mutationA, mutationB, mutationC};
 */
std::vector<MutationFn*> MutationFns = {
    mutationA,
    mutationB,
};

/**
 * @brief Select a mutation function to apply to the seed input.
 * Sample code picks a random Strategy.
 *
 * TODO: Add your own logic to select a mutation function from MutationFns.
 * Hint: You may want to make use of any global state you store
 * during feedback to make decisions on what MutationFn to choose.
 *
 * @param RunInfo struct with information about the current run.
 * @returns a pointer to a MutationFn
 */
MutationFn* selectMutationFn(RunInfo& Info) {
  int Strat = rand() % MutationFns.size();

  return MutationFns[Strat];
}

/*********************************************/
/*     Implement your feedback algorithm     */
/*********************************************/
/**
 * Update the internal state of the fuzzer using coverage feedback.
 *
 * @param Target name of target binary
 * @param Info RunInfo
 */
void feedBack(std::string& Target, RunInfo& Info) {
  std::vector<std::string> RawCoverageData;
  readCoverageFile(Target, RawCoverageData);

  // Every execution contributes to shared frequency, even if its input is
  // discarded or crashes. Coverage normalization is provided by the lab.
  auto Signature = instrument::makeCoverageSignature(RawCoverageData);
  PowerScheduler.observeExecution(Signature);

  PrevCoverageState = CoverageState;
  CoverageState.clear();

  /**
   * TODO: Implement your logic to use the coverage information from the test
   * phase to guide fuzzing. The sky is the limit!
   *
   * Hint: You want to rely on some amount of randomness to make decisions.
   *
   * You have the Coverage information of the previous test in
   * PrevCoverageState. And the raw coverage data is loaded into RawCoverageData
   * from the Target.cov file. You can either use this raw data directly or
   * process it (not-necessary). If you do some processing, make sure to update
   * CoverageState to make it available in the next call to feedback.
   */
  CoverageState.assign(RawCoverageData.begin(),
      RawCoverageData.end());  // No extra processing

  // Admission and execution accounting are separate: registering a retained
  // input must not count this execution a second time or reset an existing seed.
  if (std::find(SeedInputs.begin(), SeedInputs.end(), Info.MutatedInput) !=
      SeedInputs.end()) {
    PowerScheduler.registerSeed(Info.MutatedInput, Signature);
  }
}

int Freq = 1000;
int Count = 0;
int PassCount = 0;

bool test(std::string& Target, std::string& Input, std::string& OutDir) {
  // Clean up old coverage file before running
  std::string CoveragePath = Target + ".cov";
  std::remove(CoveragePath.c_str());

  ++Count;
  int ReturnCode = runTarget(Target, Input);
  if (ReturnCode == 127) {
    fprintf(stderr, "%s not found\n", Target.c_str());
    exit(1);
  }
  fprintf(stderr, "\e[A\rTried %d inputs, %d crashes found\n", Count, failureCount);
  if (ReturnCode == 0) {
    if (PassCount++ % Freq == 0)
      storePassingInput(Input, OutDir);
    return true;
  } else {
    storeCrashingInput(Input, OutDir);
    return false;
  }
}

// Provided calibration records each distinct initial seed's coverage once.
// It does not select seeds for mutation or consume random numbers.
void calibrateSeedInputs(std::string& Target, std::string& OutDir) {
  std::set<std::string> Calibrated;
  for (std::string Input : SeedInputs) {
    if (!Calibrated.insert(Input).second) {
      continue;
    }
    test(Target, Input, OutDir);
    std::vector<std::string> RawCoverageData;
    readCoverageFile(Target, RawCoverageData);
    auto Signature = instrument::makeCoverageSignature(RawCoverageData);
    PowerScheduler.observeExecution(Signature);
    PowerScheduler.registerSeed(Input, Signature);
  }
}

/**
 * @brief Select one parent and spend its AFLFast mutation budget.
 *
 * Preserve this entry point for the deterministic scheduling checker.
 */
void fuzzOneBatch(std::string& Target, std::string& OutDir, RunInfo& Info) {
  const std::string Input = selectInput(Info);
  // TODO: Obtain this seed's energy budget from PowerScheduler.
  const uint32_t Energy = 1;
  for (uint32_t Iteration = 0; Iteration < Energy; ++Iteration) {
    Info = RunInfo();
    Info.Input = Input;
    Info.Mutation = selectMutationFn(Info);
    Info.MutatedInput = Info.Mutation(Info.Input);
    Info.Passed = test(Target, Info.MutatedInput, OutDir);
    feedBack(Target, Info);
  }
}

/**
 * @brief Fuzz the Target program and store the results to OutDir.
 */
void fuzz(std::string Target, std::string OutDir) {
  calibrateSeedInputs(Target, OutDir);
  RunInfo Info{};
  while (true) {
    fuzzOneBatch(Target, OutDir, Info);
  }
}

/**
 * Usage:
 * ./fuzzer [target] [seed input dir] [output dir]
 *          [--freq frequency] [--seed random seed] [--dict dictionary dir]
 */
int main(int argc, char** argv) {
  if (argc < 4) {
    printf(
        "usage %s [target] [seed input dir] [output dir] "
        "[--freq frequency] [--seed random-seed] [--dict dictionary-dir]\n",
        argv[0]);
    return 1;
  }

  ARG_EXIST_CHECK(Target, argv[1]);
  ARG_EXIST_CHECK(SeedInputDir, argv[2]);
  ARG_EXIST_CHECK(OutDir, argv[3]);

  int RandomSeed = (int)time(NULL);
  for (int i = 4; i < argc; i++) {
    std::string arg = argv[i];

    if (arg == "--freq") {
      Freq = strtol(argv[i + 1], NULL, 10);
      i++;
    } else if (arg == "--seed") {
      RandomSeed = strtol(argv[i + 1], NULL, 10);
      i++;
    } else if (arg == "--dict") {
      FuzzingDict = readDictionary(argv[i + 1]);
      i++;

      if (!FuzzingDict.empty()) {
        // TODO: Uncomment as you implement each mutation
        // MutationFns.push_back(mutationDictDeterministic);
        // MutationFns.push_back(mutationDictRandom);
      }
    }
  }

  fprintf(stderr, "Using seed: %d\n", RandomSeed);

  srand(RandomSeed);
  storeSeed(OutDir, RandomSeed);
  initialize(OutDir);

  if (readSeedInputs(SeedInputs, SeedInputDir)) {
    fprintf(stderr, "Cannot read seed input directory\n");
    return 1;
  }

  fprintf(stderr, "Fuzzing %s...\n\n", Target.c_str());
  fuzz(Target, OutDir);
  return 0;
}
