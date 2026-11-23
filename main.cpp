#ifdef _WIN32
#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#ifndef ENABLE_VIRTUAL_TERMINAL_PROCESSING
#define ENABLE_VIRTUAL_TERMINAL_PROCESSING 0x0004
#endif
#endif

#include <iostream>
#include <memory>
#include <string>
#include <cstdlib>
#include <ctime>
#include <thread>
#include <chrono>
#include <limits>
#include <stdexcept>
#include "Srodowisko.h"
#include "PorostMarsjanski.h"
#include "DrapieznikMarsjanski.h"

using namespace std;

void inicjalizujKonsole() {
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    if (hOut != INVALID_HANDLE_VALUE) {
        DWORD dwMode = 0;
        if (GetConsoleMode(hOut, &dwMode)) {
            SetConsoleMode(hOut, dwMode | ENABLE_VIRTUAL_TERMINAL_PROCESSING);
        }
    }
#endif
}

int wczytajLiczbe() {
    int wartosc;
    if (!(cin >> wartosc)) {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        throw invalid_argument("Wprowadzono niepoprawny format danych (oczekiwano liczby calkowitej)!");
    }
    return wartosc;
}

int main() {
    inicjalizujKonsole();
    srand(static_cast<unsigned int>(time(nullptr)));

    Srodowisko mars(12, 8);
    mars.dodajOrganizm(make_unique<PorostMarsjanski>(10, Pozycja(2, 2), 2));
    mars.dodajOrganizm(make_unique<PorostMarsjanski>(9, Pozycja(9, 5), 2));
    mars.dodajOrganizm(make_unique<PorostMarsjanski>(8, Pozycja(5, 6), 3));
    mars.dodajOrganizm(make_unique<DrapieznikMarsjanski>(18, Pozycja(3, 3), 1));
    mars.dodajOrganizm(make_unique<DrapieznikMarsjanski>(20, Pozycja(8, 2), 2));

    string komunikat = "Misja terraformacji Marsa rozpoczeta.";
    int wybor = -1;

    while (wybor != 0) {
        mars.rysujPlansze(komunikat);
        komunikat.clear();

        cout << "\n\033[1;38;5;208m--- PANEL KONTROLNY ---\033[0m\n"
             << "1. Nastepny Sol (1 tura)\n"
             << "2. Animacja automatyczna (5 Soli)\n"
             << "3. Dodaj Porost Marsjanski (*)\n"
             << "4. Dodaj Drapieznika Marsjanskiego (X)\n"
             << "5. Skanuj pole (x, y)\n"
             << "6. Modyfikuj energie organizmu\n"
             << "7. Usun organizm z pola (x, y)\n"
             << "0. Zakoncz misje\n"
             << "Wybor: ";

        try {
            wybor = wczytajLiczbe();

            if (wybor == 1) {
                mars.wykonajTure();
            } else if (wybor == 2) {
                for (int i = 0; i < 5; ++i) {
                    mars.wykonajTure();
                    mars.rysujPlansze("Trwa symulacja automatyczna (" + to_string(i + 1) + "/5)...");
                    this_thread::sleep_for(chrono::milliseconds(450));
                }
                komunikat = "Zakonczono cykl 5 Soli.";
            } else if (wybor == 3) {
                cout << "Podaj energie, x, y: ";
                int en = wczytajLiczbe();
                int x = wczytajLiczbe();
                int y = wczytajLiczbe();
                mars.dodajOrganizm(make_unique<PorostMarsjanski>(en, Pozycja(x, y)));
                komunikat = "Dodano Porost Marsjanski.";
            } else if (wybor == 4) {
                cout << "Podaj energie, x, y, zasieg ruchu (1-3): ";
                int en = wczytajLiczbe();
                int x = wczytajLiczbe();
                int y = wczytajLiczbe();
                int zasieg = wczytajLiczbe();
                mars.dodajOrganizm(make_unique<DrapieznikMarsjanski>(en, Pozycja(x, y), zasieg));
                komunikat = "Dodano Drapieznika Marsjanskiego.";
            } else if (wybor == 5) {
                cout << "Podaj wspolrzedne x, y: ";
                int x = wczytajLiczbe();
                int y = wczytajLiczbe();
                int idx = mars.znajdzOrganizm(x, y);
                komunikat = mars.pobierzSzczegoly(idx);
            } else if (wybor == 6) {
                cout << "Podaj x, y oraz nowa energie: ";
                int x = wczytajLiczbe();
                int y = wczytajLiczbe();
                int en = wczytajLiczbe();
                komunikat = mars.modyfikujEnergie(x, y, en)
                    ? "Zaktualizowano poziom energii."
                    : "Nie znaleziono organizmu na tych wspolrzednych.";
            } else if (wybor == 7) {
                cout << "Podaj x, y organizmu do usuniecia: ";
                int x = wczytajLiczbe();
                int y = wczytajLiczbe();
                int idx = mars.znajdzOrganizm(x, y);
                if (idx != -1) {
                    mars.usunOrganizm(idx);
                    komunikat = "Usunieto organizm ze wskazanego pola.";
                } else {
                    komunikat = "Brak organizmu na tym polu.";
                }
            } else if (wybor != 0) {
                throw invalid_argument("Nieznana opcja menu: " + to_string(wybor));
            }
        } catch (const out_of_range& e) {
            komunikat = string("WYJATEK ZAKRESU: ") + e.what();
        } catch (const invalid_argument& e) {
            komunikat = string("WYJATEK ARGUMENTU: ") + e.what();
        } catch (const exception& e) {
            komunikat = string("WYJATEK SYSTEMOWY: ") + e.what();
        }
    }

    return 0;
}