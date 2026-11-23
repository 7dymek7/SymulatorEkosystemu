#include "Organizm.h"

Organizm::Organizm() : symbol('?'), energia(0), pozycja(Pozycja()) {}

Organizm::Organizm(char sym, int startEnergia, Pozycja startPozycja)
    : symbol(sym), energia(startEnergia), pozycja(startPozycja) {}

char Organizm::getSymbol() const { return symbol; }
int Organizm::getEnergia() const { return energia; }
Pozycja Organizm::getPozycja() const { return pozycja; }
bool Organizm::czyZywy() const { return energia > 0; }

void Organizm::setEnergia(int nowaEnergia) {
    if (nowaEnergia < 0) {
        energia = 0;
    } else if (nowaEnergia > 30) {
        energia = 30;
    } else {
        energia = nowaEnergia;
    }
}

void Organizm::setPozycja(Pozycja nowaPozycja) { pozycja = nowaPozycja; }

void Organizm::zmienEnergie(int delta) {
    setEnergia(energia + delta);
}