#ifndef L125_H
#define L125_H

#include <list>
#include <mutex>

extern std::list<int> safeGuardList;
extern std::mutex guardMutex;

void addSingleValue(int val);
void checkSingleValue(int targetVal);
void runTask125(int startVal, int searchVal);

#endif 