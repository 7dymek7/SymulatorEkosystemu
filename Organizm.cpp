#include "Organizm.h"
#include <stdexcept>

Organizm::Organizm() : symbol('?'), energia(0), pozycja(Pozycja()) {}

Organizm::Organizm(char sym, int startEnergia, Pozycja startPozycja)
    : symbol(sym), energia(startEnergia), pozycja(startPozycja) {
    if (startEnergia <= 0) {
        throw invalid_argument("Energia poczatkowa organizmu musi byc dodatnia!");
    }
}

char Organizm::getSymbol() const { return symbol; }
int Organizm::getEnergia() const { return energia; }
Pozycja Organizm::getPozycja() const { return pozycja; }
bool Organizm::czyZywy() const { return energia > 0; }

void Organizm::setEnergia(int nowaEnergia) {
    energia = (nowaEnergia < 0) ? 0 : nowaEnergia;
}

void Organizm::setPozycja(Pozycja nowaPozycja) { pozycja = nowaPozycja; }

void Organizm::zmienEnergie(int delta) {
    setEnergia(energia + delta);
}

bool Organizm::operator<(const Organizm& inny) const {
    return energia < inny.energia;
}

ostream& operator<<(ostream& os, const Organizm& org) {
    os << org.getTyp() << " [" << org.symbol << "]"
       << " | Energia: " << org.energia
       << " | Pozycja: " << org.pozycja;
    return os;
}