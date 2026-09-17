#include "L121i122.h"
#include <iostream>
#include <thread>
#include <chrono>

void printNumberOne() {
    std::cout << "[Потік 1] Значення: 1\n";
}

void printNumberTwo() {
    std::cout << "[Потік 2] Значення: 2\n";
}

void runTask121() {
    std::cout << "\n--- Запуск завдання 1.2.1 (без join / detach) ---\n";
    std::thread workerA(printNumberOne);
    std::thread workerB(printNumberTwo);
   
}

void runTask122() {
    std::cout << "\n--- Запуск завдання 1.2.2 (із застосуванням detach) ---\n";
    std::thread workerA(printNumberOne);
    std::thread workerB(printNumberTwo);

    workerA.detach();
    workerB.detach();

    std::this_thread::sleep_for(std::chrono::milliseconds(50));
}