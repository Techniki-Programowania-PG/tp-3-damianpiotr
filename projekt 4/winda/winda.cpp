/***********************************

Techniki programowania - projekt 4 - zad 3 (winda)

Skład grupy:
Damian Rutkowski 203304
Aleksander Stachurski 207394
ACiR 4A

***********************************/

#include <iostream>
#include <windows.h>
#include <gdiplus.h>
#include <queue>
#include <ctime>
#include <chrono>
#pragma comment(lib, "gdiplus.lib")

using namespace Gdiplus;
using namespace std;

/***
struktury
**/

struct czlowiek
{
    int start_pietro = 0;
    int koniec_pietro = 0;
    int wymiar_x = 0;
    int wymiar_y = 0;
    int gdzie_jedzie = 0;
    bool czy_w_kolejce_g = false;
    bool czy_w_kolejce_d = false;
    bool czy_w_windzie = false;
    bool czy_do_usuniecia = false;
    int waga = 70;
    bool czy_wsiada = false;
    wstring plec;
};

struct Winda
{
    vector<int> pietra_do_powrotu;
    int pozycja_y = 460;
    bool czy_jedzie = false;
    bool czy_pauzuje = false;
    int pauza = 0;
    int cel_windy = 460;
    vector<int> kolejkaPieterG;
    vector<int> kolejkaPieterD;
    int ile_osob = 0;
    int ile_wsiada = 0;
    int kierunek = 0;
    int laczna_waga = 0;
    const int maks_waga = 600;
    chrono::steady_clock::time_point czas_bezczynnosci_start;
    bool licz_bezczynnosc = false;
};

struct DaneProgramu 
{
    vector<czlowiek> ludzie;
    Winda winda;
};


/***

funkcja do WM_CREATE

***/

// Funkcja do tworzenia przycisków
void StworzPrzyciski(HWND uchwytOkna, HINSTANCE instancja) 
{
    wchar_t tytul[32];
    int x, y;

    for (int a = 0; a <= 4; a++) 
    {
        for (int i = 0; i <= 4; i++) 
        {
            swprintf_s(tytul, L"%d", 4 - i);

            if (a % 2 == 0) 
            {
                x = 5;
            }
            else {
                x = 665;
            }

            if (a > i) 
            {
                y = 5 + 30 * (i + 1) + 100 * a;
            }
            else {
                y = 5 + 30 * i + 100 * a;
            }

            if (a != i) 
            {
                CreateWindow(
                    TEXT("BUTTON"),
                    tytul,
                    WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
                    x, y,
                    25, 25,
                    uchwytOkna,
                    (HMENU)((40 - 10 * a) + (4 - i)),
                    instancja,
                    NULL);
            }
        }
    }
}

/***

funkcje do WM_COMMAND

***/

//tworzenie nowego ludzika
void DodajCzlowieka(DaneProgramu* dane, int start_pietro, int koniec_pietro)
{
    czlowiek nowyCzlowiek;
    nowyCzlowiek.start_pietro = start_pietro;
    nowyCzlowiek.koniec_pietro = koniec_pietro;

    int licz = 0;
    for (size_t i = 0; i < dane->ludzie.size(); ++i) 
    {
        if (dane->ludzie[i].start_pietro == start_pietro && !dane->ludzie[i].czy_w_windzie) 
        {
            licz++;
        }
    }

    if (start_pietro == 1 || start_pietro == 3) 
    {
        nowyCzlowiek.wymiar_x = 460 + 20 * licz;
    }
    else 
    {
        nowyCzlowiek.wymiar_x = 220 - 20 * licz;
    }

    if (start_pietro < koniec_pietro) 
    {
        nowyCzlowiek.gdzie_jedzie = 1;
    }
    else 
    {
        nowyCzlowiek.gdzie_jedzie = 2;
    }
    nowyCzlowiek.wymiar_y = 560 - 100 * start_pietro - 52;
    if (rand() % 2 == 0) 
    {
        nowyCzlowiek.plec = L"man.png";
    }
    else 
    {
        nowyCzlowiek.plec = L"woman.png";
    }

    dane->ludzie.push_back(nowyCzlowiek);
}

//aktualizacja kolejki windy - jazda w gore
void AktualizujKolejkeGora(Winda& winda, int start_pietro, int koniec_pietro)
{
    if (find(winda.kolejkaPieterG.begin(), winda.kolejkaPieterG.end(), start_pietro) == winda.kolejkaPieterG.end()) 
    {
        winda.kolejkaPieterG.push_back(start_pietro);
        sort(winda.kolejkaPieterG.begin(), winda.kolejkaPieterG.end());
    }
    if (find(winda.kolejkaPieterG.begin(), winda.kolejkaPieterG.end(), koniec_pietro) == winda.kolejkaPieterG.end()) 
    {
        winda.kolejkaPieterG.push_back(koniec_pietro);
        sort(winda.kolejkaPieterG.begin(), winda.kolejkaPieterG.end());
    }
}

//aktualizacja kolejki windy - jazda w dol
void AktualizujKolejkeDol(Winda& winda, int start_pietro, int koniec_pietro)
{
    if (find(winda.kolejkaPieterD.begin(), winda.kolejkaPieterD.end(), start_pietro) == winda.kolejkaPieterD.end()) 
    {
        winda.kolejkaPieterD.push_back(start_pietro);
        sort(winda.kolejkaPieterD.begin(), winda.kolejkaPieterD.end(), greater<int>());
    }
    if (find(winda.kolejkaPieterD.begin(), winda.kolejkaPieterD.end(), koniec_pietro) == winda.kolejkaPieterD.end()) {
        winda.kolejkaPieterD.push_back(koniec_pietro);
        sort(winda.kolejkaPieterD.begin(), winda.kolejkaPieterD.end(), greater<int>());
    }
}

/***

funkcje do WM_TIMER

***/

//wsiadanie do windy
void WsiadanieDoWindy(DaneProgramu* dane, HWND uchwytOkna) 
{
    int dodaj = 0;
    Winda& winda = dane->winda;

    for (size_t i = 0; i < dane->ludzie.size(); i++) 
    {
        auto& osoba = dane->ludzie[i];
        if (abs(dane->winda.pozycja_y + 52 - osoba.wymiar_y) <= 5 &&
            !dane->winda.czy_jedzie &&
            !osoba.czy_w_windzie) 
        {

            bool zgodny_kierunek = (dane->winda.kierunek == 1 && osoba.gdzie_jedzie == 1) ||
                (dane->winda.kierunek == -1 && osoba.gdzie_jedzie == 2);

            if (zgodny_kierunek && dane->winda.laczna_waga + osoba.waga <= dane->winda.maks_waga) 
            {
                dane->winda.ile_osob++;
                osoba.czy_w_windzie = true;
                osoba.wymiar_x = 250 + dane->winda.ile_osob * 20;
                dane->winda.ile_wsiada++;
                dane->winda.laczna_waga += osoba.waga;

                if (dane->winda.kierunek == 1) 
                {
                    dane->winda.kolejkaPieterG.push_back(osoba.koniec_pietro);
                    sort(dane->winda.kolejkaPieterG.begin(), dane->winda.kolejkaPieterG.end());
                }
                else if (dane->winda.kierunek == -1) 
                {
                    dane->winda.kolejkaPieterD.push_back(osoba.koniec_pietro);
                    sort(dane->winda.kolejkaPieterD.begin(), dane->winda.kolejkaPieterD.end(), greater<int>());
                }

                //przestawianie ludzików podczas wsiadania
                for (size_t a = 0; a < dane->ludzie.size(); a++) 
                {
                    if (!dane->ludzie[a].czy_w_windzie &&
                        dane->ludzie[a].start_pietro == osoba.start_pietro &&
                        abs(dane->winda.pozycja_y + 52 - dane->ludzie[a].wymiar_y) <= 5 &&
                        !dane->ludzie[a].czy_wsiada) {

                        int liczba_osob = 0;
                        for (size_t b = 0; b < dane->ludzie.size(); b++) 
                        {
                            if (!dane->ludzie[b].czy_w_windzie &&
                                dane->ludzie[b].start_pietro == osoba.start_pietro &&
                                abs(dane->winda.pozycja_y + 52 - dane->ludzie[b].wymiar_y) <= 5) {
                                liczba_osob++;
                            }
                        }

                        int pozycja_w_kolejce = 0;
                        for (size_t b = 0; b < a; b++) 
                        {
                            if (!dane->ludzie[b].czy_w_windzie &&
                                dane->ludzie[b].start_pietro == osoba.start_pietro &&
                                abs(dane->winda.pozycja_y + 52 - dane->ludzie[b].wymiar_y) <= 5) {
                                pozycja_w_kolejce++;
                            }
                        }

                        if ((dane->ludzie[a].start_pietro == 1 || dane->ludzie[a].start_pietro == 3)) 
                        {
                            int startX = 460;
                            int odstep = 20;
                            dane->ludzie[a].wymiar_x = startX + (liczba_osob - pozycja_w_kolejce - 1) * odstep;
                        }
                        else if ((dane->ludzie[a].start_pietro == 0 || dane->ludzie[a].start_pietro == 2 || dane->ludzie[a].start_pietro == 4)) 
                        {
                            int startX = 220;
                            int odstep = 20;
                            dane->ludzie[a].wymiar_x = startX - pozycja_w_kolejce * odstep;
                        }
                    }
                }

            }
            else if (zgodny_kierunek && dane->winda.laczna_waga + osoba.waga > dane->winda.maks_waga && dodaj == 0) 
            {
                dodaj = 1;
                if (osoba.gdzie_jedzie == 1) 
                {
                    dane->winda.kolejkaPieterG.push_back(osoba.start_pietro);
                    dane->winda.kolejkaPieterG.push_back(osoba.koniec_pietro);
                }
                else if (osoba.gdzie_jedzie == 2) 
                {
                    dane->winda.kolejkaPieterD.push_back(osoba.start_pietro);
                    dane->winda.kolejkaPieterD.push_back(osoba.koniec_pietro);
                }
            }
            if (zgodny_kierunek && dane->winda.laczna_waga + osoba.waga > dane->winda.maks_waga) 
            {
                //powrot do pietra gdzie zostali ludzi, poniewaz sie nie zmiescili -> dodawanie do powrotu windy
                if (find(dane->winda.pietra_do_powrotu.begin(), dane->winda.pietra_do_powrotu.end(), osoba.start_pietro) == dane->winda.pietra_do_powrotu.end()) 
                {
                    dane->winda.pietra_do_powrotu.push_back(osoba.start_pietro);
                }
            }
        }


    }

}

//wysiadnie z windy i usuwanie osób
void WysiadanieZWindy(DaneProgramu* dane) 
{
    for (size_t i = 0; i < dane->ludzie.size(); i++) 
    {
        auto& osoba = dane->ludzie[i];
        if (osoba.czy_w_windzie && !dane->winda.czy_jedzie) 
        {
            int pietroDoceloweY = 560 - 100 * osoba.koniec_pietro;
            if (abs(dane->winda.pozycja_y + 100 - pietroDoceloweY) <= 5)
            {
                dane->winda.ile_osob--;
                dane->winda.laczna_waga -= osoba.waga;
                osoba.czy_w_windzie = false;
                osoba.wymiar_x = 10;
                osoba.czy_do_usuniecia = true;
            }
        }
    }
    for (size_t i = 0; i < dane->ludzie.size();) 
    {
        if (dane->ludzie[i].czy_do_usuniecia) 
        {
            dane->ludzie.erase(dane->ludzie.begin() + i);
        }
        else 
        {
            ++i;
        }
    }
}

//ustawianie osob w windzie w odpowiedni sposób
void UstawPozycjeOsobWWindzie(DaneProgramu* dane) 
{
    int startX = 270, odstep = 20, licznik = 0;
    for (int i = 0; i < dane->ludzie.size(); i++) 
    {
        if (dane->ludzie[i].czy_w_windzie) {
            dane->ludzie[i].wymiar_x = startX + licznik * odstep;
            licznik++;
        }
    }

}

//sprawdzanie czy winna jest bezczynna > 5s, jezeli tak, wraca na poziom 0 
void ObsluzBezczynnosc(DaneProgramu* dane) 
{
    Winda& winda = dane->winda;

    bool ktos_czeka = false;
    for (int i = 0; i < dane->ludzie.size(); i++) 
    {
        if (!dane->ludzie[i].czy_w_windzie) 
        {
            ktos_czeka = true;
            break;
        }
    }

    if (!winda.czy_jedzie && !winda.czy_pauzuje && winda.ile_osob == 0 && !ktos_czeka) 
    {
        if (!winda.licz_bezczynnosc) 
        {
            winda.czas_bezczynnosci_start = chrono::steady_clock::now();
            winda.licz_bezczynnosc = true;
        }
        else 
        {
            auto teraz = chrono::steady_clock::now();
            auto czas_bez = chrono::duration_cast<chrono::seconds>(
                teraz - winda.czas_bezczynnosci_start).count();

            if (czas_bez >= 5) 
            {
                winda.kierunek = -1;
                winda.cel_windy = 460;
                winda.czy_jedzie = true;
                winda.licz_bezczynnosc = false;
            }
        }
    }
    else 
    {
        winda.licz_bezczynnosc = false;
    }
}

// funkcja zapisujaca na jakie pietro trzeba jeszcze wrocic, jezeli na danym pietrze bylo za duzo osob i nie wszyscy sie zmiescili
void ObsluzPowrotyNaPietra(DaneProgramu* dane) {
    Winda& winda = dane->winda;

    if (!winda.czy_jedzie && !winda.czy_pauzuje && !winda.pietra_do_powrotu.empty()) 
    {
        while (!winda.pietra_do_powrotu.empty()) 
        {
            int pietro = winda.pietra_do_powrotu.front();
            bool ktos_czeka = false;

            for (int i = 0; i < winda.kolejkaPieterG.size(); i++) 
            {
                if (winda.kolejkaPieterG[i] == pietro) 
                {
                    ktos_czeka = true;
                    break;
                }
            }

            if (!ktos_czeka) {
                for (int i = 0; i < winda.kolejkaPieterD.size(); i++) 
                {
                    if (winda.kolejkaPieterD[i] == pietro) 
                    {
                        ktos_czeka = true;
                        break;
                    }
                }

            }

            if (ktos_czeka) 
            {
                winda.cel_windy = 560 - 100 * (pietro + 1);
                if (winda.pozycja_y < winda.cel_windy) 
                {
                    winda.kierunek = 1;
                }
                else 
                {
                    winda.kierunek = -1;
                }

                winda.czy_jedzie = true;
                winda.pietra_do_powrotu.erase(winda.pietra_do_powrotu.begin());
                break;
            }
            else 
            {
                winda.pietra_do_powrotu.erase(winda.pietra_do_powrotu.begin());
            }
        }
    }
}

/***

funkcje do WM_PAINT

***/


//rysowanie tla, pieter i szybu windy
void RysujTlo(Graphics* grafika) {
    SolidBrush pedzelBialy(Color(255, 255, 255, 255));
    Pen dlugopisCzarny(Color(255, 0, 0, 0), 2);

    grafika->FillRectangle(&pedzelBialy, 0, 0, 717, 700);
    grafika->DrawRectangle(&dlugopisCzarny, 250, 10, 200, 640);

    for (int i = 0; i <= 4; i++) 
    {
        if (i % 2 != 0) {
            grafika->DrawRectangle(&dlugopisCzarny, 450, 560 - 100 * i, 250, 10);
        }
        else {
            grafika->DrawRectangle(&dlugopisCzarny, 0, 560 - 100 * i, 250, 10);
        }
    }
}

//rysowanie windy
void RysujWinde(Graphics* grafika, const Winda& winda) 
{
    Pen dlugopisDoWindyCzerwony(Color(255, 255, 0, 0), 2);
    grafika->DrawRectangle(&dlugopisDoWindyCzerwony, 252, winda.pozycja_y, 196, 100);
}

//rysowanie ludzi
void RysujLudzi(Graphics* grafika, const vector<czlowiek>& ludzie) 
{
    FontFamily fontFamily(L"Arial");
    Font font(&fontFamily, 12);
    SolidBrush pedzelTekstu(Color(255, 255, 0, 0));

    for (size_t i = 0; i < ludzie.size(); i++) {
        Bitmap* bmpLudzik = Bitmap::FromFile(ludzie[i].plec.c_str());
        if (bmpLudzik && bmpLudzik->GetLastStatus() == Ok) 
        {
            WCHAR tekst[4];
            swprintf_s(tekst, L"%d", ludzie[i].koniec_pietro);
            PointF pozycjaTextu((REAL)ludzie[i].wymiar_x + 2, (REAL)(ludzie[i].wymiar_y - 20));
            grafika->DrawString(tekst, -1, &font, pozycjaTextu, &pedzelTekstu);
            grafika->DrawImage(bmpLudzik, ludzie[i].wymiar_x, ludzie[i].wymiar_y, 19, 50);
        }
        delete bmpLudzik;
    }
}

//rysowanie przycisków
void RysujPrzyciski(Graphics* grafika) 
{
    SolidBrush pedzelBialy2(Color(255, 255, 255, 255));
    for (int i = 0; i <= 4; i++) 
    {
        if (i == 1 || i == 3) 
        {
            grafika->FillRectangle(&pedzelBialy2, 657, 9 + 100 * i, 400, 150);
        }
        else 
        {
            grafika->FillRectangle(&pedzelBialy2, 0, 9 + 100 * i, 40, 150);
        }
    }
}

//rysowanie wskaznika wagi
void RysujWage(Graphics* grafika, int laczna_waga)
{
    FontFamily fontFamily(L"Arial");
    Font fontMaly(&fontFamily, 9);
    SolidBrush pedzelTekstuCzarny(Color(255, 0, 0, 0));
    Pen dlugopisCzarny(Color(255, 0, 0, 0), 2);
    WCHAR tekstWaga[50];

    grafika->DrawRectangle(&dlugopisCzarny, 480, 30, 180, 60);
    swprintf_s(tekstWaga, L"Waga pasażerów w windzie:");
    PointF pozycjaTextu(490, 40);
    grafika->DrawString(tekstWaga, -1, &fontMaly, pozycjaTextu, &pedzelTekstuCzarny);

    swprintf_s(tekstWaga, L"%d kg", laczna_waga);
    PointF pozycjaLiczba(550, 60);
    grafika->DrawString(tekstWaga, -1, &fontMaly, pozycjaLiczba, &pedzelTekstuCzarny);
}

LRESULT CALLBACK ObslugaOkna(HWND uchwytOkna, UINT komunikat, WPARAM wParam, LPARAM lParam);

int WINAPI WinMain(HINSTANCE instancjaAplikacji, HINSTANCE poprzedniaInstancja, LPSTR argumenty, int trybWyswietlania) 
{
    srand((unsigned)time(NULL));
    GdiplusStartupInput parametryStartowe;
    ULONG_PTR tokenGDI;
    GdiplusStartup(&tokenGDI, &parametryStartowe, NULL);

    WNDCLASS klasaOkna = { 0 };
    klasaOkna.lpfnWndProc = ObslugaOkna;
    klasaOkna.hInstance = instancjaAplikacji;
    klasaOkna.lpszClassName = L"Projekt 4 - Winda";
    RegisterClass(&klasaOkna);

    HWND uchwytOkna = CreateWindowEx(
        WS_EX_COMPOSITED,
        L"Projekt 4 - Winda",
        L"Projekt 4 - Winda",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        717,
        700,
        NULL, NULL, instancjaAplikacji, NULL);

    ShowWindow(uchwytOkna, trybWyswietlania);

    MSG wiadomosc;
    while (GetMessage(&wiadomosc, NULL, 0, 0)) 
    {
        TranslateMessage(&wiadomosc);
        DispatchMessage(&wiadomosc);
    }

    GdiplusShutdown(tokenGDI);
    return (int)wiadomosc.wParam;
}

LRESULT CALLBACK ObslugaOkna(HWND uchwytOkna, UINT komunikat, WPARAM wParam, LPARAM lParam) 
{
    switch (komunikat) 
    {
        case WM_CREATE:
        {
            DaneProgramu* dane = new DaneProgramu;
            SetWindowLongPtr(uchwytOkna, GWLP_USERDATA, (LONG_PTR)dane);

            HINSTANCE instancja = ((LPCREATESTRUCT)lParam)->hInstance;

            StworzPrzyciski(uchwytOkna, instancja);

            SetTimer(uchwytOkna, 1, 40, NULL);
            break;
        }

        case WM_COMMAND:
        {
            DaneProgramu* dane = (DaneProgramu*)GetWindowLongPtr(uchwytOkna, GWLP_USERDATA);

            int id = LOWORD(wParam);
            int start_pietro = id / 10;
            int koniec_pietro = id % 10;

            // Dodaj nową osobę
            DodajCzlowieka(dane, start_pietro, koniec_pietro);
            czlowiek& nowyCzlowiek = dane->ludzie.back();

            // Aktualizuj kolejki
            if (!dane->winda.czy_jedzie && !dane->winda.czy_pauzuje &&
                dane->winda.kolejkaPieterG.empty() && dane->winda.kolejkaPieterD.empty())
            {
                dane->winda.czy_jedzie = true;
                if (nowyCzlowiek.gdzie_jedzie == 1) 
                {
                    dane->winda.kolejkaPieterG.push_back(nowyCzlowiek.start_pietro);
                    dane->winda.kolejkaPieterG.push_back(nowyCzlowiek.koniec_pietro);
                    sort(dane->winda.kolejkaPieterG.begin(), dane->winda.kolejkaPieterG.end());
                }
                else 
                {
                    dane->winda.kolejkaPieterD.push_back(nowyCzlowiek.start_pietro);
                    dane->winda.kolejkaPieterD.push_back(nowyCzlowiek.koniec_pietro);
                    sort(dane->winda.kolejkaPieterD.begin(), dane->winda.kolejkaPieterD.end(), greater<int>());
                }
            }
            else
            {
                if (nowyCzlowiek.gdzie_jedzie == 1) 
                {
                    AktualizujKolejkeGora(dane->winda, nowyCzlowiek.start_pietro, nowyCzlowiek.koniec_pietro);
                }
                else 
                {
                    AktualizujKolejkeDol(dane->winda, nowyCzlowiek.start_pietro, nowyCzlowiek.koniec_pietro);
                }
            }

            InvalidateRect(uchwytOkna, NULL, TRUE);
            break;
        }

        case WM_TIMER:
        {
            if (wParam == 1) 
            {
                DaneProgramu* dane = (DaneProgramu*)GetWindowLongPtr(uchwytOkna, GWLP_USERDATA);
                if (!dane) break;

                Winda& winda = dane->winda;

                // pauza windy
                if (winda.czy_pauzuje) 
                {
                    winda.pauza -= 27;
                    if (winda.pauza <= 0) {
                        winda.czy_pauzuje = false;

                        if (!winda.kolejkaPieterG.empty() || !winda.kolejkaPieterD.empty()) 
                        {
                            if (!winda.kolejkaPieterG.empty()) 
                            {
                                winda.cel_windy = 560 - 100 * (winda.kolejkaPieterG.front() + 1);
                                winda.kolejkaPieterG.erase(winda.kolejkaPieterG.begin());
                                winda.kierunek = 1;
                            }
                            else 
                            {
                                winda.cel_windy = 560 - 100 * (winda.kolejkaPieterD.front() + 1);
                                winda.kolejkaPieterD.erase(winda.kolejkaPieterD.begin());
                                winda.kierunek = -1;
                            }
                            winda.czy_jedzie = true;
                        }
                        else 
                        {
                            winda.czy_jedzie = false;
                        }
                    }
                    InvalidateRect(uchwytOkna, NULL, TRUE);
                    return 0;
                }

                // ruch windy
                if (winda.czy_jedzie) 
                {
                    int& y = winda.pozycja_y;
                    int cel = winda.cel_windy;

                    if (y < cel)
                    {
                        y += min(3, cel - y);
                    }
                    else if (y > cel)
                    {
                        y -= min(3, y - cel);
                    }

                    for (int i = 0; i < dane->ludzie.size(); i++) 
                    {
                        if (dane->ludzie[i].czy_w_windzie) 
                        {
                            dane->ludzie[i].wymiar_y = y + 48;
                        }
                    }

                    if (y == cel) 
                    {
                        winda.czy_jedzie = false;
                        winda.czy_pauzuje = true;
                        winda.pauza = 100;
                    }

                    InvalidateRect(uchwytOkna, NULL, TRUE);
                }

                // wysiadanie i wsiadanie
                WsiadanieDoWindy(dane, uchwytOkna);
                WysiadanieZWindy(dane);

                // utawianie ludzi w windzie obok siebie
                UstawPozycjeOsobWWindzie(dane);

                // sprawdzanie bezczynnosci windy
                ObsluzBezczynnosc(dane);

                // obslugiwanie pieter gdzie ludzie sie nie zmiescili na raz
                ObsluzPowrotyNaPietra(dane);

                InvalidateRect(uchwytOkna, NULL, TRUE);
            }
            break;
        }


        case WM_PAINT: 
        {
            DaneProgramu* dane = (DaneProgramu*)GetWindowLongPtr(uchwytOkna, GWLP_USERDATA);
            PAINTSTRUCT uchwytMalowania;
            HDC hdc = BeginPaint(uchwytOkna, &uchwytMalowania);

            RECT rect;
            GetClientRect(uchwytOkna, &rect);
            Bitmap bufor(rect.right, rect.bottom, PixelFormat32bppARGB);
            Graphics grafika(&bufor);

            RysujTlo(&grafika);
            RysujWinde(&grafika, dane->winda);
            RysujLudzi(&grafika, dane->ludzie);
            RysujPrzyciski(&grafika);
            RysujWage(&grafika, dane->winda.laczna_waga);
            Graphics ekran(hdc);
            ekran.DrawImage(&bufor, 0, 0);

            EndPaint(uchwytOkna, &uchwytMalowania);
            return 0;
        }

        case WM_DESTROY:
            DaneProgramu* dane = (DaneProgramu*)GetWindowLongPtr(uchwytOkna, GWLP_USERDATA);
            delete dane;
            PostQuitMessage(0);
            return 0;
    }

    return DefWindowProc(uchwytOkna, komunikat, wParam, lParam);
}