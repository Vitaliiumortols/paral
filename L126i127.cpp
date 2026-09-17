#include "L126i127.h"
#include <iostream>
#include <thread>
#include <chrono>
#include <utility>

void someData::display() const {
    std::cout << "[Ім'я: " << name 
              << " | Прізвище: " << surname 
              << " | Адреса: " << address 
              << " | Вік: " << age << "]\n";
}

void exchangePerson::JohnDoe(exchangePerson& target) {
    std::lock_guard<std::mutex> lock(target.mtx);
    target.data.name = "John";
    target.data.surname = "Doe";
    target.data.address = "Unknown";
    target.data.age = 120;
}

void exchangePerson::JacobSmith(exchangePerson& target) {
    std::lock_guard<std::mutex> lock(target.mtx);
    target.data.name = "Jacob";
    target.data.surname = "Smith";
    target.data.address = "Known";
    target.data.age = 1;
}

void exchangePerson::SwapAdoptLock(exchangePerson& lhs, exchangePerson& rhs) {
    if (&lhs == &rhs) {
        return;
    }

    std::lock(lhs.mtx, rhs.mtx);
    std::lock_guard<std::mutex> lockA(lhs.mtx, std::adopt_lock);
    std::lock_guard<std::mutex> lockB(rhs.mtx, std::adopt_lock);

    std::swap(lhs.data, rhs.data);
}

void exchangePerson::SwapUniqueLock(exchangePerson& lhs, exchangePerson& rhs) {
    if (&lhs == &rhs) {
        return;
    }

    std::unique_lock<std::mutex> lockA(lhs.mtx, std::defer_lock);
    std::unique_lock<std::mutex> lockB(rhs.mtx, std::defer_lock);

    std::lock(lockA, lockB);

    std::swap(lhs.data, rhs.data);
}

void runTask126() {
    std::cout << "\n--- Запуск завдання 1.2.6 (std::lock + std::adopt_lock) ---\n";
    exchangePerson user1, user2;

    std::cout << "Початковий стан:\nUser1: "; user1.data.display();
    std::cout << "User2: "; user2.data.display();

    std::thread tA(exchangePerson::JohnDoe, std::ref(user1));
    std::thread tB(exchangePerson::JacobSmith, std::ref(user2));
    tA.detach();
    tB.detach();

    std::this_thread::sleep_for(std::chrono::milliseconds(200));

    std::cout << "\nПісля встановлення значень (JohnDoe / JacobSmith):\nUser1: "; user1.data.display();
    std::cout << "User2: "; user2.data.display();

    std::cout << "\nВиконання SwapAdoptLock у виділеному потоці...\n";
    std::thread tSwap(exchangePerson::SwapAdoptLock, std::ref(user1), std::ref(user2));
    tSwap.join();

    std::cout << "Після обміну:\nUser1: "; user1.data.display();
    std::cout << "User2: "; user2.data.display();
}

void runTask127() {
    std::cout << "\n--- Запуск завдання 1.2.7 (std::unique_lock + defer_lock) ---\n";
    exchangePerson user1, user2;

    std::thread tA(exchangePerson::JohnDoe, std::ref(user1));
    std::thread tB(exchangePerson::JacobSmith, std::ref(user2));
    tA.detach();
    tB.detach();

    std::this_thread::sleep_for(std::chrono::milliseconds(200));

    std::cout << "Дані перед викликом SwapUniqueLock:\nUser1: "; user1.data.display();
    std::cout << "User2: "; user2.data.display();

    std::thread tSwap(exchangePerson::SwapUniqueLock, std::ref(user1), std::ref(user2));
    tSwap.join();

    std::cout << "Дані після виконання SwapUniqueLock:\nUser1: "; user1.data.display();
    std::cout << "User2: "; user2.data.display();
}