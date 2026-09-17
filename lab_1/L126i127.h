#ifndef L126I127_H
#define L126I127_H

#include <string>
#include <mutex>

class someData {
public:
    std::string name;
    std::string surname;
    std::string address;
    int age;

    someData(std::string n = "Default", std::string s = "User", std::string a = "None", int ag = 0)
        : name(std::move(n)), surname(std::move(s)), address(std::move(a)), age(ag) {}

    void display() const;
};

class exchangePerson {
public:
    someData data;
    std::mutex mtx;

    exchangePerson() = default;
    exchangePerson(const someData& d) : data(d) {}

    static void JohnDoe(exchangePerson& target);
    static void JacobSmith(exchangePerson& target);

    static void SwapAdoptLock(exchangePerson& lhs, exchangePerson& rhs);

    static void SwapUniqueLock(exchangePerson& lhs, exchangePerson& rhs);
};

void runTask126();
void runTask127();

#endif 