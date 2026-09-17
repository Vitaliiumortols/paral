#include "L125.h"
#include <iostream>
#include <algorithm>
#include <thread>
#include <chrono>

std::list<int> safeGuardList;
std::mutex guardMutex;

void addSingleValue(int val) {
    std::lock_guard<std::mutex> lock(guardMutex);
    safeGuardList.push_back(val);
    std::cout << "[lock_guard] Вставка елемента: " << val << "\n";
}

void checkSingleValue(int targetVal) {
    std::lock_guard<std::mutex> lock(guardMutex);
    bool match = (std::find(safeGuardList.begin(), safeGuardList.end(), targetVal) != safeGuardList.end());
    std::cout << "[lock_guard] Пошук " << targetVal 
              << (match ? " -> знайдено" : " -> не знайдено") << "\n";
}

void runTask125(int startVal, int searchVal) {
    std::cout << "\n--- Запуск завдання 1.2.5 (10 окремих потоків через lock_guard) ---\n";
    safeGuardList.clear();

    for (int i = 0; i < 10; ++i) {
        std::thread pushWorker(addSingleValue, startVal + i);
        std::thread searchWorker(checkSingleValue, searchVal);

        pushWorker.detach();
        searchWorker.detach();
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(800));
}