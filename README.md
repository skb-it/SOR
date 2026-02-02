# 🏥 SOR - Symulacja Szpitalnego Oddziału Ratunkowego

Wieloprocesowa symulacja działania Szpitalnego Oddziału Ratunkowego (SOR) z wykorzystaniem mechanizmów **IPC System V** (kolejki komunikatów, semafory, pamięć dzielona).

---

## 📋 Spis treści

- [Opis projektu](#-opis-projektu)
- [Wymagania systemowe](#-wymagania-systemowe)
- [Instalacja i uruchomienie](#-instalacja-i-uruchomienie)
- [Architektura systemu](#-architektura-systemu)
- [Mechanizmy IPC](#-mechanizmy-ipc)
- [Obsługa sygnałów](#-obsługa-sygnałów)



---

## 🎯 Opis projektu

System symuluje przepływ pacjentów przez kluczowe etapy obsługi medycznej:

```
Pacjent → Poczekalnia → Rejestracja → Lekarz POZ (Triaż) → Specjalista → Wyjście
```

### Główne funkcjonalności:

- **System triażu** - przydzielanie kodów pilności (🔴 Czerwony / 🟡 Żółty / 🟢 Zielony)
- **6 lekarzy specjalistów** - Kardiolog, Neurolog, Okulista, Laryngolog, Chirurg, Pediatra
- **Dynamiczna rejestracja** - automatyczne otwieranie/zamykanie drugiego okienka
- **Obsługa pacjentów VIP** (15% szansy) z priorytetem obsługi
- **Obsługa dzieci** (<18 lat) z towarzyszącym opiekunem
- **Ewakuacja** - bezpieczne zamknięcie systemu (Ctrl+C)
- **Statystyki końcowe** - raport z liczbą obsłużonych pacjentów

---

## 💻 Wymagania systemowe

- System operacyjny: **Linux** (testowane na serwerze Torus)
- Kompilator: **gcc** lub **g++**
- Narzędzie: **make**
- Biblioteki IPC System V:
  - `sys/msg.h` - kolejki komunikatów
  - `sys/sem.h` - semafory
  - `sys/shm.h` - pamięć dzielona

---

## 🚀 Instalacja i uruchomienie

### Kompilacja

```bash
# Wyczyść poprzednie pliki binarne
make clean

# Skompiluj projekt
make all
```

### Uruchomienie

```bash
./director
```

Po uruchomieniu system poprosi o podanie **N** - rozmiaru poczekalni.

### Zatrzymanie symulacji

Wciśnij `Ctrl+C` aby wywołać procedurę ewakuacji i bezpiecznie zakończyć wszystkie procesy.

---

## 🏗 Architektura systemu

### Procesy:

| Proces | Plik | Opis |
|--------|------|------|
| **Dyrektor** | `director.c` | Zarządca systemu, inicjalizacja IPC, monitoring |
| **Generator** | `generator.c` | Tworzenie procesów pacjentów |
| **Pacjent** | `patient.c` | Reprezentacja pacjenta w systemie |
| **Rejestracja** | `registration.c` | Obsługa rejestracji (1-2 okienka) |
| **Lekarz POZ** | `pc_doctor.c` | Triaż i kierowanie do specjalistów |
| **Specjaliści** | `*.c` | 6 procesów lekarzy specjalistów |

---

## 🔄 Mechanizmy IPC

### Semafory

| ID | Nazwa | Przeznaczenie |
|----|-------|---------------|
| `'a'` | `ID_SEM_WAITING_ROOM` | Limituje miejsca w poczekalni (N) |
| `'m'` | `ID_SEM_STATS` | Chroni dostęp do statystyk |
| `'d'-'i'` | `ID_SEM_MSG_*` | Synchronizacja ze specjalistami |

### Kolejki komunikatów

| ID | Nazwa | Komunikacja |
|----|-------|-------------|
| `'s'` | `ID_MSG_PAT_REG` | Pacjent → Rejestracja |
| `'r'` | `ID_MSG_PAT_DOC` | Pacjent ↔ Lekarz POZ |
| `'t'-'z'` | `ID_MSG_PAT_*` | Pacjent ↔ Specjaliści |

### Pamięć dzielona

| ID | Nazwa | Zawartość |
|----|-------|-----------|
| `'n'` | `ID_SHM_STATS` | Struktura `PatientStats` ze statystykami |
| `'p'` | `ID_SHM_REG_DOC` | Dane pacjenta (Rejestracja → POZ) |

---

## 📡 Obsługa sygnałów

| Sygnał | Akcja |
|--------|-------|
| `SIGINT` (Ctrl+C) | Ewakuacja - bezpieczne zakończenie wszystkich procesów |
| `SIGUSR1` | Wezwanie lekarza na oddział (czasowa niedostępność) |
| `SIGTERM` | Bezpieczne zakończenie procesu ze zwolnieniem zasobów |
| `SIGCHLD` | Zbieranie zakończonych procesów potomnych |

---

