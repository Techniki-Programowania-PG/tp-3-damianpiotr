import projekt3 as pt
import numpy as np
import math
from PIL import Image

# Podstawowe sygnały
pt.sinus(2.0, 0.0, 8 * np.pi, 500)  # częstotliwość=2, od 0 do 8π, 500 próbek
pt.cosinus(2.0, 0.0, 8 * np.pi, 500)
pt.prostokat(5.0, 0.0, 4 * np.pi, 1000)
pt.pila(5.0, 0.0, 4 * np.pi, 1000)

# DFT i IDFT
N = 100        # liczba próbek
f = 5          # częstotliwość sygnału w Hz
fs = 100       # częstotliwość próbkowania w Hz
sinus_sygnal = [math.sin(2 * math.pi * f * n / fs) for n in range(N)]
pt.dft_idft(sinus_sygnal)

# Wykrywanie pików
t = np.linspace(0, 4*np.pi, 200)  # 2 okresy sinusoidy
sygnal = np.sin(t)
pt.pokaz_piki(sygnal.tolist())


# Filtracja 1D
t = np.linspace(0, 16*np.pi, 200)
sygnal = np.sin(t) + 0.1*np.random.randn(200)
pt.pokaz_filtracje(sygnal.tolist())


# Przetwarzanie obrazu 2D
img = Image.open("zdjecie.jpg").convert("L")
img_np = np.array(img) / 255.0  # normalizacja do [0,1]
img_lista = img_np.tolist()

przefiltrowane = pt.filtruj_2d(img_lista)

pt.pokaz_obraz(img_lista, "oryginal.png", "Przed filtracją")
pt.pokaz_obraz(przefiltrowane, "przefiltrowany.png", "Po filtracji 2D")
