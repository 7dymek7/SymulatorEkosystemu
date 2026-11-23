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
    const string CYAN = "\033[1;96m";
    const string CZERWONY = "\033[1;91m";
    const string ZIELONY = "\033[1;92m";
    const string ZOLTY = "\033[1;93m";
    const string SZARY = "\033[90m";

    // Tla kafelkow mapy (256-kolorowe ANSI)
    const string TLO_GLEBA = "\033[48;5;52m";
    const string TLO_BURZA = "\033[48;5;94m";
    const string TLO_LIDER = "\033[48;5;24m";
    const string KROPKA_GLEBA = "\033[38;5;130m";
    const string WIATR_BURZA = "\033[38;5;214m";

    string zbudujPasek(int wartosc, int maksimum, int dlugosc) {
        if (maksimum <= 0) maksimum = 1;
        int wypelnione = (wartosc * dlugosc) / maksimum;
        if (wypelnione > dlugosc) wypelnione = dlugosc;
        if (wypelnione < 0) wypelnione = 0;

        string pasek = "[";
        for (int i = 0; i < dlugosc; ++i) {
            pasek += (i < wypelnione) ? "█" : "░";
        }
        pasek += "]";
        return pasek;
    }
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

int Srodowisko::getSzerokosc() const { return szerokosc; }
int Srodowisko::getWysokosc() const { return wysokosc; }
int Srodowisko::getSol() const { return sol; }
int Srodowisko::getTemperatura() const { return temperatura; }
bool Srodowisko::czyBurzaPylowa() const { return burzaPylowa; }
int Srodowisko::getLiczbaOrganizmow() const { return static_cast<int>(organizmy.size()); }
const vector<unique_ptr<Organizm>>& Srodowisko::getOrganizmy() const { return organizmy; }
const vector<string>& Srodowisko::getLogi() const { return logi; }

const Organizm* Srodowisko::znajdzLidera() const {
    const Organizm* lider = nullptr;
    for (const auto& org : organizmy) {
        // Praktyczne wykorzystanie przeciazonego operatora <
        if (lider == nullptr || *lider < *org) {
            lider = org.get();
        }
    }
    return lider;
}

void Srodowisko::inicjalizujDomyslnyEkosystem() {
    dodajOrganizm(make_unique<PorostMarsjanski>(10, Pozycja(2, 2), 2));
    dodajOrganizm(make_unique<PorostMarsjanski>(9, Pozycja(9, 5), 2));
    dodajOrganizm(make_unique<PorostMarsjanski>(8, Pozycja(5, 6), 3));
    dodajOrganizm(make_unique<DrapieznikMarsjanski>(18, Pozycja(3, 3), 1));
    dodajOrganizm(make_unique<DrapieznikMarsjanski>(20, Pozycja(8, 2), 2));
}

void Srodowisko::resetuj() {
    organizmy.clear();
    logi.clear();
    sol = 1;
    temperatura = -48;
    burzaPylowa = false;
    inicjalizujDomyslnyEkosystem();
}

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
    ostringstream oss;
    oss << "SKAN: " << *organizmy[indeks];
    return oss.str();
}

void Srodowisko::rysujPlansze(const string& komunikat) const {
    cout << "\033[2J\033[H";

    int liczbaPorostow = 0;
    int liczbaDrapieznikow = 0;
    for (const auto& org : organizmy) {
        if (org->getTyp() == "Porost Marsjanski") ++liczbaPorostow;
        else if (org->getTyp() == "Drapieznik Marsjanski") ++liczbaDrapieznikow;
    }

    const Organizm* lider = znajdzLidera();
    int maxPop = max(1, static_cast<int>(organizmy.size()));

    // Naglowek misji
    cout << MARS_RAMKA << "╔════════════════════════════════════════════════════════════════════════════╗\n";
    cout << "║            SYMULATOR EKOSYSTEMU: SEKTOR KRATERU JEZERO (MARS)              ║\n";
    cout << "╚════════════════════════════════════════════════════════════════════════════╝" << RESET << "\n";

    cout << CYAN << " SOL: " << setw(3) << sol << RESET
         << "  |  " << MARS_RAMKA << "TEMP: " << temperatura << " C" << RESET
         << "  |  " << (burzaPylowa ? CZERWONY + "ALERT: BURZA PYLOWA" : ZIELONY + "POGODA STABILNA") << RESET << "\n";

    if (lider != nullptr) {
        cout << CYAN << " Lider: " << RESET << *lider << "\n";
    } else {
        cout << CZERWONY << " Sektor wymarl - uzyj opcji 9 (Reset) lub dodaj nowe organizmy." << RESET << "\n";
    }
    cout << "\n";

    // Siatka wskaznikow do rysowania
    vector<vector<const Organizm*>> mapa(wysokosc, vector<const Organizm*>(szerokosc, nullptr));
    for (const auto& org : organizmy) {
        int x = org->getPozycja().getX();
        int y = org->getPozycja().getY();
        mapa[y][x] = org.get();
    }

    // Prawy panel (wyswietlany obok mapy)
    vector<string> panelPrawy = {
        CYAN + "══ LEGENDA I BILANS SEKTORA ═════════" + RESET,
        TLO_GLEBA + ZIELONY + " * " + RESET + " Porost Marsjanski (fotosynteza)",
        TLO_GLEBA + ZOLTY + " X " + RESET + " Drapieznik (poluje na porost)",
        TLO_LIDER + "\033[1;97m L " + RESET + " Podswietlenie lidera energii",
        TLO_BURZA + WIATR_BURZA + " ~ " + RESET + " Powierzchnia w burzy pylowej",
        SZARY + "─────────────────────────────────────" + RESET,
        ZIELONY + "Porosty     " + RESET + zbudujPasek(liczbaPorostow, maxPop, 10) + " " + to_string(liczbaPorostow),
        ZOLTY + "Drapiezniki " + RESET + zbudujPasek(liczbaDrapieznikow, maxPop, 10) + " " + to_string(liczbaDrapieznikow),
        CYAN + "Lacznie     " + RESET + to_string(organizmy.size()) + " zywych organizmow",
        CYAN + "Warunki     " + RESET + (burzaPylowa ? CZERWONY + "Burza (-energia, -swiatlo)" : ZIELONY + "Stabilne (pelny wzrost)") + RESET
    };

    // Os X nad mapa
    cout << SZARY << "    ";
    for (int x = 0; x < szerokosc; ++x) {
        if (x < 10) cout << " " << x << " ";
        else cout << " " << x;
    }
    cout << RESET << "\n";

    // Gorna ramka mapy + linia 0 prawego panelu
    cout << MARS_RAMKA << "   ╔";
    for (int i = 0; i < szerokosc * 3; ++i) cout << "═";
    cout << "╗" << RESET << "   " << panelPrawy[0] << "\n";

    // Wiersze mapy + prawy panel
    for (int y = 0; y < wysokosc; ++y) {
        cout << SZARY << setw(2) << y << " " << MARS_RAMKA << "║" << RESET;
        for (int x = 0; x < szerokosc; ++x) {
            const Organizm* org = mapa[y][x];
            string tlo = burzaPylowa ? TLO_BURZA : TLO_GLEBA;
            if (org != nullptr && org == lider) {
                tlo = TLO_LIDER;
            }

            if (org != nullptr) {
                cout << tlo << org->getKolorANSI() << " " << org->getSymbol() << " " << RESET;
            } else if (burzaPylowa) {
                char wiatr = ((x + y + sol) % 2 == 0) ? '~' : ':';
                cout << tlo << WIATR_BURZA << " " << wiatr << " " << RESET;
            } else {
                cout << tlo << KROPKA_GLEBA << " . " << RESET;
            }
        }
        cout << MARS_RAMKA << "║" << RESET;

        int idxPanelu = y + 1;
        if (idxPanelu < static_cast<int>(panelPrawy.size())) {
            cout << "   " << panelPrawy[idxPanelu];
        }
        cout << "\n";
    }

    // Dolna ramka mapy + ostatnia linia prawego panelu
    cout << MARS_RAMKA << "   ╚";
    for (int i = 0; i < szerokosc * 3; ++i) cout << "═";
    cout << "╝" << RESET;
    if (wysokosc + 1 < static_cast<int>(panelPrawy.size())) {
        cout << "   " << panelPrawy[wysokosc + 1];
    }
    cout << "\n";

    // Dziennik zdarzen
    if (!logi.empty()) {
        cout << CYAN << "\n[TELEMETRIA SOLA " << sol << "]" << RESET << "\n";
        int limit = min(static_cast<int>(logi.size()), 3);
        for (int i = 0; i < limit; ++i) {
            cout << " > " << logi[i] << "\n";
        }
    }

    if (!komunikat.empty()) {
        cout << "\n" << CZERWONY << "[STATUS] " << RESET << komunikat << "\n";
    }
}

void Srodowisko::wyswietlTabeleOrganizmow() const {
    cout << "\033[2J\033[H";
    cout << MARS_RAMKA << "╔════════════════════════════════════════════════════════════════════════════╗\n";
    cout << "║                     RAPORT SEKTORA - LISTA ORGANIZMOW                      ║\n";
    cout << "╚════════════════════════════════════════════════════════════════════════════╝" << RESET << "\n\n";

    if (organizmy.empty()) {
        cout << CZERWONY << "Brak zywych organizmow w sektorze Marsa.\n" << RESET;
        return;
    }

    const Organizm* lider = znajdzLidera();
    int maxEnergia = lider ? max(20, lider->getEnergia()) : 20;

    cout << CYAN << left << setw(5) << "ID"
         << setw(25) << "Gatunek"
         << setw(12) << "Pozycja"
         << setw(10) << "Energia"
         << "Poziom energii" << RESET << "\n";
    cout << SZARY << "──────────────────────────────────────────────────────────────────────────────" << RESET << "\n";

    for (size_t i = 0; i < organizmy.size(); ++i) {
        const auto& org = organizmy[i];
        ostringstream pozStream;
        pozStream << org->getPozycja();

        string znacznikLidera = (org.get() == lider) ? (CYAN + " [LIDER]" + RESET) : "";

        cout << left << setw(5) << i
             << org->getKolorANSI() << setw(25) << org->getTyp() << RESET
             << setw(12) << pozStream.str()
             << setw(10) << org->getEnergia()
             << org->getKolorANSI() << zbudujPasek(org->getEnergia(), maxEnergia, 14) << RESET
             << znacznikLidera << "\n";
    }
}