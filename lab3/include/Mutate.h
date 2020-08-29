#include <string>
#include <map>

enum Campaign { MutationA, MutationB, MutationC, NumCampaigns };// add your mumation tags here, NumCampaigns shows how many muatations you have

std::string mutate(std::string Origin, Campaign &FuzzCamp); 

