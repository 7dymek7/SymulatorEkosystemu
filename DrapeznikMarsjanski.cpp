#include "DrapieznikMarsjanski.h"
#include <cstdlib>

DrapieznikMarsjanski::DrapieznikMarsjanski()
    : Organizm('D', 16, Pozycja(0, 0)), zasiegRuchu(1) {}

DrapieznikMarsjanski::DrapieznikMarsjanski(Pozycja startPozycja, int startEnergia, int zasieg)
    : Organizm('D', startEnergia, startPozycja), zasiegRuchu(zasieg) {}

int DrapieznikMarsjanski::getZasiegRuchu() const {
    return zasiegRuchu;
}

void DrapieznikMarsjanski::wykonajAkcje(int szerokosc, int wysokosc, bool burzaPylowa) {
    int zakres = 2 * zasiegRuchu + 1;
    int dx = (std::rand() % zakres) - zasiegRuchu;
    int dy = (std::rand() % zakres) - zasiegRuchu;

    int noweX = getPozycja().getX() + dx;
    int noweY = getPozycja().getY() + dy;

    if (noweX < 0) noweX = 0;
    if (noweX >= szerokosc) noweX = szerokosc - 1;
    if (noweY < 0) noweY = 0;
    if (noweY >= wysokosc) noweY = wysokosc - 1;

    setPozycja(Pozycja(noweX, noweY));

    // Burza pylowa mocniej wyczerpuje drapieznika
    zmienEnergie(burzaPylowa ? -2 : -1);
}

std::string DrapieznikMarsjanski::getTyp() const {
    return "Drapieznik Marsjanski";
}

std::string DrapieznikMarsjanski::getKolorANSI() const {
    return "\033[1;33m"; // Zloty / zolty
}