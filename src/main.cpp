#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <matplot/matplot.h>
#include <vector>
#include <complex>
#include <algorithm>
#include <stdexcept>
#define _USE_MATH_DEFINES
#include <cmath>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

namespace py = pybind11;
using namespace matplot;
using Kompleks = std::complex<double>;
using TablicaKompleks = std::vector<Kompleks>;

std::vector<double> filtr_1d(const std::vector<double>& sygnal) {
    const int rozmiar_okna = 5;
    std::vector<double> wynik(sygnal.size());
    int polowa_okna = rozmiar_okna / 2;

    for (size_t i = 0; i < sygnal.size(); ++i) {
        double suma = 0.0;
        int licznik = 0;

        for (int j = -polowa_okna; j <= polowa_okna; ++j) {
            int indeks = static_cast<int>(i) + j;
            if (indeks >= 0 && indeks < static_cast<int>(sygnal.size())) {
                suma += sygnal[indeks];
                licznik++;
            }
        }

        wynik[i] = suma / licznik;
    }

    return wynik;
}

void pokaz_filtracje_1d(const std::vector<double>& sygnal) {
    auto przefiltrowany = filtr_1d(sygnal);

    auto figura = figure();
    figura->size(800, 400);
    hold(on);
    auto p1 = plot(sygnal);
    auto p2 = plot(przefiltrowany);
    p1->line_width(1.5).display_name("Oryginalny");
    p2->line_width(1.5).display_name("Przefiltrowany");
    title("Filtracja 1D");
    xlabel("Próbka");
    ylabel("Amplituda");
    legend();
    grid(on);
    hold(off);
    save("filtracja1d.png");
}

std::vector<int> znajdz_piki_binarne(const std::vector<double>& sygnal) {
    std::vector<int> piki(sygnal.size(), 0);
    if(sygnal.empty()) return piki;
    
    auto max_iter = std::max_element(sygnal.begin(), sygnal.end());
    double max_wartosc = *max_iter;
    if(max_wartosc == 0) return piki;
    
    std::vector<double> znormalizowany;
    for(auto wartosc : sygnal) {
        znormalizowany.push_back(std::abs(wartosc) / max_wartosc);
    }
    
    const double prog = 0.5;
    const int min_odleglosc = 10;
    int ostatni_pik = 0;
    
    for(size_t i = 1; i < znormalizowany.size()-1; ++i) {
        if(znormalizowany[i] > prog && 
           znormalizowany[i] > znormalizowany[i-1] && 
           znormalizowany[i] > znormalizowany[i+1] &&
           (i - ostatni_pik) > min_odleglosc) {
            piki[i] = 1;
            ostatni_pik = i;
        }
    }
    
    return piki;
}

void pokaz_piki_porownanie(const std::vector<double>& sygnal) {
    auto piki = znajdz_piki_binarne(sygnal);
    
    auto figura = figure();
    figura->size(800, 600);
    
    subplot(2,1,0);
    auto wykres1 = plot(sygnal);
    wykres1->line_width(1.5);
    title("Sygnał oryginalny");
    xlabel("Próbka");
    ylabel("Amplituda");
    grid(on);
    
    subplot(2,1,1);
    auto wykres2 = stairs(piki);
    wykres2->line_width(1.5).color("r");
    title("Wykryte piki");
    xlabel("Próbka");
    ylabel("Pik (0/1)");
    ylim({-0.1, 1.1});
    grid(on);
    
    save("wykrywanie_piku.png");
}

TablicaKompleks oblicz_dft(const std::vector<double>& sygnal) {
    size_t N = sygnal.size();
    TablicaKompleks dft(N);
    
    for (size_t k = 0; k < N; k++) {
        for (size_t n = 0; n < N; n++) {
            double kat = -2 * M_PI * k * n / N;
            dft[k] += sygnal[n] * Kompleks(std::cos(kat), std::sin(kat));
        }
    }
    return dft;
}

std::vector<double> oblicz_idft(const TablicaKompleks& dft) {
    size_t N = dft.size();
    std::vector<double> sygnal(N);
    
    for (size_t n = 0; n < N; n++) {
        Kompleks suma(0, 0);
        for (size_t k = 0; k < N; k++) {
            double kat = 2 * M_PI * k * n / N;
            suma += dft[k] * Kompleks(std::cos(kat), std::sin(kat));
        }
        sygnal[n] = suma.real() / N;
    }
    return sygnal;
}

void pokaz_dft_idft(const std::vector<double>& sygnal) {
    const double czest_probkowania = 1.0;
    auto dft = oblicz_dft(sygnal);
    auto rekonstrukcja = oblicz_idft(dft);
    
    size_t N = sygnal.size();
    std::vector<double> czestotliwosci(N/2), amplitudy(N/2);
    for (size_t i = 0; i < N/2; i++) {
        czestotliwosci[i] = i * czest_probkowania / N;
        amplitudy[i] = std::abs(dft[i]) / N;
    }

    auto figura1 = figure();
    figura1->size(800, 400);
    stem(czestotliwosci, amplitudy);
    title("Widmo DFT"); 
    xlabel("Częstotliwość [Hz]"); 
    ylabel("Amplituda");
    grid(on);
    save("dft.png");

    auto figura2 = figure();
    figura2->size(800, 400);
    plot(rekonstrukcja)->line_width(1.5).color("r");
    title("Sygnał zrekonstruowany"); 
    xlabel("Próbka"); 
    ylabel("Amplituda");
    grid(on);
    save("idft.png");
}

void rysuj_sygnal(const std::vector<double>& y, const std::vector<double>& x = {}) {
    auto figura = figure();
    figura->size(800, 400);
    if (x.empty()) {
        plot(y)->line_width(1.5);
        xlabel("Próbka");
    } else {
        plot(x, y)->line_width(1.5);
        xlabel("Czas [s]");
    }
    ylabel("Amplituda");
    ylim({-1.5, 1.5});
    grid(on);
    save("sygnal.png");
}

std::vector<double> moja_linspace(double start, double end, size_t num) {
    std::vector<double> result(num);
    if (num == 1) {
        result[0] = start;
    } else {
        double step = (end - start) / (num - 1);
        for (size_t i = 0; i < num; ++i) {
            result[i] = start + i * step;
        }
    }
    return result;
}

void rysuj_sin(double czestotliwosc = 1.0, double start = 0.0, double end = 2 * M_PI, size_t num_samples = 1000) {
    if (czestotliwosc <= 0) throw std::invalid_argument("Częstotliwość musi być dodatnia");
    if (num_samples == 0) throw std::invalid_argument("Liczba próbek musi być większa od 0");
    
    auto x = moja_linspace(start, end, num_samples);
    std::vector<double> y(num_samples);
    for (size_t i = 0; i < num_samples; ++i) {
        y[i] = std::sin(czestotliwosc * x[i]);
    }
    auto figura = figure();
    figura->size(800, 400);
    plot(x, y)->line_width(1.5);
    title("Sinus");
    xlabel("Czas [s]");
    ylabel("Amplituda");
    ylim({-1.5, 1.5});
    grid(on);
    save("sinus.png");
}

void rysuj_cos(double czestotliwosc = 1.0, double start = 0.0, double end = 2 * M_PI, size_t num_samples = 1000) {
    if (czestotliwosc <= 0) throw std::invalid_argument("Częstotliwość musi być dodatnia");
    if (num_samples == 0) throw std::invalid_argument("Liczba próbek musi być większa od 0");

    auto x = moja_linspace(start, end, num_samples);
    std::vector<double> y(num_samples);
    for (size_t i = 0; i < num_samples; ++i) {
        y[i] = std::cos(czestotliwosc * x[i]);
    }
    auto figura = figure();
    figura->size(800, 400);
    plot(x, y)->line_width(1.5);
    title("Cosinus");
    xlabel("Czas [s]");
    ylabel("Amplituda");
    ylim({-1.5, 1.5});
    grid(on);
    save("cosinus.png");
}

void rysuj_prostokat(double czestotliwosc = 1.0, double start = 0.0, double end = 2 * M_PI, size_t num_samples = 1000) {
    if (czestotliwosc <= 0) throw std::invalid_argument("Częstotliwość musi być dodatnia");
    if (num_samples == 0) throw std::invalid_argument("Liczba próbek musi być większa od 0");

    auto x = moja_linspace(start, end, num_samples);
    std::vector<double> y(num_samples);
    for (size_t i = 0; i < num_samples; ++i) {
        y[i] = std::sin(czestotliwosc * x[i]) >= 0 ? 1.0 : -1.0;
    }
    auto figura = figure();
    figura->size(800, 400);
    plot(x, y)->line_width(1.5);
    title("Sygnał prostokątny");
    xlabel("Czas [s]");
    ylabel("Amplituda");
    ylim({-1.5, 1.5});
    grid(on);
    save("prostokatny.png");
}

void rysuj_pila(double czestotliwosc = 1.0, double start = 0.0, double end = 2 * M_PI, size_t num_samples = 1000) {
    if (czestotliwosc <= 0) throw std::invalid_argument("Częstotliwość musi być dodatnia");
    if (num_samples == 0) throw std::invalid_argument("Liczba próbek musi być większa od 0");

    auto x = moja_linspace(start, end, num_samples);
    std::vector<double> y(num_samples);
    for (size_t i = 0; i < num_samples; ++i) {
        double faza = std::fmod(czestotliwosc * x[i], 2 * M_PI);
        y[i] = 2.0 * (faza / (2 * M_PI)) - 1.0;
    }
    auto figura = figure();
    figura->size(800, 400);
    plot(x, y)->line_width(1.5);
    title("Sygnał piłokształtny");
    xlabel("Czas [s]");
    ylabel("Amplituda");
    ylim({-1.1, 1.1});
    grid(on);
    save("piloksztaltny.png");
}

std::vector<std::vector<double>> filtr_2d(const std::vector<std::vector<double>>& obraz) {
    const int rozmiar_okna = 3;
    int polowa = rozmiar_okna / 2;
    int wiersze = static_cast<int>(obraz.size());
    if (wiersze == 0) return obraz;
    int kolumny = static_cast<int>(obraz[0].size());

    std::vector<std::vector<double>> wynik(wiersze, std::vector<double>(kolumny, 0.0));

    for (int i = 0; i < wiersze; ++i) {
        for (int j = 0; j < kolumny; ++j) {
            double suma = 0.0;
            int licznik = 0;

            for (int wi = -polowa; wi <= polowa; ++wi) {
                for (int wj = -polowa; wj <= polowa; ++wj) {
                    int ni = i + wi;
                    int nj = j + wj;
                    if (ni >= 0 && ni < wiersze && nj >= 0 && nj < kolumny) {
                        suma += obraz[ni][nj];
                        licznik++;
                    }
                }
            }

            wynik[i][j] = suma / licznik;
        }
    }

    return wynik;
}

void pokaz_obraz_2d(const std::vector<std::vector<double>>& obraz, const std::string& nazwa_pliku, const std::string& naglowek) {
    matplot::figure();
    matplot::imagesc(obraz);
    matplot::colormap(matplot::palette::gray());
    matplot::colorbar();
    matplot::title(naglowek);
    matplot::save(nazwa_pliku);
}


PYBIND11_MODULE(_core, m) {
     m.def("sinus", &rysuj_sin, 
          py::arg("czestotliwosc") = 1.0,
          py::arg("start") = 0.0,
          py::arg("end") = 2 * M_PI,
          py::arg("num_samples") = 1000,
          "Rysuje sygnał sinusoidalny");

    m.def("cosinus", &rysuj_cos,
          py::arg("czestotliwosc") = 1.0,
          py::arg("start") = 0.0,
          py::arg("end") = 2 * M_PI,
          py::arg("num_samples") = 1000,
          "Rysuje sygnał cosinusoidalny");

    m.def("prostokat", &rysuj_prostokat,
          py::arg("czestotliwosc") = 1.0,
          py::arg("start") = 0.0,
          py::arg("end") = 2 * M_PI,
          py::arg("num_samples") = 1000,
          "Rysuje sygnał prostokątny");

    m.def("pila", &rysuj_pila,
          py::arg("czestotliwosc") = 1.0,
          py::arg("start") = 0.0,
          py::arg("end") = 2 * M_PI,
          py::arg("num_samples") = 1000,
          "Rysuje sygnał piłokształtny");
          
    m.def("dft_idft", &pokaz_dft_idft, 
          py::arg("sygnal"),
          "Pokazuje DFT i rekonstrukcję sygnału");

    m.def("pokaz_piki", &pokaz_piki_porownanie,
          py::arg("sygnal"),
          "Pokazuje wykryte piki w sygnale");

    m.def("filtruj_1d", &filtr_1d,
          py::arg("sygnal"),
          "Filtruje sygnał 1D");

    m.def("pokaz_filtracje", &pokaz_filtracje_1d,
          py::arg("sygnal"),
          "Pokazuje efekt filtracji 1D");

    m.def("filtruj_2d", &filtr_2d,
          py::arg("obraz"),
          "Filtruje obraz 2D");

    m.def("pokaz_obraz", &pokaz_obraz_2d,
      py::arg("obraz"), py::arg("nazwa_pliku"), py::arg("naglowek"),
      "Wyświetla i zapisuje obraz 2D do pliku");
}