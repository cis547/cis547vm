#include <dirent.h>
#include <fstream>
#include <iostream>
#include <map>
#include <regex>
#include <set>
#include <streambuf>
#include <string>
#include <sys/stat.h>

extern int successCount;
extern int failureCount;

std::string readOneFile(std::string &Path);
int runTarget(std::string &Target, std::string &Input);
void initialize(std::string &OutDir);
void storePassingInput(std::string &Input, std::string &OutDir);
void storeCrashingInput(std::string &Input, std::string &OutDir);

