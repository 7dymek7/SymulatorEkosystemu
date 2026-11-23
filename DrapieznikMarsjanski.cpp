#include "DrapieznikMarsjanski.h"
#include <cstdlib>
#include <stdexcept>

using namespace std;

DrapieznikMarsjanski::DrapieznikMarsjanski()
    : Organizm('X', 15, Pozycja()), zasiegRuchu(1) {}

DrapieznikMarsjanski::DrapieznikMarsjanski(int startEnergia, Pozycja startPozycja, int zasieg)
    : Organizm('X', startEnergia, startPozycja), zasiegRuchu(zasieg) {
    if (zasieg < 1 || zasieg > 3) {
        throw invalid_argument("Zasieg ruchu drapieznika musi miescic sie w przedziale 1-3!");
    }
}

int DrapieznikMarsjanski::getZasiegRuchu() const {
    return zasiegRuchu;
}

void DrapieznikMarsjanski::wykonajAkcje(int szerokosc, int wysokosc, bool burzaPylowa) {
    int zakres = 2 * zasiegRuchu + 1;
    int dx = (rand() % zakres) - zasiegRuchu;
    int dy = (rand() % zakres) - zasiegRuchu;

    int noweX = getPozycja().getX() + dx;
    int noweY = getPozycja().getY() + dy;

    if (noweX < 0) noweX = 0;
    if (noweX >= szerokosc) noweX = szerokosc - 1;
    if (noweY < 0) noweY = 0;
    if (noweY >= wysokosc) noweY = wysokosc - 1;

    setPozycja(Pozycja(noweX, noweY));
    zmienEnergie(burzaPylowa ? -3 : -2);
}

string DrapieznikMarsjanski::getTyp() const {
    return "Drapieznik Marsjanski";
}

string DrapieznikMarsjanski::getKolorANSI() const {
    return "\033[1;93m";
}