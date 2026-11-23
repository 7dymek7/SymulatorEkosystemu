#ifndef ORGANIZM_H
#define ORGANIZM_H

#include <string>
#include <iostream>
#include "Pozycja.h"

using namespace std;

class Organizm {
private:
    char symbol;
    int energia;
    Pozycja pozycja;

public:
    Organizm();
    Organizm(char sym, int startEnergia, Pozycja startPozycja);
    virtual ~Organizm() = default;

    char getSymbol() const;
    int getEnergia() const;
    Pozycja getPozycja() const;
    bool czyZywy() const;

    void setEnergia(int nowaEnergia);
    void setPozycja(Pozycja nowaPozycja);
    void zmienEnergie(int delta);

    virtual void wykonajAkcje(int szerokosc, int wysokosc, bool burzaPylowa) = 0;
    virtual string getTyp() const = 0;
    virtual string getKolorANSI() const = 0;

    bool operator<(const Organizm& inny) const;
    friend ostream& operator<<(ostream& os, const Organizm& org);
};

#endif