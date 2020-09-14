/*
* NOTE: You should feel free to manipulate any content in this .cpp file
*/

#include <cstdlib>
#include <iostream>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <time.h>

#include <cstring>
#include <cstdio>

#include "Utils.h"

std::vector<std::string> SeedInputs;

/************************************************/
/* 		Implement your select input algorithm 	*/
/************************************************/

std::string selectInput() {
  /*
  * select the mutant string here. You can select this from the SeedInputs vector, where you can store all mutants 
  */
  return SeedInputs.back();
}

/*********************************************/
/* 		Implement your mutation algorithms	 */
/*********************************************/
 
const char *alpha = "abcdefghijklmnopqrstuvwxyz\n\0";

/**
 * you are required to make changes to the argument Origin and return the new string
 * the current implementation just returns the current string itself
 * get creative with your mutation strategy
 */
std::string mutate(std::string Origin) {
  return Origin;
}

/*********************************************/
/* 		Implement your feedback algorithm	 */
/*********************************************/

void feedBack(std::string &Target, std::string &Mutated) {
  /*
  * Target here is a string of the executable file name, you can find the information from its corresponding coverage file
  * to determine if the mutated string is interesting
  * Keep track of the mutant string and its corresponding coverage
  */
}

/*****************************************************************/
//helper functions 

int readSeedInputs(std::string &SeedInputDir) {
  DIR *Directory;
  struct dirent *Ent;
  if ((Directory = opendir(SeedInputDir.c_str())) != NULL) {
    while ((Ent = readdir(Directory)) != NULL) {
      if (!(Ent->d_type == DT_REG))
        continue;
      std::string Path = SeedInputDir + "/" + std::string(Ent->d_name);
      std::string Line = readOneFile(Path);
      SeedInputs.push_back(Line);
    }
    closedir(Directory);
    return 0;
  } else {
    return 1;
  }
}

int Freq = 1000;
int Count = 0;

bool test(std::string &Target, std::string &Input, std::string &OutDir) {
  // Clean up old coverage file before running 
  std::string CoveragePath = Target + ".cov";
  std::remove(CoveragePath.c_str());

  Count++;
  int ReturnCode = runTarget(Target, Input);
  switch (ReturnCode) {
  case 0:
    if (Count % Freq == 0)
      storePassingInput(Input, OutDir);
    return true;
  case 256:
    fprintf(stderr, "%d crashes found\n", failureCount);
    storeCrashingInput(Input, OutDir);
    return false;
  case 127:
    fprintf(stderr, "%s not found\n", Target.c_str());
    exit(1);
  }
}

void storeSeed(std::string &OutDir, int randomSeed) {
  std::string Path = OutDir + "/randomSeed.txt";
  std::fstream File(Path, std::fstream::out | std::ios_base::trunc);
  File << std::to_string(randomSeed);
  File.close();
}

int main(int argc, char **argv) { 
  if (argc < 4) { 
    printf("usage %s [exe file] [seed input dir] [output dir] [seed (optional arg)]\n", argv[0]);
    return 1;
  }

  struct stat Buffer;
  if (stat(argv[1], &Buffer)) {
    fprintf(stderr, "%s not found\n", argv[1]);
    return 1;
  }

  if (stat(argv[2], &Buffer)) {
    fprintf(stderr, "%s not found\n", argv[2]);
    return 1;
  }

  if (stat(argv[3], &Buffer)) {
    fprintf(stderr, "%s not found\n", argv[3]);
    return 1;
  }

  int randomSeed = (int)time(NULL);
  if (argc > 4) {
    randomSeed = strtol(argv[4], NULL, 10);
  }
  srand(randomSeed);

  std::string Target(argv[1]);
  std::string SeedInputDir(argv[2]);
  std::string OutDir(argv[3]);
  
  storeSeed(OutDir, randomSeed);
  
  initialize(OutDir);

  if (readSeedInputs(SeedInputDir)) {
    fprintf(stderr, "Cannot read seed input directory\n");
    return 1;
  }

  while (true) {
      std::string SC = selectInput();
      auto Mutant = mutate(SC);
      test(Target, Mutant, OutDir);
      feedBack(Target, Mutant);
  }
  return 0;
}
