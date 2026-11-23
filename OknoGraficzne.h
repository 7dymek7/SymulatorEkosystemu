#ifndef OKNO_GRAFICZNE_H
#define OKNO_GRAFICZNE_H

#include "Srodowisko.h"

class OknoGraficzne {
private:
    Srodowisko& srodowisko;

public:
    explicit OknoGraficzne(Srodowisko& srod);
    void uruchom();
};

#endif