#ifndef SRODOWISKO_H
#define SRODOWISKO_H

#include <vector>
#include "Organizm.h"

class Srodowisko {
private:
    int szerokosc;
    int wysokosc;
    std::vector<Organizm> organizmy;

public:
    Srodowisko();
    Srodowisko(int szer, int wys);

    void dodajOrganizm(Organizm org);
    void usunOrganizm(int indeks);
    int znajdzOrganizm(int x, int y);
    bool modyfikujEnergie(int x, int y, int nowaEnergia);

    void wykonajTure();
    void rysujPlansze();
    void wypiszSzczegoly(int indeks);
};

#endif