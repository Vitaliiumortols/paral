#include <iostream>
#include <clocale>
#include "L121i122.h"
#include "L123.h"
#include "L124.h"
#include "L125.h"
#include "L126i127.h"

int main() {
    std::setlocale(LC_ALL, "");

    std::cout << "=== ЛАБОРАТОРНА РОБОТА №1: М'ЮТЕКСИ ТА СИНХРОНІЗАЦІЯ ПОТОКІВ ===\n";

    runTask122();

    int initialElement = 0;
    int elementToFind = 0;

    std::cout << "\nВведіть базове число для генерації списку: ";
    std::cin >> initialElement;
    std::cout << "Введіть число для перевірки наявності: ";
    std::cin >> elementToFind;

    runTask123(initialElement, elementToFind);

    runTask124(initialElement, elementToFind);

    runTask125(initialElement, elementToFind);

    runTask126(); 
    runTask127(); 

    std::cout << "\nУсі пункти лабораторної успішно відпрацювали.\n";
    return 0;
}