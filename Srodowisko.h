#ifndef SRODOWISKO_H
#define SRODOWISKO_H

#include <vector>
#include "Organizm.h"

class Srodowisko {
private:
    std::vector<Organizm> organizmy;

public:
    Srodowisko();

    // Podstawowe operacje na obiektach (wymóg na 3.0)
    void dodajOrganizm(Organizm org);
    void usunOrganizm(int indeks);
    void wyswietlStan();
};

#endif