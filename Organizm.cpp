#include "Organizm.h"

Organizm::Organizm() : symbol('?'), energia(0), pozycja(Pozycja()) {}
Organizm::Organizm(char sym, int startEnergia, Pozycja startPozycja) 
    : symbol(sym), energia(startEnergia), pozycja(startPozycja) {}

char Organizm::getSymbol() { return symbol; }
int Organizm::getEnergia() { return energia; }
Pozycja Organizm::getPozycja() { return pozycja; }

void Organizm::setEnergia(int nowaEnergia) { energia = nowaEnergia; }
void Organizm::setPozycja(Pozycja nowaPozycja) { pozycja = nowaPozycja; }