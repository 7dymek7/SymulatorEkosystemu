#ifdef _WIN32
#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif

#include "OknoGraficzne.h"
#include "PorostMarsjanski.h"
#include "DrapieznikMarsjanski.h"
#include <iostream>
#include <sstream>
#include <algorithm>
#include <stdexcept>

using namespace std;

#ifdef _WIN32
namespace {
    const int MARGIN_X = 24;
    const int MARGIN_Y = 64;
    const int TILE = 56;
    const int PANEL_SZER = 300;
    const int PASEK_DOLNY = 44;
    const UINT_PTR TIMER_AUTO = 1;

    struct KontekstOkna {
        Srodowisko& srodowisko;
        bool autoSymulacja = false;
        string status = "LPM: dodaj Porost / skanuj | PPM: dodaj Drapieznika / usun | Spacja: Sol | A: Auto";
        RECT btnSol{};
        RECT btnAuto{};
        RECT btnReset{};
        int panelX = 0;
        int oknoSzer = 0;
        int oknoWys = 0;
    };

    HFONT stworzCzcionke(int rozmiar, int waga, const char* nazwa = "Segoe UI") {
        return CreateFontA(rozmiar, 0, 0, 0, waga, FALSE, FALSE, FALSE,
                           DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
                           CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_SWISS, nazwa);
    }

    void wypelnijProstokat(HDC hdc, int x1, int y1, int x2, int y2, COLORREF kolor) {
        HBRUSH pedzel = CreateSolidBrush(kolor);
        RECT r = { x1, y1, x2, y2 };
        FillRect(hdc, &r, pedzel);
        DeleteObject(pedzel);
    }

    void rysujRamke(HDC hdc, int x1, int y1, int x2, int y2, COLORREF kolor, int grubosc = 1) {
        HPEN pioro = CreatePen(PS_SOLID, grubosc, kolor);
        HPEN starePioro = (HPEN)SelectObject(hdc, pioro);
        HBRUSH staryPedzel = (HBRUSH)SelectObject(hdc, GetStockObject(NULL_BRUSH));
        Rectangle(hdc, x1, y1, x2, y2);
        SelectObject(hdc, staryPedzel);
        SelectObject(hdc, starePioro);
        DeleteObject(pioro);
    }

    void rysujPrzycisk(HDC hdc, const RECT& r, const string& tekst, COLORREF tlo, COLORREF ramka, HFONT czcionka) {
        wypelnijProstokat(hdc, r.left, r.top, r.right, r.bottom, tlo);
        rysujRamke(hdc, r.left, r.top, r.right, r.bottom, ramka, 2);

        HFONT stara = (HFONT)SelectObject(hdc, czcionka);
        SetTextColor(hdc, RGB(245, 245, 245));
        RECT tr = r;
        DrawTextA(hdc, tekst.c_str(), -1, &tr, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
        SelectObject(hdc, stara);
    }

    void rysujPorost(HDC hdc, int cx, int cy) {
        HPEN pioro = CreatePen(PS_SOLID, 1, RGB(25, 110, 45));
        HBRUSH ciemny = CreateSolidBrush(RGB(42, 168, 76));
        HBRUSH jasny = CreateSolidBrush(RGB(102, 226, 120));

        HPEN starePioro = (HPEN)SelectObject(hdc, pioro);
        HBRUSH staryPedzel = (HBRUSH)SelectObject(hdc, ciemny);

        Ellipse(hdc, cx - 14, cy - 10, cx + 4, cy + 8);
        Ellipse(hdc, cx - 2, cy - 14, cx + 15, cy + 4);
        Ellipse(hdc, cx - 9, cy - 3, cx + 10, cy + 12);

        SelectObject(hdc, jasny);
        Ellipse(hdc, cx - 6, cy - 8, cx + 6, cy + 4);

        SelectObject(hdc, staryPedzel);
        SelectObject(hdc, starePioro);
        DeleteObject(ciemny);
        DeleteObject(jasny);
        DeleteObject(pioro);
    }

    void rysujDrapieznika(HDC hdc, int cx, int cy) {
        POINT punkty[4] = {
            { cx, cy - 15 },
            { cx + 15, cy },
            { cx, cy + 13 },
            { cx - 15, cy }
        };

        HPEN pioro = CreatePen(PS_SOLID, 2, RGB(175, 92, 18));
        HBRUSH cialo = CreateSolidBrush(RGB(245, 198, 48));
        HBRUSH oczy = CreateSolidBrush(RGB(210, 35, 35));

        HPEN starePioro = (HPEN)SelectObject(hdc, pioro);
        HBRUSH staryPedzel = (HBRUSH)SelectObject(hdc, cialo);
        Polygon(hdc, punkty, 4);

        SelectObject(hdc, oczy);
        SelectObject(hdc, GetStockObject(NULL_PEN));
        Ellipse(hdc, cx - 7, cy - 5, cx - 2, cy);
        Ellipse(hdc, cx + 2, cy - 5, cx + 7, cy);

        SelectObject(hdc, staryPedzel);
        SelectObject(hdc, starePioro);
        DeleteObject(cialo);
        DeleteObject(oczy);
        DeleteObject(pioro);
    }

    void rysujPasekEnergii(HDC hdc, int tx, int ty, int energia, COLORREF kolor) {
        int bx1 = tx + 6;
        int by1 = ty + TILE - 10;
        int bx2 = tx + TILE - 6;
        int by2 = ty + TILE - 4;

        wypelnijProstokat(hdc, bx1, by1, bx2, by2, RGB(24, 14, 14));
        int maxSzer = bx2 - bx1;
        int wypelnienie = min(maxSzer, (energia * maxSzer) / 25);
        if (wypelnienie > 0) {
            wypelnijProstokat(hdc, bx1, by1, bx1 + wypelnienie, by2, kolor);
        }
    }

    void rysujScene(HDC hdc, const KontekstOkna& ctx) {
        const Srodowisko& srod = ctx.srodowisko;
        int szer = srod.getSzerokosc();
        int wys = srod.getWysokosc();
        bool burza = srod.czyBurzaPylowa();
        const Organizm* lider = srod.znajdzLidera();

        // Tlo calego okna
        wypelnijProstokat(hdc, 0, 0, ctx.oknoSzer, ctx.oknoWys, RGB(24, 16, 16));
        SetBkMode(hdc, TRANSPARENT);

        HFONT fTytul = stworzCzcionke(20, FW_BOLD);
        HFONT fNaglowek = stworzCzcionke(16, FW_BOLD);
        HFONT fTekst = stworzCzcionke(15, FW_NORMAL);
        HFONT fMaly = stworzCzcionke(13, FW_NORMAL);

        // 1. Gorny pasek tytulowy
        wypelnijProstokat(hdc, 0, 0, ctx.oknoSzer, 50, RGB(38, 20, 18));
        wypelnijProstokat(hdc, 0, 48, ctx.oknoSzer, 50, RGB(224, 112, 48));

        HFONT stara = (HFONT)SelectObject(hdc, fTytul);
        SetTextColor(hdc, RGB(255, 165, 85));
        TextOutA(hdc, MARGIN_X, 14, "SYMULATOR EKOSYSTEMU: SEKTOR KRATERU JEZERO (MARS)", 50);

        if (lider != nullptr) {
            ostringstream oss;
            oss << "Lider: " << *lider;
            string tekstLidera = oss.str();
            SelectObject(hdc, fMaly);
            SetTextColor(hdc, RGB(90, 225, 250));
            RECT rLider = { ctx.oknoSzer - 460, 16, ctx.oknoSzer - MARGIN_X, 38 };
            DrawTextA(hdc, tekstLidera.c_str(), -1, &rLider, DT_RIGHT | DT_VCENTER | DT_SINGLELINE | DT_END_ELLIPSIS);
        }

        // 2. Siatka mapy Marsa
        vector<vector<const Organizm*>> mapa(wys, vector<const Organizm*>(szer, nullptr));
        int liczbaPorostow = 0;
        int liczbaDrapieznikow = 0;
        for (const auto& org : srod.getOrganizmy()) {
            int ox = org->getPozycja().getX();
            int oy = org->getPozycja().getY();
            if (ox >= 0 && ox < szer && oy >= 0 && oy < wys) {
                mapa[oy][ox] = org.get();
            }
            if (org->getTyp() == "Porost Marsjanski") ++liczbaPorostow;
            else if (org->getTyp() == "Drapieznik Marsjanski") ++liczbaDrapieznikow;
        }

        HPEN pioroWiatru = CreatePen(PS_SOLID, 1, RGB(215, 145, 70));
        HPEN pioroSiatki = CreatePen(PS_SOLID, 1, RGB(58, 26, 18));

        for (int y = 0; y < wys; ++y) {
            for (int x = 0; x < szer; ++x) {
                int tx = MARGIN_X + x * TILE;
                int ty = MARGIN_Y + y * TILE;

                bool parzyste = ((x + y) % 2 == 0);
                COLORREF kolorPola;
                if (burza) {
                    kolorPola = parzyste ? RGB(132, 72, 28) : RGB(118, 62, 24);
                } else {
                    kolorPola = parzyste ? RGB(88, 38, 24) : RGB(78, 33, 21);
                }

                wypelnijProstokat(hdc, tx, ty, tx + TILE, ty + TILE, kolorPola);

                // Smugi wiatru podczas burzy pylowej
                if (burza && ((x + y + srod.getSol()) % 2 == 0)) {
                    HPEN stP = (HPEN)SelectObject(hdc, pioroWiatru);
                    MoveToEx(hdc, tx + 8, ty + 16, nullptr);
                    LineTo(hdc, tx + 28, ty + 12);
                    MoveToEx(hdc, tx + 24, ty + 34, nullptr);
                    LineTo(hdc, tx + 48, ty + 30);
                    SelectObject(hdc, stP);
                }

                // Delikatna krawedz kafelka
                HPEN stP = (HPEN)SelectObject(hdc, pioroSiatki);
                HBRUSH stB = (HBRUSH)SelectObject(hdc, GetStockObject(NULL_BRUSH));
                Rectangle(hdc, tx, ty, tx + TILE, ty + TILE);
                SelectObject(hdc, stB);
                SelectObject(hdc, stP);

                const Organizm* org = mapa[y][x];
                if (org != nullptr) {
                    int cx = tx + TILE / 2;
                    int cy = ty + (TILE - 10) / 2;

                    if (org == lider) {
                        rysujRamke(hdc, tx + 2, ty + 2, tx + TILE - 2, ty + TILE - 2, RGB(56, 217, 245), 2);
                    }

                    if (org->getTyp() == "Porost Marsjanski") {
                        rysujPorost(hdc, cx, cy);
                        rysujPasekEnergii(hdc, tx, ty, org->getEnergia(), RGB(95, 225, 115));
                    } else {
                        rysujDrapieznika(hdc, cx, cy);
                        rysujPasekEnergii(hdc, tx, ty, org->getEnergia(), RGB(245, 200, 50));
                    }
                }
            }
        }

        DeleteObject(pioroWiatru);
        DeleteObject(pioroSiatki);

        // Zewnetrzna ramka mapy
        rysujRamke(hdc, MARGIN_X - 2, MARGIN_Y - 2,
                   MARGIN_X + szer * TILE + 2, MARGIN_Y + wys * TILE + 2,
                   RGB(224, 112, 48), 2);

        // 3. Prawy panel: Telemetria
        int px = ctx.panelX;
        int py = MARGIN_Y;
        wypelnijProstokat(hdc, px, py, px + PANEL_SZER, py + 104, RGB(34, 22, 22));
        rysujRamke(hdc, px, py, px + PANEL_SZER, py + 104, RGB(120, 62, 38), 1);

        SelectObject(hdc, fNaglowek);
        SetTextColor(hdc, RGB(90, 225, 250));
        TextOutA(hdc, px + 12, py + 8, "TELEMETRIA SEKTORA", 18);

        SelectObject(hdc, fTekst);
        SetTextColor(hdc, RGB(235, 235, 235));
        string linia1 = "SOL: " + to_string(srod.getSol()) + "    |    TEMP: " + to_string(srod.getTemperatura()) + " C";
        TextOutA(hdc, px + 12, py + 32, linia1.c_str(), static_cast<int>(linia1.size()));

        SetTextColor(hdc, burza ? RGB(255, 95, 85) : RGB(105, 225, 125));
        string linia2 = burza ? "POGODA: ALERT - BURZA PYLOWA" : "POGODA: STABILNA";
        TextOutA(hdc, px + 12, py + 54, linia2.c_str(), static_cast<int>(linia2.size()));

        SetTextColor(hdc, RGB(105, 225, 125));
        string pTxt = "Porosty: " + to_string(liczbaPorostow);
        TextOutA(hdc, px + 12, py + 76, pTxt.c_str(), static_cast<int>(pTxt.size()));

        SetTextColor(hdc, RGB(245, 200, 50));
        string dTxt = "Drapiezniki: " + to_string(liczbaDrapieznikow);
        TextOutA(hdc, px + 145, py + 76, dTxt.c_str(), static_cast<int>(dTxt.size()));

        // 4. Przyciski sterujace
        rysujPrzycisk(hdc, ctx.btnSol, "NASTEPNY SOL  [Spacja]",
                      RGB(165, 72, 28), RGB(235, 130, 55), fNaglowek);
        rysujPrzycisk(hdc, ctx.btnAuto, ctx.autoSymulacja ? "AUTO: WL [A]" : "AUTO: WYL [A]",
                      ctx.autoSymulacja ? RGB(35, 125, 65) : RGB(52, 42, 46),
                      ctx.autoSymulacja ? RGB(95, 225, 125) : RGB(130, 105, 105), fNaglowek);
        rysujPrzycisk(hdc, ctx.btnReset, "RESET [R]",
                      RGB(120, 38, 38), RGB(215, 85, 85), fNaglowek);

        // 5. Legenda i instrukcja myszy
        int ly = ctx.btnAuto.bottom + 14;
        wypelnijProstokat(hdc, px, ly, px + PANEL_SZER, ly + 108, RGB(34, 22, 22));
        rysujRamke(hdc, px, ly, px + PANEL_SZER, ly + 108, RGB(120, 62, 38), 1);

        SelectObject(hdc, fNaglowek);
        SetTextColor(hdc, RGB(90, 225, 250));
        TextOutA(hdc, px + 12, ly + 8, "LEGENDA I MYSZ", 14);

        SelectObject(hdc, fMaly);
        SetTextColor(hdc, RGB(105, 225, 125));
        TextOutA(hdc, px + 12, ly + 30, "[*] Porost Marsjanski (fotosynteza)", 35);
        SetTextColor(hdc, RGB(245, 200, 50));
        TextOutA(hdc, px + 12, ly + 48, "[X] Drapieznik (poluje na porost)", 33);
        SetTextColor(hdc, RGB(90, 225, 250));
        TextOutA(hdc, px + 12, ly + 66, "Ramka cyan = Lider energii (<)", 30);
        SetTextColor(hdc, RGB(205, 190, 185));
        TextOutA(hdc, px + 12, ly + 86, "LPM: Porost/Skan | PPM: Drapieznik/Usun", 39);

        // 6. Dziennik zdarzen
        int logY = ly + 120;
        int logBottom = MARGIN_Y + wys * TILE;
        wypelnijProstokat(hdc, px, logY, px + PANEL_SZER, logBottom, RGB(34, 22, 22));
        rysujRamke(hdc, px, logY, px + PANEL_SZER, logBottom, RGB(120, 62, 38), 1);

        SelectObject(hdc, fNaglowek);
        SetTextColor(hdc, RGB(90, 225, 250));
        TextOutA(hdc, px + 12, logY + 8, "DZIENNIK ZDARZEN", 16);

        SelectObject(hdc, fMaly);
        SetTextColor(hdc, RGB(220, 215, 210));
        const auto& logi = srod.getLogi();
        if (logi.empty()) {
            TextOutA(hdc, px + 12, logY + 34, "Brak zdarzen w biezacym Solu.", 29);
        } else {
            int maxLinii = min(static_cast<int>(logi.size()), 4);
            for (int i = 0; i < maxLinii; ++i) {
                string wpis = "> " + logi[i];
                RECT rLog = { px + 12, logY + 32 + i * 20, px + PANEL_SZER - 10, logY + 52 + i * 20 };
                DrawTextA(hdc, wpis.c_str(), -1, &rLog, DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_END_ELLIPSIS);
            }
        }

        // 7. Dolny pasek statusu
        int sy = MARGIN_Y + wys * TILE + 12;
        wypelnijProstokat(hdc, MARGIN_X, sy, ctx.oknoSzer - MARGIN_X, sy + 32, RGB(38, 22, 22));
        rysujRamke(hdc, MARGIN_X, sy, ctx.oknoSzer - MARGIN_X, sy + 32, RGB(145, 72, 42), 1);

        SelectObject(hdc, fMaly);
        SetTextColor(hdc, RGB(255, 200, 140));
        string pelnyStatus = "[STATUS] " + ctx.status;
        RECT rStat = { MARGIN_X + 10, sy, ctx.oknoSzer - MARGIN_X - 10, sy + 32 };
        DrawTextA(hdc, pelnyStatus.c_str(), -1, &rStat, DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_END_ELLIPSIS);

        SelectObject(hdc, stara);
        DeleteObject(fTytul);
        DeleteObject(fNaglowek);
        DeleteObject(fTekst);
        DeleteObject(fMaly);
    }

    bool punktWProstokacie(int x, int y, const RECT& r) {
        return x >= r.left && x <= r.right && y >= r.top && y <= r.bottom;
    }

    void przelaczAuto(HWND hwnd, KontekstOkna& ctx) {
        ctx.autoSymulacja = !ctx.autoSymulacja;
        if (ctx.autoSymulacja) {
            SetTimer(hwnd, TIMER_AUTO, 500, nullptr);
            ctx.status = "Uruchomiono automatyczna symulacje (1 Sol / 0.5s).";
        } else {
            KillTimer(hwnd, TIMER_AUTO);
            ctx.status = "Zatrzymano automatyczna symulacje.";
        }
        InvalidateRect(hwnd, nullptr, FALSE);
    }

    LRESULT CALLBACK proceduraOkna(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
        if (msg == WM_NCCREATE) {
            CREATESTRUCTA* cs = reinterpret_cast<CREATESTRUCTA*>(lParam);
            SetWindowLongPtrA(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(cs->lpCreateParams));
            return TRUE;
        }

        KontekstOkna* ctx = reinterpret_cast<KontekstOkna*>(GetWindowLongPtrA(hwnd, GWLP_USERDATA));
        if (!ctx) {
            return DefWindowProcA(hwnd, msg, wParam, lParam);
        }

        switch (msg) {
            case WM_ERASEBKGND:
                return 1;

            case WM_PAINT: {
                PAINTSTRUCT ps;
                HDC hdc = BeginPaint(hwnd, &ps);
                RECT rc;
                GetClientRect(hwnd, &rc);

                HDC memDC = CreateCompatibleDC(hdc);
                HBITMAP memBmp = CreateCompatibleBitmap(hdc, rc.right, rc.bottom);
                HBITMAP staraBmp = (HBITMAP)SelectObject(memDC, memBmp);

                rysujScene(memDC, *ctx);

                BitBlt(hdc, 0, 0, rc.right, rc.bottom, memDC, 0, 0, SRCCOPY);
                SelectObject(memDC, staraBmp);
                DeleteObject(memBmp);
                DeleteDC(memDC);

                EndPaint(hwnd, &ps);
                return 0;
            }

            case WM_TIMER:
                if (wParam == TIMER_AUTO) {
                    ctx->srodowisko.wykonajTure();
                    InvalidateRect(hwnd, nullptr, FALSE);
                }
                return 0;

            case WM_KEYDOWN:
                if (wParam == VK_SPACE) {
                    ctx->srodowisko.wykonajTure();
                    ctx->status = "Wykonano Sol " + to_string(ctx->srodowisko.getSol()) + ".";
                    InvalidateRect(hwnd, nullptr, FALSE);
                } else if (wParam == 'A') {
                    przelaczAuto(hwnd, *ctx);
                } else if (wParam == 'R') {
                    ctx->srodowisko.resetuj();
                    ctx->status = "Zresetowano symulacje do Sol 1.";
                    InvalidateRect(hwnd, nullptr, FALSE);
                } else if (wParam == VK_ESCAPE) {
                    DestroyWindow(hwnd);
                }
                return 0;

            case WM_LBUTTONDOWN: {
                int mx = LOWORD(lParam);
                int my = HIWORD(lParam);

                try {
                    if (punktWProstokacie(mx, my, ctx->btnSol)) {
                        ctx->srodowisko.wykonajTure();
                        ctx->status = "Wykonano Sol " + to_string(ctx->srodowisko.getSol()) + ".";
                    } else if (punktWProstokacie(mx, my, ctx->btnAuto)) {
                        przelaczAuto(hwnd, *ctx);
                        return 0;
                    } else if (punktWProstokacie(mx, my, ctx->btnReset)) {
                        ctx->srodowisko.resetuj();
                        ctx->status = "Zresetowano symulacje do Sol 1.";
                    } else {
                        int szerMapy = ctx->srodowisko.getSzerokosc() * TILE;
                        int wysMapy = ctx->srodowisko.getWysokosc() * TILE;
                        if (mx >= MARGIN_X && mx < MARGIN_X + szerMapy &&
                            my >= MARGIN_Y && my < MARGIN_Y + wysMapy) {
                            int gx = (mx - MARGIN_X) / TILE;
                            int gy = (my - MARGIN_Y) / TILE;
                            int idx = ctx->srodowisko.znajdzOrganizm(gx, gy);
                            if (idx == -1) {
                                ctx->srodowisko.dodajOrganizm(make_unique<PorostMarsjanski>(10, Pozycja(gx, gy), 2));
                                ctx->status = "Dodano Porost Marsjanski na polu (" + to_string(gx) + ", " + to_string(gy) + ").";
                            } else {
                                ctx->status = ctx->srodowisko.pobierzSzczegoly(idx);
                            }
                        }
                    }
                } catch (const exception& e) {
                    ctx->status = string("WYJATEK: ") + e.what();
                }

                InvalidateRect(hwnd, nullptr, FALSE);
                return 0;
            }

            case WM_RBUTTONDOWN: {
                int mx = LOWORD(lParam);
                int my = HIWORD(lParam);
                int szerMapy = ctx->srodowisko.getSzerokosc() * TILE;
                int wysMapy = ctx->srodowisko.getWysokosc() * TILE;

                if (mx >= MARGIN_X && mx < MARGIN_X + szerMapy &&
                    my >= MARGIN_Y && my < MARGIN_Y + wysMapy) {
                    try {
                        int gx = (mx - MARGIN_X) / TILE;
                        int gy = (my - MARGIN_Y) / TILE;
                        int idx = ctx->srodowisko.znajdzOrganizm(gx, gy);
                        if (idx == -1) {
                            ctx->srodowisko.dodajOrganizm(make_unique<DrapieznikMarsjanski>(18, Pozycja(gx, gy), 1));
                            ctx->status = "Dodano Drapieznika Marsjanskiego na polu (" + to_string(gx) + ", " + to_string(gy) + ").";
                        } else {
                            ctx->srodowisko.usunOrganizm(idx);
                            ctx->status = "Usunieto organizm z pola (" + to_string(gx) + ", " + to_string(gy) + ").";
                        }
                    } catch (const exception& e) {
                        ctx->status = string("WYJATEK: ") + e.what();
                    }
                    InvalidateRect(hwnd, nullptr, FALSE);
                }
                return 0;
            }

            case WM_DESTROY:
                if (ctx->autoSymulacja) {
                    KillTimer(hwnd, TIMER_AUTO);
                    ctx->autoSymulacja = false;
                }
                PostQuitMessage(0);
                return 0;
        }

        return DefWindowProcA(hwnd, msg, wParam, lParam);
    }
}
#endif

OknoGraficzne::OknoGraficzne(Srodowisko& srod) : srodowisko(srod) {}

void OknoGraficzne::uruchom() {
#ifdef _WIN32
    HINSTANCE hInst = GetModuleHandleA(nullptr);
    const char* KLASA_OKNA = "SymulatorEkosystemuMars2D";

    WNDCLASSEXA wc{};
    wc.cbSize = sizeof(WNDCLASSEXA);
    wc.lpfnWndProc = proceduraOkna;
    wc.hInstance = hInst;
    wc.hCursor = LoadCursorA(nullptr, IDC_ARROW);
    wc.lpszClassName = KLASA_OKNA;

    RegisterClassExA(&wc);

    KontekstOkna ctx{ srodowisko };
    int szerMapy = srodowisko.getSzerokosc() * TILE;
    int wysMapy = srodowisko.getWysokosc() * TILE;

    ctx.panelX = MARGIN_X + szerMapy + 24;
    ctx.oknoSzer = ctx.panelX + PANEL_SZER + MARGIN_X;
    ctx.oknoWys = MARGIN_Y + wysMapy + PASEK_DOLNY + 14;

    ctx.btnSol   = { ctx.panelX, MARGIN_Y + 116, ctx.panelX + PANEL_SZER, MARGIN_Y + 150 };
    ctx.btnAuto  = { ctx.panelX, MARGIN_Y + 158, ctx.panelX + 145, MARGIN_Y + 192 };
    ctx.btnReset = { ctx.panelX + 155, MARGIN_Y + 158, ctx.panelX + PANEL_SZER, MARGIN_Y + 192 };

    DWORD styl = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX;
    RECT rc = { 0, 0, ctx.oknoSzer, ctx.oknoWys };
    AdjustWindowRect(&rc, styl, FALSE);

    HWND hwnd = CreateWindowExA(
        0,
        KLASA_OKNA,
        "Symulator Ekosystemu Marsjanskiego - Widok 2D",
        styl,
        CW_USEDEFAULT, CW_USEDEFAULT,
        rc.right - rc.left, rc.bottom - rc.top,
        nullptr, nullptr, hInst, &ctx
    );

    if (!hwnd) {
        UnregisterClassA(KLASA_OKNA, hInst);
        throw runtime_error("Nie udalo sie utworzyc natywnego okna 2D!");
    }

    ShowWindow(hwnd, SW_SHOW);
    UpdateWindow(hwnd);

    MSG msg{};
    while (GetMessageA(&msg, nullptr, 0, 0) > 0) {
        TranslateMessage(&msg);
        DispatchMessageA(&msg);
    }

    UnregisterClassA(KLASA_OKNA, hInst);
#else
    cout << "Natywne okno 2D Win32 GDI jest dostepne tylko w systemie Windows.\n";
#endif
}