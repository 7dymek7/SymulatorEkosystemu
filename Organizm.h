#ifndef ORGANIZM_H
#define ORGANIZM_H

#include "Pozycja.h"

class Organizm {
private:
    char symbol;
    int energia;
    Pozycja pozycja;

public:
    Organizm();
    Organizm(char sym, int startEnergia, Pozycja startPozycja);

    char getSymbol();
    int getEnergia();
    Pozycja getPozycja();
    
    void setEnergia(int nowaEnergia);
    void setPozycja(Pozycja nowaPozycja);
};

#endif