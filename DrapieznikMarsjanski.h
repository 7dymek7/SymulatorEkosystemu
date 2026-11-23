#ifndef DRAPIEZNIK_MARSJANSKI_H
#define DRAPIEZNIK_MARSJANSKI_H

#include "Organizm.h"

class DrapieznikMarsjanski : public Organizm {
private:
    int zasiegRuchu;

public:
    DrapieznikMarsjanski();
    DrapieznikMarsjanski(int startEnergia, Pozycja startPozycja, int zasieg = 1);

    int getZasiegRuchu() const;

    void wykonajAkcje(int szerokosc, int wysokosc, bool burzaPylowa) override;
    string getTyp() const override;
    string getKolorANSI() const override;
};

#endif