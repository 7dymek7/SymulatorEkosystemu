#include "PorostMarsjanski.h"

PorostMarsjanski::PorostMarsjanski()
    : Organizm('P', 10, Pozycja(0, 0)), tempoWzrostu(2) {}

PorostMarsjanski::PorostMarsjanski(Pozycja startPozycja, int startEnergia, int wzrost)
    : Organizm('P', startEnergia, startPozycja), tempoWzrostu(wzrost) {}

int PorostMarsjanski::getTempoWzrostu() const {
    return tempoWzrostu;
}

void PorostMarsjanski::wykonajAkcje(int /*szerokosc*/, int /*wysokosc*/, bool burzaPylowa) {
    // Porost nie przemieszcza sie; podczas burzy traci energie, w sloncu rosnie
    if (burzaPylowa) {
        zmienEnergie(-2);
    } else {
        zmienEnergie(tempoWzrostu);
    }
}

std::string PorostMarsjanski::getTyp() const {
    return "Porost Marsjanski";
}

std::string PorostMarsjanski::getKolorANSI() const {
    return "\033[1;32m"; // Jasna zielen
}