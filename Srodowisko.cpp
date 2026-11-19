#include "Srodowisko.h"
#include <iostream>
#include <cstdlib>

Srodowisko::Srodowisko() : szerokosc(10), wysokosc(10) {}

Srodowisko::Srodowisko(int szer, int wys) : szerokosc(szer), wysokosc(wys) {}

void Srodowisko::dodajOrganizm(Organizm org) {
    int x = org.getPozycja().getX();
    int y = org.getPozycja().getY();

    if (x >= 0 && x < szerokosc && y >= 0 && y < wysokosc) {
        organizmy.push_back(org);
    } else {
        std::cout << "Blad: pozycja poza granicami planszy!\n";
    }
}

void Srodowisko::usunOrganizm(int indeks) {
    if (indeks >= 0 && indeks < static_cast<int>(organizmy.size())) {
        organizmy.erase(organizmy.begin() + indeks);
    }
}

int Srodowisko::znajdzOrganizm(int x, int y) {
    for (int i = 0; i < static_cast<int>(organizmy.size()); ++i) {
        if (organizmy[i].getPozycja().getX() == x &&
            organizmy[i].getPozycja().getY() == y) {
            return i;
        }
    }
    return -1;
}

bool Srodowisko::modyfikujEnergie(int x, int y, int nowaEnergia) {
    int indeks = znajdzOrganizm(x, y);
    if (indeks == -1) {
        return false;
    }
    organizmy[indeks].setEnergia(nowaEnergia);
    return true;
}

void Srodowisko::wykonajTure() {
    for (int i = 0; i < static_cast<int>(organizmy.size()); ++i) {
        int dx = (std::rand() % 3) - 1; // -1, 0 lub 1
        int dy = (std::rand() % 3) - 1;

        int noweX = organizmy[i].getPozycja().getX() + dx;
        int noweY = organizmy[i].getPozycja().getY() + dy;

        // Zabezpieczenie przed wyjsciem poza siatke
        if (noweX < 0) noweX = 0;
        if (noweX >= szerokosc) noweX = szerokosc - 1;
        if (noweY < 0) noweY = 0;
        if (noweY >= wysokosc) noweY = wysokosc - 1;

        organizmy[i].setPozycja(Pozycja(noweX, noweY));
        organizmy[i].zmienEnergie(-1);
    }

    // Usuwanie martwych organizmow
    for (int i = static_cast<int>(organizmy.size()) - 1; i >= 0; --i) {
        if (!organizmy[i].czyZywy()) {
            usunOrganizm(i);
        }
    }
}

void Srodowisko::rysujPlansze() {
    std::vector<std::vector<char>> siatka(wysokosc, std::vector<char>(szerokosc, '.'));

    for (int i = 0; i < static_cast<int>(organizmy.size()); ++i) {
        int x = organizmy[i].getPozycja().getX();
        int y = organizmy[i].getPozycja().getY();
        siatka[y][x] = organizmy[i].getSymbol();
    }

    std::cout << "\n";
    for (int x = 0; x < szerokosc + 2; ++x) std::cout << '#';
    std::cout << "\n";

    for (int y = 0; y < wysokosc; ++y) {
        std::cout << '#';
        for (int x = 0; x < szerokosc; ++x) {
            std::cout << siatka[y][x];
        }
        std::cout << "#\n";
    }

    for (int x = 0; x < szerokosc + 2; ++x) std::cout << '#';
    std::cout << "\nLiczba zywych organizmow: " << organizmy.size() << "\n";
}

void Srodowisko::wypiszSzczegoly(int indeks) {
    if (indeks >= 0 && indeks < static_cast<int>(organizmy.size())) {
        Organizm& org = organizmy[indeks];
        std::cout << "Znaleziono: [" << org.getSymbol()
                  << "] | Energia: " << org.getEnergia()
                  << " | Pozycja: (" << org.getPozycja().getX()
                  << ", " << org.getPozycja().getY() << ")\n";
    }
}