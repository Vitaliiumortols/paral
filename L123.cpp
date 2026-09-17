#include "L123.h"
#include <iostream>
#include <algorithm>
#include <chrono>
#include <thread>

std::list<int> globalSharedList;

void appendElementsUnsafe(int initialVal) {
    for (int step = 0; step < 10; ++step) {
        int current = initialVal + step;
        globalSharedList.push_back(current);
        std::cout << "-> [Unsafe Push]: " << current << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(300));
    }
}

void searchElementsUnsafe(int targetVal) {
    for (int attempt = 1; attempt <= 10; ++attempt) {
        auto it = std::find(globalSharedList.begin(), globalSharedList.end(), targetVal);
        bool exists = (it != globalSharedList.end());

        std::cout << "<?> [Unsafe Check #" << attempt << "] Шукане " << targetVal 
                  << (exists ? " ПРИСУТНЄ" : " ВІДСУТНЄ") << std::endl;

        std::this_thread::sleep_for(std::chrono::milliseconds(450));
    }
}

void runTask123(int startVal, int searchVal) {
    std::cout << "\n--- Запуск завдання 1.2.3 (Без синхронізації) ---\n";
    globalSharedList.clear();

    std::thread thProducer(appendElementsUnsafe, startVal);
    std::thread thConsumer(searchElementsUnsafe, searchVal);

    thProducer.join();
    thConsumer.join();
}