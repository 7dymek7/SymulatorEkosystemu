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

int wczytajLiczbe(const string& podpowiedz = "") {
    if (!podpowiedz.empty()) {
        cout << podpowiedz;
    }
    int wartosc;
    if (!(cin >> wartosc)) {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        throw invalid_argument("Wprowadzono niepoprawny format danych (oczekiwano liczby calkowitej)!");
    }
    return wartosc;
}

void poczekajNaEnter(bool wyczyscBufor = true) {
    cout << "\n\033[1;96mNacisnij [Enter], aby kontynuowac...\033[0m";
    if (wyczyscBufor) {
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }
    cin.get();
}

void pokazBriefing(bool zMenu) {
    cout << "\033[2J\033[H";
    cout << "\033[1;38;5;208m";
    cout << "╔════════════════════════════════════════════════════════════════════════════╗\n";
    cout << "║             MISJA MARS: SYMULATOR EKOSYSTEMU KRATERU JEZERO                ║\n";
    cout << "╚════════════════════════════════════════════════════════════════════════════╝\033[0m\n\n";

    cout << "\033[1;96mZASADY SYMULACJI:\033[0m\n"
         << "  \033[1;92m*\033[0m \033[1mPorost Marsjanski\033[0m   - pozostaje w miejscu, laduje energie ze slonca\n"
         << "                          i rozrasta sie na sasiednie pola (energia >= 14).\n"
         << "  \033[1;93mX\033[0m \033[1mDrapieznik\033[0m          - przemieszcza sie po mapie, traci energie na ruch\n"
         << "                          i pochlania Porost, gdy wejdzie na jego pole.\n"
         << "  \033[1;91m~\033[0m \033[1mBurza pylowa\033[0m        - pojawia sie losowo; ogranicza fotosynteze porostow\n"
         << "                          i zwieksza zuzycie energii przez drapiezniki.\n"
         << "  \033[48;5;24m\033[1;97m L \033[0m \033[1mLider sektora\033[0m     - niebieskie podswietlenie wskazuje najsilniejszy\n"
         << "                          organizm na mapie (porownanie operatorem <).\n\n";

    cout << "\033[1;96mWSKAZOWKI:\033[0m\n"
         << "  - Uzyj opcji [2], aby obejrzec plynna animacje wielu dni marsjanskich (Soli).\n"
         << "  - Uzyj opcji [8], aby podejrzec tabele wszystkich organizmow i ich wspolrzedne.\n"
         << "  - Bledne wspolrzedne lub wartosci sa bezpiecznie obslugiwane przez wyjatki.\n";

    poczekajNaEnter(zMenu);
}

int main() {
    inicjalizujKonsole();
    srand(static_cast<unsigned int>(time(nullptr)));

    Srodowisko mars(12, 8);
    mars.inicjalizujDomyslnyEkosystem();

    pokazBriefing(false);

    string komunikat = "Misja terraformacji Marsa rozpoczeta.";
    int wybor = -1;

    while (wybor != 0) {
        mars.rysujPlansze(komunikat);
        komunikat.clear();

        cout << "\n\033[1;38;5;208m─── PANEL KONTROLNY ──────────────────────────────────────────────────────────\033[0m\n"
             << "  1. Nastepny Sol (1 tura)           6. Modyfikuj energie organizmu\n"
             << "  2. Animacja (wybrana liczba Soli)  7. Usun organizm z pola (x, y)\n"
             << "  3. Dodaj Porost Marsjanski (*)     8. Raport sektora (tabela organizmow)\n"
             << "  4. Dodaj Drapieznika (X)           9. Resetuj symulacje\n"
             << "  5. Skanuj pole (x, y)             10. Pomoc / Briefing misji\n"
             << "  0. Zakoncz misje\n";

        try {
            wybor = wczytajLiczbe("\nWybor: ");
            int maxX = mars.getSzerokosc() - 1;
            int maxY = mars.getWysokosc() - 1;

            if (wybor == 1) {
                mars.wykonajTure();
            } else if (wybor == 2) {
                int liczbaSoli = wczytajLiczbe("Ile Soli zasymulowac [1-50]: ");
                if (liczbaSoli < 1 || liczbaSoli > 50) {
                    throw invalid_argument("Liczba Soli do animacji musi miescic sie w przedziale 1-50!");
                }
                for (int i = 0; i < liczbaSoli; ++i) {
                    mars.wykonajTure();
                    mars.rysujPlansze("Trwa symulacja automatyczna (" + to_string(i + 1) + "/" + to_string(liczbaSoli) + ")...");
                    this_thread::sleep_for(chrono::milliseconds(400));
                }
                komunikat = "Zakonczono cykl " + to_string(liczbaSoli) + " Soli.";
            } else if (wybor == 3) {
                int en = wczytajLiczbe("Podaj energie poczatkowa (> 0): ");
                int x = wczytajLiczbe("Podaj wspolrzedna X [0-" + to_string(maxX) + "]: ");
                int y = wczytajLiczbe("Podaj wspolrzedna Y [0-" + to_string(maxY) + "]: ");
                mars.dodajOrganizm(make_unique<PorostMarsjanski>(en, Pozycja(x, y)));
                komunikat = "Dodano Porost Marsjanski na polu (" + to_string(x) + ", " + to_string(y) + ").";
            } else if (wybor == 4) {
                int en = wczytajLiczbe("Podaj energie poczatkowa (> 0): ");
                int x = wczytajLiczbe("Podaj wspolrzedna X [0-" + to_string(maxX) + "]: ");
                int y = wczytajLiczbe("Podaj wspolrzedna Y [0-" + to_string(maxY) + "]: ");
                int zasieg = wczytajLiczbe("Podaj zasieg ruchu [1-3]: ");
                mars.dodajOrganizm(make_unique<DrapieznikMarsjanski>(en, Pozycja(x, y), zasieg));
                komunikat = "Dodano Drapieznika Marsjanskiego na polu (" + to_string(x) + ", " + to_string(y) + ").";
            } else if (wybor == 5) {
                int x = wczytajLiczbe("Podaj wspolrzedna X [0-" + to_string(maxX) + "]: ");
                int y = wczytajLiczbe("Podaj wspolrzedna Y [0-" + to_string(maxY) + "]: ");
                int idx = mars.znajdzOrganizm(x, y);
                komunikat = mars.pobierzSzczegoly(idx);
            } else if (wybor == 6) {
                int x = wczytajLiczbe("Podaj wspolrzedna X [0-" + to_string(maxX) + "]: ");
                int y = wczytajLiczbe("Podaj wspolrzedna Y [0-" + to_string(maxY) + "]: ");
                int en = wczytajLiczbe("Podaj nowa wartosc energii (>= 0): ");
                komunikat = mars.modyfikujEnergie(x, y, en)
                    ? "Zaktualizowano poziom energii."
                    : "Nie znaleziono organizmu na tych wspolrzednych.";
            } else if (wybor == 7) {
                int x = wczytajLiczbe("Podaj wspolrzedna X [0-" + to_string(maxX) + "]: ");
                int y = wczytajLiczbe("Podaj wspolrzedna Y [0-" + to_string(maxY) + "]: ");
                int idx = mars.znajdzOrganizm(x, y);
                if (idx != -1) {
                    mars.usunOrganizm(idx);
                    komunikat = "Usunieto organizm ze wskazanego pola.";
                } else {
                    komunikat = "Brak organizmu na tym polu.";
                }
            } else if (wybor == 8) {
                mars.wyswietlTabeleOrganizmow();
                poczekajNaEnter(true);
            } else if (wybor == 9) {
                mars.resetuj();
                komunikat = "Zresetowano symulacje do stanu poczatkowego (Sol 1).";
            } else if (wybor == 10) {
                pokazBriefing(true);
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