#include "PorostMarsjanski.h"
#include <stdexcept>

PorostMarsjanski::PorostMarsjanski()
    : Organizm('*', 8, Pozycja()), tempoFotosyntezy(2) {}

PorostMarsjanski::PorostMarsjanski(int startEnergia, Pozycja startPozycja, int tempo)
    : Organizm('*', startEnergia, startPozycja), tempoFotosyntezy(tempo) {
    if (tempo <= 0) {
        throw invalid_argument("Tempo fotosyntezy musi byc dodatnie!");
    }
}

int PorostMarsjanski::getTempoFotosyntezy() const {
    return tempoFotosyntezy;
}

void PorostMarsjanski::wykonajAkcje(int, int, bool burzaPylowa) {
    if (burzaPylowa) {
        zmienEnergie(1);
    } else {
        zmienEnergie(tempoFotosyntezy);
    }
}

string PorostMarsjanski::getTyp() const {
    return "Porost Marsjanski";
}

string PorostMarsjanski::getKolorANSI() const {
    return "\033[1;92m";
}