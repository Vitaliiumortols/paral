#include "L124.h"
#include <iostream>
#include <algorithm>
#include <chrono>
#include <thread>

std::list<int> syncList;
std::mutex syncListMutex;

void appendElementsSync(int initialVal) {
    for (int idx = 0; idx < 10; ++idx) {
        int item = initialVal + idx;

        syncListMutex.lock();
        syncList.push_back(item);
        std::cout << "[Mutex Push]: Успішно додано " << item << std::endl;
        syncListMutex.unlock();

        std::this_thread::sleep_for(std::chrono::milliseconds(200));
    }
}

void searchElementsSync(int targetVal) {
    for (int attempt = 1; attempt <= 10; ++attempt) {
        syncListMutex.lock();
        bool found = (std::find(syncList.begin(), syncList.end(), targetVal) != syncList.end());
        syncListMutex.unlock();

        std::cout << "[Mutex Check " << attempt << "]: Значення " << targetVal 
                  << (found ? " знайдено в колекції" : " не знайдено") << std::endl;

        std::this_thread::sleep_for(std::chrono::milliseconds(300));
    }
}

void runTask124(int startVal, int searchVal) {
    std::cout << "\n--- Запуск завдання 1.2.4 (Синхронізація через std::mutex lock/unlock) ---\n";
    syncList.clear();

    std::thread thAdd(appendElementsSync, startVal);
    std::thread thFind(searchElementsSync, searchVal);

    thAdd.join();
    thFind.join();
}