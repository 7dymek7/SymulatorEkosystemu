#include "Srodowisko.h"
#include "PorostMarsjanski.h"
#include <iostream>
#include <cstdlib>

Srodowisko::Srodowisko()
    : szerokosc(16), wysokosc(10), numerSola(1), burzaPylowa(false),
      ostatniKomunikat("Ladowanie w kraterze Jezero zakonczone.") {}

Srodowisko::Srodowisko(int szer, int wys)
    : szerokosc(szer), wysokosc(wys), numerSola(1), burzaPylowa(false),
      ostatniKomunikat("Ladowanie w kraterze Jezero zakonczone.") {}

void Srodowisko::dodajOrganizm(std::unique_ptr<Organizm> org) {
    int x = org->getPozycja().getX();
    int y = org->getPozycja().getY();

    if (x >= 0 && x < szerokosc && y >= 0 && y < wysokosc) {
        ostatniKomunikat = "Dodano: " + org->getTyp() + " na polu (" +
                           std::to_string(x) + ", " + std::to_string(y) + ").";
        organizmy.push_back(std::move(org));
    } else {
        ostatniKomunikat = "Blad: wspolrzedne poza sektorem Marsa!";
    }
}

void Srodowisko::usunOrganizm(int indeks) {
    if (indeks >= 0 && indeks < static_cast<int>(organizmy.size())) {
        organizmy.erase(organizmy.begin() + indeks);
    }
}

int Srodowisko::znajdzOrganizm(int x, int y) const {
    for (int i = 0; i < static_cast<int>(organizmy.size()); ++i) {
        if (organizmy[i]->getPozycja().getX() == x &&
            organizmy[i]->getPozycja().getY() == y) {
            return i;
        }
    }
    return -1;
}

bool Srodowisko::modyfikujEnergie(int x, int y, int nowaEnergia) {
    int indeks = znajdzOrganizm(x, y);
    if (indeks == -1) {
        return false;
    }
    organizmy[indeks]->setEnergia(nowaEnergia);
    ostatniKomunikat = "Zmieniono energie organizmu (" + std::to_string(x) +
                       ", " + std::to_string(y) + ") na " + std::to_string(nowaEnergia) + ".";
    return true;
}

int Srodowisko::policzTyp(const std::string& typ) const {
    int licznik = 0;
    for (const auto& org : organizmy) {
        if (org->getTyp() == typ) {
            licznik++;
        }
    }
    return licznik;
}

int Srodowisko::getNumerSola() const { return numerSola; }
bool Srodowisko::czyBurzaPylowa() const { return burzaPylowa; }

void Srodowisko::wykonajTure() {
    numerSola++;
    burzaPylowa = (std::rand() % 4 == 0); // 25% szans na burze pylowa

    if (burzaPylowa) {
        ostatniKomunikat = "ALARM! Burza pylowa na Marsie oslabia ekosystem!";
    } else {
        ostatniKomunikat = "Sloneczny Sol. Porosty fotosyntetyzuja w regolicie.";
    }

    // 1. Polimorficzne wykonanie akcji kazdego organizmu
    for (auto& org : organizmy) {
        org->wykonajAkcje(szerokosc, wysokosc, burzaPylowa);
    }

    // 2. Interakcje: drapieznik zjada porost na tym samym polu
    for (size_t i = 0; i < organizmy.size(); ++i) {
        if (!organizmy[i]->czyZywy() || organizmy[i]->getTyp() != "Drapieznik Marsjanski") {
            continue;
        }
        for (size_t j = 0; j < organizmy.size(); ++j) {
            if (i == j || !organizmy[j]->czyZywy() || organizmy[j]->getTyp() != "Porost Marsjanski") {
                continue;
            }
            if (organizmy[i]->getPozycja().getX() == organizmy[j]->getPozycja().getX() &&
                organizmy[i]->getPozycja().getY() == organizmy[j]->getPozycja().getY()) {
                organizmy[i]->zmienEnergie(8);
                organizmy[j]->setEnergia(0);
                ostatniKomunikat = "Drapieznik pochlonął Porost na polu (" +
                                   std::to_string(organizmy[i]->getPozycja().getX()) + ", " +
                                   std::to_string(organizmy[i]->getPozycja().getY()) + ")!";
                break;
            }
        }
    }

    // 3. Rozrastanie sie silnych porostow (gdy nie ma burzy)
    std::vector<std::unique_ptr<Organizm>> noweOrganizmy;
    if (!burzaPylowa && organizmy.size() < 35) {
        for (auto& org : organizmy) {
            if (org->czyZywy() && org->getTyp() == "Porost Marsjanski" && org->getEnergia() >= 18) {
                int nx = org->getPozycja().getX() + ((std::rand() % 3) - 1);
                int ny = org->getPozycja().getY() + ((std::rand() % 3) - 1);

                if (nx >= 0 && nx < szerokosc && ny >= 0 && ny < wysokosc && znajdzOrganizm(nx, ny) == -1) {
                    org->zmienEnergie(-8);
                    noweOrganizmy.push_back(std::make_unique<PorostMarsjanski>(Pozycja(nx, ny), 8, 2));
                    ostatniKomunikat = "Porost marsjanski rozrosl sie na sasiedni sektor!";
                }
            }
        }
    }

    for (auto& nowy : noweOrganizmy) {
        organizmy.push_back(std::move(nowy));
    }

    // 4. Usuwanie martwych organizmow
    for (int i = static_cast<int>(organizmy.size()) - 1; i >= 0; --i) {
        if (!organizmy[i]->czyZywy()) {
            usunOrganizm(i);
        }
    }
}

void Srodowisko::rysujPlansze() const {
    const std::string RESET = "\033[0m";
    const std::string MARS_RAMKA = "\033[1;31m";
    const std::string MARS_PIASEK = burzaPylowa ? "\033[33m" : "\033[31m";
    const std::string CYAN = "\033[1;36m";
    const std::string ZIELONY = "\033[1;32m";
    const std::string ZOLTY = "\033[1;33m";

    char tloZnak = burzaPylowa ? '~' : '.';
    std::vector<std::vector<char>> siatka(wysokosc, std::vector<char>(szerokosc, tloZnak));
    std::vector<std::vector<std::string>> kolory(wysokosc, std::vector<std::string>(szerokosc, MARS_PIASEK));

    // Najpierw porosty, potem drapiezniki (zeby drapiezniki byly zawsze widoczne na wierzchu)
    for (const auto& org : organizmy) {
        int x = org->getPozycja().getX();
        int y = org->getPozycja().getY();
        siatka[y][x] = org->getSymbol();
        kolory[y][x] = org->getKolorANSI();
    }

    std::cout << "\033[2J\033[H"; // Wyczysc ekran i ustaw kursor na gorze

    // Naglowek HUD
    std::cout << MARS_RAMKA << "╔══════════════════════════════════════════════════════╗\n";
    std::cout << "║" << CYAN << "       SYMULATOR EKOSYSTEMU MARSA - KRATER JEZERO     " << MARS_RAMKA << "║\n";
    std::cout << "╚══════════════════════════════════════════════════════╝" << RESET << "\n";

    std::cout << "  SOL: " << CYAN << numerSola << RESET
              << "  |  POGODA: "
              << (burzaPylowa ? "\033[1;31mBURZA PYLOWA (-E)" : "\033[1;32mCZYSTE NIEBO (+E)")
              << RESET << "\n";

    std::cout << "  " << ZIELONY << "[P] Porosty: " << policzTyp("Porost Marsjanski") << RESET
              << "   |   "
              << ZOLTY << "[D] Drapiezniki: " << policzTyp("Drapieznik Marsjanski") << RESET
              << "   |   Razem: " << organizmy.size() << "\n\n";

    // Rysowanie mapy Marsa z ramka
    std::cout << "  " << MARS_RAMKA << "╔";
    for (int i = 0; i < szerokosc * 2 + 1; ++i) std::cout << "═";
    std::cout << "╗" << RESET << "\n";

    for (int y = 0; y < wysokosc; ++y) {
        std::cout << "  " << MARS_RAMKA << "║ " << RESET;
        for (int x = 0; x < szerokosc; ++x) {
            std::cout << kolory[y][x] << siatka[y][x] << " " << RESET;
        }
        std::cout << MARS_RAMKA << "║" << RESET << " " << y << "\n";
    }

    std::cout << "  " << MARS_RAMKA << "╚";
    for (int i = 0; i < szerokosc * 2 + 1; ++i) std::cout << "═";
    std::cout << "╝" << RESET << "\n";

    // Pasek statusu wydarzen
    std::cout << "\n  " << CYAN << "[LOG]: " << RESET << ostatniKomunikat << "\n";
}

std::string Srodowisko::pobierzSzczegoly(int indeks) const {
    if (indeks >= 0 && indeks < static_cast<int>(organizmy.size())) {
        const auto& org = organizmy[indeks];
        return "Znaleziono: " + org->getTyp() + " [" + org->getSymbol() +
               "] | Energia: " + std::to_string(org->getEnergia()) +
               " | Pozycja: (" + std::to_string(org->getPozycja().getX()) +
               ", " + std::to_string(org->getPozycja().getY()) + ")";
    }
    return "Brak organizmu na podanych wspolrzednych.";
}