#include <iostream>
#include "Srodowisko.h"
#include "Organizm.h"
#include "Pozycja.h"

int main() {
    Srodowisko swiat;
    Pozycja poz(5, 5);
    Organizm o1('O', 100, poz);
    
    swiat.dodajOrganizm(o1);
    swiat.wyswietlStan();
    
    return 0;
}