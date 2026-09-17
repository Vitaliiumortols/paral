#ifndef L123_H
#define L123_H

#include <list>

extern std::list<int> globalSharedList;

void appendElementsUnsafe(int initialVal);
void searchElementsUnsafe(int targetVal);
void runTask123(int startVal, int searchVal);

#endif 