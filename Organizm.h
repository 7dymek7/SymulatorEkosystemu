#ifndef ORGANIZM_H
#define ORGANIZM_H

#include <string>
#include "Pozycja.h"

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

    // Metody wirtualne realizujace polimorfizm (ocena 4.0)
    virtual void wykonajAkcje(int szerokosc, int wysokosc, bool burzaPylowa) = 0;
    virtual std::string getTyp() const = 0;
    virtual std::string getKolorANSI() const = 0;
};

#endif