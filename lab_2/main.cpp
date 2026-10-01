#include <chrono>
#include <clocale>
#include <cmath>
#include <condition_variable>
#include <cstddef>
#include <cstdint>
#include <deque>
#include <future>
#include <iomanip>
#include <iostream>
#include <limits>
#include <mutex>
#include <queue>
#include <stdexcept>
#include <string>
#include <thread>
#include <utility>
#include <vector>

// Лабораторна робота №2.

namespace prime_utils {

bool isPrime(const std::int64_t number) {
    if (number < 2) {
        return false;
    }
    if (number == 2) {
        return true;
    }
    if (number % 2 == 0) {
        return false;
    }

    for (std::int64_t divisor = 3; divisor <= number / divisor; divisor += 2) {
        if (number % divisor == 0) {
            return false;
        }
    }
    return true;
}

std::uint64_t nthPrime(const std::size_t index) {
    if (index == 0) {
        throw std::invalid_argument("Номер простого числа має бути додатним.");
    }

    constexpr std::uint64_t smallPrimes[] = {2, 3, 5, 7, 11};
    if (index <= 5) {
        return smallPrimes[index - 1];
    }

    const double n = static_cast<double>(index);
    const double estimate = std::ceil(n * (std::log(n) + std::log(std::log(n)))) + 1.0;
    if (estimate >= static_cast<double>(std::numeric_limits<std::size_t>::max())) {
        throw std::overflow_error("Заданий номер завеликий.");
    }

    const std::size_t limit = static_cast<std::size_t>(estimate);
    std::vector<bool> primeTable(limit + 1, true);
    primeTable[0] = false;
    primeTable[1] = false;

    for (std::size_t value = 2; value <= limit / value; ++value) {
        if (primeTable[value]) {
            for (std::size_t multiple = value * value; multiple <= limit; multiple += value) {
                primeTable[multiple] = false;
            }
        }
    }

    std::size_t count = 0;
    for (std::size_t value = 2; value <= limit; ++value) {
        if (primeTable[value] && ++count == index) {
            return value;
        }
    }

    throw std::runtime_error("Не вдалося обчислити просте число.");
}

}  // namespace prime_utils

namespace task_1_2_1 {

// Пункт 1.2.1 — прапорець, unique_lock і перевірка через кожні 100 мс.
std::queue<std::int64_t> dataQueue;
std::mutex dataMutex;
bool inputFinished = false;

void DataPreparation() {
    std::cout << "Вводьте цілі числа (0 завершує введення):\n";
    std::int64_t value = 0;
    while (std::cin >> value && value != 0) {
        std::unique_lock<std::mutex> lock(dataMutex);
        dataQueue.push(value);
    }

    std::cout << "Введення завершено.\n";
    std::unique_lock<std::mutex> lock(dataMutex);
    inputFinished = true;
}

void DataProcessing() {
    while (true) {
        std::unique_lock<std::mutex> lock(dataMutex);
        if (inputFinished) {
            break;
        }
        lock.unlock();
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    std::unique_lock<std::mutex> lock(dataMutex);
    std::cout << "Прості числа з черги: ";
    bool foundPrime = false;
    while (!dataQueue.empty()) {
        const std::int64_t value = dataQueue.front();
        dataQueue.pop();
        if (prime_utils::isPrime(value)) {
            std::cout << value << ' ';
            foundPrime = true;
        }
    }
    std::cout << (foundPrime ? "\n" : "немає\n");
}

void run() {
    std::thread preparationThread(DataPreparation);
    std::thread processingThread(DataProcessing);
    preparationThread.detach();
    processingThread.join();
}

}  // namespace task_1_2_1

namespace task_1_2_2 {

// Пункт 1.2.2 — три потоки очікують, доки Awake встановить i = 1.
std::mutex signalMutex;
std::condition_variable signalCondition;
int i = 0;

void Waits(const int id) {
    std::unique_lock<std::mutex> lock(signalMutex);
    std::cout << "Потік " << id << " входить у стан очікування.\n";
    signalCondition.wait(lock, [] { return i == 1; });
    std::cout << "Потік " << id << " завершив очікування.\n";
}

void Awake() {
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    {
        std::lock_guard<std::mutex> lock(signalMutex);
        std::cout << "Перше notify_all: i ще дорівнює 0.\n";
    }
    signalCondition.notify_all();

    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    {
        std::lock_guard<std::mutex> lock(signalMutex);
        i = 1;
        std::cout << "Друге notify_all: i встановлено в 1.\n";
    }
    signalCondition.notify_all();
}

void run() {
    std::thread waiter1(Waits, 1);
    std::thread waiter2(Waits, 2);
    std::thread waiter3(Waits, 3);
    std::thread awakeThread(Awake);

    awakeThread.join();
    waiter1.join();
    waiter2.join();
    waiter3.join();
}

}  // namespace task_1_2_2

namespace task_1_2_3 {

// Пункт 1.2.3 — notify_one пробуджує лише один із трьох потоків.
std::mutex signalMutex;
std::condition_variable signalCondition;
std::condition_variable handledCondition;
int i = 0;
bool signalHandled = false;
bool shutdown = false;

void Waits(const int id) {
    std::unique_lock<std::mutex> lock(signalMutex);
    signalCondition.wait(lock, [] { return i == 1 || shutdown; });

    if (i == 1) {
        i = 0;
        signalHandled = true;
        std::cout << "Повідомлення з потоку " << id << ".\n";
        lock.unlock();
        handledCondition.notify_one();
    }
}

void Notify() {
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    {
        std::lock_guard<std::mutex> lock(signalMutex);
        i = 1;
        std::cout << "Notify встановив i = 1 і викликає notify_one().\n";
    }
    signalCondition.notify_one();
}

void run() {
    std::thread thread1(Waits, 1);
    std::thread thread2(Waits, 2);
    std::thread thread3(Waits, 3);
    std::thread notifyThread(Notify);

    notifyThread.join();
    {
        std::unique_lock<std::mutex> lock(signalMutex);
        handledCondition.wait(lock, [] { return signalHandled; });
        // Службове завершення не імітує отримання основного сигналу.
        shutdown = true;
    }
    signalCondition.notify_all();

    thread1.join();
    thread2.join();
    thread3.join();
}

}  // namespace task_1_2_3

namespace task_1_2_4 {

// Пункт 1.2.4 — умовна змінна замість періодичного опитування прапорця.
std::queue<std::int64_t> dataQueue;
std::mutex dataMutex;
std::condition_variable dataCondition;
bool inputFinished = false;

void DataPreparation() {
    std::cout << "Вводьте цілі числа (0 завершує введення):\n";
    std::int64_t value = 0;
    while (std::cin >> value && value != 0) {
        {
            std::lock_guard<std::mutex> lock(dataMutex);
            dataQueue.push(value);
        }
        dataCondition.notify_one();
    }

    {
        std::lock_guard<std::mutex> lock(dataMutex);
        inputFinished = true;
    }
    dataCondition.notify_one();
}

void DataProcessing() {
    std::unique_lock<std::mutex> lock(dataMutex);
    while (true) {
        dataCondition.wait(lock, [] {
            return !dataQueue.empty() || inputFinished;
        });

        while (!dataQueue.empty()) {
            const std::int64_t value = dataQueue.front();
            dataQueue.pop();
            lock.unlock();
            if (prime_utils::isPrime(value)) {
                std::cout << value << " — просте число.\n";
            }
            lock.lock();
        }

        if (inputFinished) {
            break;
        }
    }
    std::cout << "Усі числа опрацьовано.\n";
}

void run() {
    std::thread preparationThread(DataPreparation);
    std::thread processingThread(DataProcessing);
    preparationThread.detach();
    processingThread.join();
}

}  // namespace task_1_2_4

namespace task_1_2_5 {

// Пункт 1.2.5 — порівняння launch::deferred та launch::async.
void calculateAdditionalFunction(const std::size_t n) {
    std::cout << "Оберіть функцію від n: 1 — sqrt, 2 — sin, 3 — ln: ";
    int choice = 0;
    std::cin >> choice;

    const double argument = static_cast<double>(n);
    double result = 0.0;
    std::string name;
    switch (choice) {
        case 1:
            result = std::sqrt(argument);
            name = "sqrt";
            break;
        case 2:
            result = std::sin(argument);
            name = "sin";
            break;
        case 3:
            result = std::log(argument);
            name = "ln";
            break;
        default:
            std::cout << "Невідомий пункт.\n";
            return;
    }
    std::cout << name << '(' << n << ") = " << std::setprecision(12) << result << '\n';
}

void experiment(const std::launch policy, const std::string& name, const std::size_t n) {
    std::cout << "\n=== " << name << " ===\n";
    const auto started = std::chrono::steady_clock::now();
    std::future<std::uint64_t> future =
        std::async(policy, prime_utils::nthPrime, n);

    // У режимі async обчислення може йти паралельно з цим вибором користувача.
    calculateAdditionalFunction(n);

    const auto beforeGet = std::chrono::steady_clock::now();
    const std::uint64_t result = future.get();
    const auto finished = std::chrono::steady_clock::now();

    const auto totalMs =
        std::chrono::duration_cast<std::chrono::milliseconds>(finished - started).count();
    const auto waitMs =
        std::chrono::duration_cast<std::chrono::milliseconds>(finished - beforeGet).count();

    std::cout << n << "-те просте число = " << result << '\n'
              << "Загальний час: " << totalMs << " мс\n"
              << "Очікування у get(): " << waitMs << " мс\n";
}

void run() {
    std::size_t n = 0;
    std::cout << "Введіть номер простого числа n: ";
    if (!(std::cin >> n) || n == 0) {
        std::cout << "n має бути додатним.\n";
        return;
    }

    experiment(std::launch::deferred, "launch::deferred", n);
    experiment(std::launch::async, "launch::async", n);
    std::cout << "Deferred починає роботу у get(), а async — в окремому потоці.\n";
}

}  // namespace task_1_2_5

namespace task_1_2_6 {

// Пункт 1.2.6 — два деки: завдання packaged_task та відповідні номери n.
std::deque<std::packaged_task<std::uint64_t(std::size_t)>> taskQueue;
std::deque<std::size_t> numberQueue;
std::mutex queueMutex;
std::condition_variable queueCondition;
bool stopping = false;

struct PendingResult {
    std::size_t n;
    std::future<std::uint64_t> future;
};

void Worker() {
    while (true) {
        std::packaged_task<std::uint64_t(std::size_t)> task;
        std::size_t n = 0;
        {
            std::unique_lock<std::mutex> lock(queueMutex);
            queueCondition.wait(lock, [] { return !taskQueue.empty() || stopping; });
            if (taskQueue.empty() && stopping) {
                break;
            }

            task = std::move(taskQueue.front());
            n = numberQueue.front();
            taskQueue.pop_front();
            numberQueue.pop_front();
        }
        task(n);
    }
}

void run() {
    std::thread workerThread(Worker);
    std::vector<PendingResult> results;

    std::cout << "Вводьте номери простих чисел; q завершує введення.\n";
    while (true) {
        std::cout << "n або q: ";
        std::string token;
        if (!(std::cin >> token) || token == "q" || token == "Q") {
            break;
        }

        std::size_t n = 0;
        std::size_t parsedCharacters = 0;
        try {
            if (token.empty() || token.front() == '-') {
                throw std::invalid_argument("invalid number");
            }
            const unsigned long long parsed = std::stoull(token, &parsedCharacters);
            if (parsed == 0 || parsedCharacters != token.size() ||
                parsed > std::numeric_limits<std::size_t>::max()) {
                throw std::invalid_argument("invalid number");
            }
            n = static_cast<std::size_t>(parsed);
        } catch (const std::exception&) {
            std::cout << "Потрібне додатне число або q.\n";
            continue;
        }

        std::packaged_task<std::uint64_t(std::size_t)> task(prime_utils::nthPrime);
        std::future<std::uint64_t> future = task.get_future();
        {
            std::lock_guard<std::mutex> lock(queueMutex);
            taskQueue.push_back(std::move(task));
            numberQueue.push_back(n);
        }
        results.push_back(PendingResult{n, std::move(future)});
        queueCondition.notify_one();
    }

    {
        std::lock_guard<std::mutex> lock(queueMutex);
        stopping = true;
    }
    queueCondition.notify_one();

    std::cout << "Результати:\n";
    for (PendingResult& result : results) {
        std::cout << result.n << "-те просте число = " << result.future.get() << '\n';
    }
    workerThread.join();
}

}  // namespace task_1_2_6

namespace task_1_2_7 {

// Пункт 1.2.7 — promise/future, два void-потоки та жодних глобальних даних.
void FirstThread(
    const std::size_t n,
    std::promise<std::uint64_t> nthPromise,
    std::promise<bool> startPromise,
    std::promise<std::uint64_t> tenNthPromise) {
    nthPromise.set_value(prime_utils::nthPrime(n));
    startPromise.set_value(true);
    tenNthPromise.set_value(prime_utils::nthPrime(n * 10));
}

void SecondThread(
    const std::size_t n,
    std::future<bool> startSignal,
    std::mutex& outputMutex) {
    if (startSignal.get()) {
        std::this_thread::sleep_for(std::chrono::seconds(2));
        std::lock_guard<std::mutex> lock(outputMutex);
        std::cout << "[Другий потік] sqrt(" << n << ") = "
                  << std::setprecision(12) << std::sqrt(static_cast<double>(n)) << '\n';
    }
}

void run() {
    std::size_t n = 0;
    std::cout << "Введіть номер простого числа n: ";
    if (!(std::cin >> n) || n == 0 || n > std::numeric_limits<std::size_t>::max() / 10) {
        std::cout << "Некоректне значення n.\n";
        return;
    }

    std::promise<std::uint64_t> nthPromise;
    std::promise<bool> startPromise;
    std::promise<std::uint64_t> tenNthPromise;
    std::future<std::uint64_t> nthFuture = nthPromise.get_future();
    std::future<bool> startFuture = startPromise.get_future();
    std::future<std::uint64_t> tenNthFuture = tenNthPromise.get_future();
    std::mutex outputMutex;

    std::thread firstThread(
        FirstThread,
        n,
        std::move(nthPromise),
        std::move(startPromise),
        std::move(tenNthPromise));
    std::thread secondThread(SecondThread, n, std::move(startFuture), std::ref(outputMutex));

    const std::uint64_t nthResult = nthFuture.get();
    {
        std::lock_guard<std::mutex> lock(outputMutex);
        std::cout << "[Головний потік] " << n << "-те просте число = "
                  << nthResult << '\n';
    }

    const std::uint64_t tenNthResult = tenNthFuture.get();
    {
        std::lock_guard<std::mutex> lock(outputMutex);
        std::cout << "[Головний потік] " << n * 10 << "-те просте число = "
                  << tenNthResult << '\n';
    }

    firstThread.join();
    secondThread.join();
}

}  // namespace task_1_2_7

int main() {
    std::setlocale(LC_ALL, "");

    std::cout << "Лабораторна робота №2\n"
              << "Оберіть пункт для запуску:\n"
              << "1 — пункт 1.2.1\n"
              << "2 — пункт 1.2.2\n"
              << "3 — пункт 1.2.3\n"
              << "4 — пункт 1.2.4\n"
              << "5 — пункт 1.2.5\n"
              << "6 — пункт 1.2.6\n"
              << "7 — пункт 1.2.7\n"
              << "Ваш вибір: ";

    int choice = 0;
    if (!(std::cin >> choice)) {
        std::cerr << "Помилка введення.\n";
        return 1;
    }

    switch (choice) {
        case 1:
            task_1_2_1::run();
            break;
        case 2:
            task_1_2_2::run();
            break;
        case 3:
            task_1_2_3::run();
            break;
        case 4:
            task_1_2_4::run();
            break;
        case 5:
            task_1_2_5::run();
            break;
        case 6:
            task_1_2_6::run();
            break;
        case 7:
            task_1_2_7::run();
            break;
        default:
            std::cerr << "Такого пункту немає.\n";
            return 1;
    }

    return 0;
}
