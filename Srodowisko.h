#ifndef SRODOWISKO_H
#define SRODOWISKO_H

#include <vector>
#include <memory>
#include <string>
#include "Organizm.h"

class Srodowisko {
private:
    int szerokosc;
    int wysokosc;
    int numerSola;
    bool burzaPylowa;
    std::string ostatniKomunikat;
    std::vector<std::unique_ptr<Organizm>> organizmy;

public:
    Srodowisko();
    Srodowisko(int szer, int wys);

    void dodajOrganizm(std::unique_ptr<Organizm> org);
    void usunOrganizm(int indeks);
    int znajdzOrganizm(int x, int y) const;
    bool modyfikujEnergie(int x, int y, int nowaEnergia);

    int policzTyp(const std::string& typ) const;
    int getNumerSola() const;
    bool czyBurzaPylowa() const;

    void wykonajTure();
    void rysujPlansze() const;
    std::string pobierzSzczegoly(int indeks) const;
};

#endif