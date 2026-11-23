#ifndef DRAPIEZNIKMARSJANSKI_H
#define DRAPIEZNIKMARSJANSKI_H

#include "Organizm.h"

class DrapieznikMarsjanski : public Organizm {
private:
    int zasiegRuchu;

public:
    DrapieznikMarsjanski();
    DrapieznikMarsjanski(Pozycja startPozycja, int startEnergia = 16, int zasieg = 1);

    int getZasiegRuchu() const;

    void wykonajAkcje(int szerokosc, int wysokosc, bool burzaPylowa) override;
    std::string getTyp() const override;
    std::string getKolorANSI() const override;
};

#endif