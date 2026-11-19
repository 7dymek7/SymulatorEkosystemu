#include <iostream>
#include <cstdlib>
#include <ctime>
#include "Srodowisko.h"

int main() {
    std::srand(static_cast<unsigned int>(std::time(nullptr)));

    Srodowisko swiat(12, 8);
    swiat.dodajOrganizm(Organizm('A', 10, Pozycja(2, 2)));
    swiat.dodajOrganizm(Organizm('B', 8, Pozycja(8, 5)));

    int wybor = -1;
    while (wybor != 0) {
        swiat.rysujPlansze();
        std::cout << "\n--- MENU ---\n"
                  << "1. Wykonaj ture (ruch i spadek energii)\n"
                  << "2. Dodaj organizm\n"
                  << "3. Wyszukaj organizm na pozycji (x, y)\n"
                  << "4. Modyfikuj energie organizmu\n"
                  << "5. Usun organizm na pozycji (x, y)\n"
                  << "0. Zakoncz\n"
                  << "Wybor: ";
        std::cin >> wybor;

        if (wybor == 1) {
            swiat.wykonajTure();
        } else if (wybor == 2) {
            char sym;
            int en, x, y;
            std::cout << "Podaj symbol, energie, x, y: ";
            std::cin >> sym >> en >> x >> y;
            swiat.dodajOrganizm(Organizm(sym, en, Pozycja(x, y)));
        } else if (wybor == 3) {
            int x, y;
            std::cout << "Podaj x, y: ";
            std::cin >> x >> y;
            int idx = swiat.znajdzOrganizm(x, y);
            if (idx != -1) {
                swiat.wypiszSzczegoly(idx);
            } else {
                std::cout << "Brak organizmu na tej pozycji.\n";
            }
        } else if (wybor == 4) {
            int x, y, en;
            std::cout << "Podaj x, y oraz nowa energie: ";
            std::cin >> x >> y >> en;
            if (swiat.modyfikujEnergie(x, y, en)) {
                std::cout << "Zaktualizowano energie.\n";
            } else {
                std::cout << "Nie znaleziono organizmu na tej pozycji.\n";
            }
        } else if (wybor == 5) {
            int x, y;
            std::cout << "Podaj x, y organizmu do usuniecia: ";
            std::cin >> x >> y;
            int idx = swiat.znajdzOrganizm(x, y);
            if (idx != -1) {
                swiat.usunOrganizm(idx);
                std::cout << "Usunieto organizm.\n";
            } else {
                std::cout << "Brak organizmu na tej pozycji.\n";
            }
        }
    }

    return 0;
}