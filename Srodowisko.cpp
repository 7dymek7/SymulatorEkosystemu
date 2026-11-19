#include "Srodowisko.h"
#include <iostream>

Srodowisko::Srodowisko() {}

void Srodowisko::dodajOrganizm(Organizm org) {
    organizmy.push_back(org);
}

void Srodowisko::usunOrganizm(int indeks) {
    if (indeks >= 0 && indeks < organizmy.size()) {
        organizmy.erase(organizmy.begin() + indeks);
    }
}

void Srodowisko::wyswietlStan() {
    std::cout << "Liczba organizmow w srodowisku: " << organizmy.size() << std::endl;
}