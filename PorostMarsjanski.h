#ifndef POROSTMARSJANSKI_H
#define POROSTMARSJANSKI_H

#include "Organizm.h"

class PorostMarsjanski : public Organizm {
private:
    int tempoWzrostu;

public:
    PorostMarsjanski();
    PorostMarsjanski(Pozycja startPozycja, int startEnergia = 10, int wzrost = 2);

    int getTempoWzrostu() const;

    void wykonajAkcje(int szerokosc, int wysokosc, bool burzaPylowa) override;
    std::string getTyp() const override;
    std::string getKolorANSI() const override;
};

#endif