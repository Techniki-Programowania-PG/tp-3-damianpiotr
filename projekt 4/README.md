# Sprawozdanie z projektu 4 zadanie 3 (winda)

## 1. Treść zadania
>Napisz program symulujący działanie windy. Symulator powinien uwzględniać możliwość zatrzymywania
się na poszczególnych piętrach oraz przywołania winda za pomocą przycisku na interfejsie użytkownika.
Program powinien kolejkować wywołania. Prezentacja poruszania się windy powinna zostać
przedstawiona na modelu osadzonym w przestrzeni dwuwymiarowej. <br /><br />
>3.1<br />
Zasymuluj w aplikacji wożenie osób na poszczególne piętra. Użytkownik ma możliwość zdefiniowania
ilości osób wchodzących i wychodzących na poszczególnych piętrach. Stwórz mechanizm kontrolujący
maksymalny udźwig widny. Przyjmij, że średnia waga pasażera to 70kg, natomiast maksymalny udźwig
windy to 600 kg.<br /><br />
>3.2<br />
Stwórz mechanizm sprawdzający czy winda po zatrzymaniu się na określonym piętrze jest pusta. Jeśli tak
odliczaj 5 sek. i zjeżdżaj windą na parter.<br /><br />
>3.3<br />
Napisz program prezentujący na interfejsie użytkownika masę przewożonych pasażerów."

## 2. Skład grupy
**Damian Rutkowski 203304** <br />
**Aleksander Stachurski 207394**

## 3. Zasada działania programu

### Generowanie pasażerów

Użytkownicy (osoby) są przypisywani do określonych pięter. Każda osoba ma między innymi zdefiniowane:
- **piętro początkowe** (startowe),
- **piętro docelowe**,
- **kierunek jazdy** (w górę lub w dół),
- **wagę**.

Osoby są dodawane poprzez przyciski umieszczone przy każdym piętrze w interfejsie graficznym.

---

### Poruszanie się windy

Winda przemieszcza się między piętrami zgodnie z kolejką zgłoszeń:
- oddzielna kolejka dla **pięter w górę** i **w dół**,
- winda zatrzymuje się, jeśli:
  - ktoś ma tam **wysiąść**,
  - ktoś na piętrze **czeka na wsiadanie**,
- po osiągnięciu piętra docelowego następuje krótka **pauza**, po czym jazda jest kontynuowana.

---

### Wsiadanie i wysiadanie

- Osoby mogą wsiąść tylko wtedy, gdy:
  - winda znajduje się na ich piętrze,
  - winda nie porusza się,
  - kierunek jazdy się zgadza,
  - waga nie przekracza dopuszczalnego limitu.
- Po dotarciu do piętra docelowego osoba **opuszcza windę** i zostaje usunięta z symulacji.

---

### Bezczynność windy

- Jeżeli przez **5 sekund** nie ma żadnych zgłoszeń i nikt nie znajduje się w windzie, to:
  - winda **automatycznie wraca na parter (0 piętro)**.
- Jeżeli na którymś piętrze **nie udało się zabrać wszystkich osób** (np. z powodu przekroczenia limitu wagi), to:
  - piętro to trafia na **listę powrotów**,
  - winda wróci tam później.

## 4. Film przedstawiający działanie programu

https://github.com/user-attachments/assets/dbce4985-c9a3-468f-9947-b3cec47b6330

