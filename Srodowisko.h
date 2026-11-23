#ifndef SRODOWISKO_H
#define SRODOWISKO_H

#include <vector>
#include <memory>
#include <string>
#include "Organizm.h"

using namespace std;

class Srodowisko {
private:
    int szerokosc;
    int wysokosc;
    int sol;
    int temperatura;
    bool burzaPylowa;
    vector<unique_ptr<Organizm>> organizmy;
    vector<string> logi;

    void sprawdzWspolrzedne(int x, int y) const;

public:
    Srodowisko();
    Srodowisko(int szer, int wys);

    int getSzerokosc() const;
    int getWysokosc() const;
    int getSol() const;
    int getTemperatura() const;
    bool czyBurzaPylowa() const;
    int getLiczbaOrganizmow() const;
    const vector<unique_ptr<Organizm>>& getOrganizmy() const;
    const vector<string>& getLogi() const;
    const Organizm* znajdzLidera() const;

    void inicjalizujDomyslnyEkosystem();
    void resetuj();

    void dodajOrganizm(unique_ptr<Organizm> org);
    void usunOrganizm(int indeks);
    int znajdzOrganizm(int x, int y) const;
    bool modyfikujEnergie(int x, int y, int nowaEnergia);

    void wykonajTure();
    string pobierzSzczegoly(int indeks) const;
    void rysujPlansze(const string& komunikat = "") const;
    void wyswietlTabeleOrganizmow() const;
};

#endif