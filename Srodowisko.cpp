#include "Srodowisko.h"
#include "PorostMarsjanski.h"
#include "DrapieznikMarsjanski.h"
#include <iostream>
#include <iomanip>
#include <sstream>
#include <cstdlib>
#include <algorithm>
#include <stdexcept>

using namespace std;

namespace {
    const string RESET = "\033[0m";
    const string MARS_RAMKA = "\033[1;38;5;208m";
    const string MARS_GLEBA = "\033[38;5;130m";
    const string CYAN = "\033[1;96m";
    const string CZERWONY = "\033[1;91m";
    const string SZARY = "\033[90m";
}

Srodowisko::Srodowisko()
    : szerokosc(12), wysokosc(8), sol(1), temperatura(-48), burzaPylowa(false) {}

Srodowisko::Srodowisko(int szer, int wys)
    : szerokosc(szer), wysokosc(wys), sol(1), temperatura(-48), burzaPylowa(false) {
    if (szer <= 0 || wys <= 0) {
        throw invalid_argument("Wymiary sektora Marsa musza byc dodatnie!");
    }
}

void Srodowisko::sprawdzWspolrzedne(int x, int y) const {
    if (x < 0 || x >= szerokosc || y < 0 || y >= wysokosc) {
        throw out_of_range("Wspolrzedne (" + to_string(x) + ", " + to_string(y) +
                           ") wykraczaja poza sektor Marsa [0.." + to_string(szerokosc - 1) +
                           ", 0.." + to_string(wysokosc - 1) + "]!");
    }
}

int Srodowisko::getSol() const { return sol; }
int Srodowisko::getTemperatura() const { return temperatura; }
bool Srodowisko::czyBurzaPylowa() const { return burzaPylowa; }
int Srodowisko::getLiczbaOrganizmow() const { return static_cast<int>(organizmy.size()); }

void Srodowisko::dodajOrganizm(unique_ptr<Organizm> org) {
    if (!org) {
        throw invalid_argument("Proba dodania pustego wskaznika na organizm!");
    }
    sprawdzWspolrzedne(org->getPozycja().getX(), org->getPozycja().getY());
    organizmy.push_back(move(org));
}

void Srodowisko::usunOrganizm(int indeks) {
    if (indeks < 0 || indeks >= static_cast<int>(organizmy.size())) {
        throw out_of_range("Niepoprawny indeks organizmu do usuniecia!");
    }
    organizmy.erase(organizmy.begin() + indeks);
}

int Srodowisko::znajdzOrganizm(int x, int y) const {
    sprawdzWspolrzedne(x, y);
    Pozycja cel(x, y);
    for (int i = 0; i < static_cast<int>(organizmy.size()); ++i) {
        // Praktyczne wykorzystanie przeciazonego operatora ==
        if (organizmy[i]->getPozycja() == cel) {
            return i;
        }
    }
    return -1;
}

bool Srodowisko::modyfikujEnergie(int x, int y, int nowaEnergia) {
    sprawdzWspolrzedne(x, y);
    if (nowaEnergia < 0) {
        throw invalid_argument("Energia organizmu nie moze byc wartoscia ujemna!");
    }
    int indeks = znajdzOrganizm(x, y);
    if (indeks == -1) return false;
    organizmy[indeks]->setEnergia(nowaEnergia);
    return true;
}

void Srodowisko::wykonajTure() {
    ++sol;
    temperatura = -65 + (rand() % 46);
    burzaPylowa = (rand() % 4 == 0);
    logi.clear();

    if (burzaPylowa) {
        logi.push_back("UWAGA: Nad sektorem przechodzi burza pylowa!");
    }

    for (auto& org : organizmy) {
        org->wykonajAkcje(szerokosc, wysokosc, burzaPylowa);
    }

    for (size_t i = 0; i < organizmy.size(); ++i) {
        if (organizmy[i]->getTyp() == "Drapieznik Marsjanski" && organizmy[i]->czyZywy()) {
            for (size_t j = 0; j < organizmy.size(); ++j) {
                if (i == j) continue;
                if (organizmy[j]->getTyp() == "Porost Marsjanski" && organizmy[j]->czyZywy()) {
                    // Wykorzystanie operatora == dla klasy Pozycja
                    if (organizmy[i]->getPozycja() == organizmy[j]->getPozycja()) {
                        organizmy[i]->zmienEnergie(organizmy[j]->getEnergia());
                        organizmy[j]->setEnergia(0);
                        ostringstream oss;
                        oss << "Drapieznik pochlonol Porost na polu " << organizmy[i]->getPozycja() << ".";
                        logi.push_back(oss.str());
                    }
                }
            }
        }
    }

    vector<unique_ptr<Organizm>> noweOrganizmy;
    for (auto& org : organizmy) {
        if (org->getTyp() == "Porost Marsjanski" && org->getEnergia() >= 14) {
            int nx = org->getPozycja().getX() + ((rand() % 3) - 1);
            int ny = org->getPozycja().getY() + ((rand() % 3) - 1);
            if (nx >= 0 && nx < szerokosc && ny >= 0 && ny < wysokosc && znajdzOrganizm(nx, ny) == -1) {
                org->zmienEnergie(-7);
                noweOrganizmy.push_back(make_unique<PorostMarsjanski>(6, Pozycja(nx, ny), 2));
                logi.push_back("Porost rozrosl sie na pole (" +
                    to_string(nx) + ", " + to_string(ny) + ").");
            }
        }
    }
    for (auto& nowy : noweOrganizmy) {
        organizmy.push_back(move(nowy));
    }

    for (int i = static_cast<int>(organizmy.size()) - 1; i >= 0; --i) {
        if (!organizmy[i]->czyZywy()) {
            logi.push_back(organizmy[i]->getTyp() + " wyginal z braku energii.");
            usunOrganizm(i);
        }
    }
}

string Srodowisko::pobierzSzczegoly(int indeks) const {
    if (indeks < 0 || indeks >= static_cast<int>(organizmy.size())) {
        return "Brak organizmu na wskazanych wspolrzednych.";
    }
    // Praktyczne wykorzystanie przeciazonego operatora <<
    ostringstream oss;
    oss << "SKAN: " << *organizmy[indeks];
    return oss.str();
}

void Srodowisko::rysujPlansze(const string& komunikat) const {
    cout << "\033[2J\033[H";

    int liczbaPorostow = 0;
    int liczbaDrapieznikow = 0;
    const Organizm* lider = nullptr;

    for (const auto& org : organizmy) {
        if (org->getTyp() == "Porost Marsjanski") ++liczbaPorostow;
        else if (org->getTyp() == "Drapieznik Marsjanski") ++liczbaDrapieznikow;

        // Praktyczne wykorzystanie przeciazonego operatora <
        if (lider == nullptr || *lider < *org) {
            lider = org.get();
        }
    }

    cout << MARS_RAMKA << "╔══════════════════════════════════════════════════════╗\n";
    cout << "║     SYMULATOR EKOSYSTEMU: SEKTOR KRATERU JEZERO      ║\n";
    cout << "╚══════════════════════════════════════════════════════╝" << RESET << "\n";

    cout << CYAN << " SOL: " << setw(3) << sol << RESET
         << " | " << MARS_RAMKA << "TEMP: " << temperatura << " C" << RESET
         << " | " << (burzaPylowa ? CZERWONY + "BURZA PYLOWA" : CYAN + "POGODA STABILNA") << RESET << "\n";
    cout << " \033[1;92mPorost (*): " << liczbaPorostow << RESET
         << " | \033[1;93mDrapiezniki (X): " << liczbaDrapieznikow << RESET << "\n";

    if (lider != nullptr) {
        // Praktyczne wykorzystanie przeciazonego operatora << bezposrednio na cout
        cout << CYAN << " Lider: " << RESET << *lider << "\n";
    }
    cout << "\n";

    vector<vector<char>> siatka(wysokosc, vector<char>(szerokosc, '.'));
    vector<vector<string>> kolory(wysokosc, vector<string>(szerokosc, MARS_GLEBA));

    for (const auto& org : organizmy) {
        int x = org->getPozycja().getX();
        int y = org->getPozycja().getY();
        siatka[y][x] = org->getSymbol();
        kolory[y][x] = org->getKolorANSI();
    }

    cout << SZARY << "    ";
    for (int x = 0; x < szerokosc; ++x) {
        cout << setw(3) << x;
    }
    cout << RESET << "\n";

    cout << MARS_RAMKA << "   ╔";
    for (int i = 0; i < szerokosc * 3 + 1; ++i) cout << "═";
    cout << "╗" << RESET << "\n";

    for (int y = 0; y < wysokosc; ++y) {
        cout << SZARY << setw(2) << y << " " << MARS_RAMKA << "║" << RESET;
        for (int x = 0; x < szerokosc; ++x) {
            cout << "  " << kolory[y][x] << siatka[y][x] << RESET;
        }
        cout << " " << MARS_RAMKA << "║" << RESET << "\n";
    }

    cout << MARS_RAMKA << "   ╚";
    for (int i = 0; i < szerokosc * 3 + 1; ++i) cout << "═";
    cout << "╝" << RESET << "\n";

    if (!logi.empty()) {
        cout << CYAN << "\n[TELEMETRIA SOLA " << sol << "]" << RESET << "\n";
        int limit = min(static_cast<int>(logi.size()), 4);
        for (int i = 0; i < limit; ++i) {
            cout << " > " << logi[i] << "\n";
        }
    }

    if (!komunikat.empty()) {
        cout << "\n" << CZERWONY << "[STATUS] " << RESET << komunikat << "\n";
    }
}