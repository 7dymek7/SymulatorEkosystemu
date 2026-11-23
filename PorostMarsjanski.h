#ifndef POROST_MARSJANSKI_H
#define POROST_MARSJANSKI_H

#include "Organizm.h"

class PorostMarsjanski : public Organizm {
private:
    int tempoFotosyntezy;

public:
    PorostMarsjanski();
    PorostMarsjanski(int startEnergia, Pozycja startPozycja, int tempo = 2);

    int getTempoFotosyntezy() const;

    void wykonajAkcje(int szerokosc, int wysokosc, bool burzaPylowa) override;
    string getTyp() const override;
    string getKolorANSI() const override;
};

#endif