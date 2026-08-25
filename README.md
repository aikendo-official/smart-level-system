# AIKENDO TECH SCADA - Water Treatment Plant Monitoring System

Solusi sistem pemantauan terintegrasi (*Industrial SCADA & HMI System*) berbasis microcontroller **ESP32**, **WebSockets**, dan **TFT Display (TFT_eSPI)**. Project ini dirancang untuk memantau indikator industri seperti level air, volume reservoir, flow rate, dan sinyal analog transmiter (4-20mA) secara real-time melalui web browser dashboard dan layar lokal TFT.

---

## 🌟 Fitur Utama

- **Dual-Display System**:
  - **TFT Display HMI**: Tampilan antarmuka lokal pada layar TFT (320x480) dengan konsep *flicker-free UI*, indikator status real-time, jam NTP, serta visualisasi level tangki.
  - **Web Dashboard (WebSocket)**: Antarmuka berbasis HTML5, CSS3, dan JS (Chart.js) yang responsif untuk monitoring multi-device jarak jauh melalui koneksi WebSocket berkecepatan tinggi.
- **Monitoring Industri Metric**:
  - Tinggi muka air (*Level*) dalam unit `cm`.
  - Kapasitas volume air dalam unit `m³` / `Liter`.
  - Laju aliran air (*Flow Rate*) dalam unit `L/s`.
  - Simulasi sinyal sensor industri **4-20 mA**.
- **System Resource Diagnostics**:
  - Monitoring penggunaan **RAM (Free Heap)** dan **FLASH Memory (Sketch Size)** secara langsung pada layar dashboard.
- **Real-Time Synchronization**:
  - Sinkronisasi waktu otomatis melalui server **NTP** (`pool.ntp.org`).
  - Pembaruan data *flicker-free* pada display lokal dan update *push* otomatis ke Web Browser tanpa reload halaman.

---

## 🏗️ Arsitektur Proyek

Proyek ini terdiri dari 2 file utama:

| File | Deskripsi |
| :--- | :--- |
| `main.cpp` / `*.ino` | Kode C++ ESP32 yang menangani konektivitas WiFi, NTP, pengolahan sensor, driver layar TFT (`TFT_eSPI`), WebSocket server (`ESPAsyncWebServer`), dan pengiriman data JSON (`ArduinoJson`). |
| `index_html.h` | Template HTML/CSS/JS yang disimpan pada memori `PROGMEM` ESP32 untuk menyajikan Web Dashboard interaktif. |

---

## 🛠️ Komponen & Library yang Dibutuhkan

### Hardware
- Microcontroller **ESP32** (NodeMCU ESP32 / ESP32-WROOM-32)
- Display Module TFT SPI (Mendukung driver `TFT_eSPI`, misalnya ILI9488 / ST7796 / ILI9341 480x320)

### Software / Libraries (Arduino IDE / PlatformIO)
1. **[WiFi.h](https://github.com/espressif/arduino-esp32)** (Bawaan ESP32)
2. **[AsyncTCP](https://github.com/me-no-dev/AsyncTCP)**
3. **[ESPAsyncWebServer](https://github.com/me-no-dev/ESPAsyncWebServer)**
4. **[ArduinoJson](https://arduinojson.org/)** (v6.x)
5. **[TFT_eSPI](https://github.com/Bodmer/TFT_eSPI)**
6. **[Chart.js](https://www.chartjs.org/)** (Dimuat via CDN pada web clients)

---

## 🚀 Panduan Memulai (Quick Start)

### 1. Konfigurasi TFT_eSPI
Sebelum melakukan upload program, pastikan Anda telah mengonfigurasi file `User_Setup.h` pada library **TFT_eSPI** sesuai dengan pinout hardware dan jenis driver layar yang Anda gunakan.

### 2. Konfigurasi WiFi
Buka file utama project (`.ino` atau `main.cpp`), lalu ubah kredensial jaringan WiFi Anda pada baris berikut:

```cpp
const char* ssid = "NAMA_WIFI_ANDA";
const char* password = "PASSWORD_WIFI_ANDA";
