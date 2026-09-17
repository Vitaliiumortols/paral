#ifndef L124_H
#define L124_H

#include <list>
#include <mutex>

extern std::list<int> syncList;
extern std::mutex syncListMutex;

void appendElementsSync(int initialVal);
void searchElementsSync(int targetVal);
void runTask124(int startVal, int searchVal);

#endif 