#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <ArduinoJson.h>
#include <TFT_eSPI.h>
#include <time.h> // Library bawaan ESP32 untuk NTP Time
#include "index_html.h"

// --- KONFIGURASI WIFI ---
const char* ssid = "Gyga";
const char* password = "gyagazi24";

// --- KONFIGURASI WAKTU NTP (WIB = UTC+7) ---
const char* ntpServer = "pool.ntp.org";
const long gmtOffset_sec = 7 * 3600; // GMT+7 untuk WIB
const int daylightOffset_sec = 0;

// --- DATA TANGKI TUNGGAL (RESERVOIR 1) ---
struct Tank {
  String name;
  float capacity; // m³
  float maxH;     // Max Tinggi (cm)
  float cm;       // Tinggi saat ini (cm)
  float vol;      // Volume saat ini (Liter)
  float flow;     // Flow rate (L/s)
  float ma;       // Sinyal Sensor (4-20mA)
};

Tank res1 = {"RESERVOIR 1", 30.0, 300.0, 185.0, 18500.0, 0.0, 13.86};

int lastFillH = -1; // Untuk melacak posisi isi air di tangki

TFT_eSPI tft = TFT_eSPI();
AsyncWebServer server(80);
AsyncWebSocket ws("/ws");

unsigned long lastUpdate = 0;
bool heartbeatDot = false;

// --- PALET WARNA HMI MODERN (RGB565) ---
#define COLOR_DARK_BG    0x0842 // Dark Slate / Charcoal
#define COLOR_PANEL_BG   0x18C5 // Deep Navy Gray
#define COLOR_CARD_BG    0x2128 // Card Background
#define COLOR_NEON_BLUE  0x05BF // Electric Cyan Blue
#define COLOR_WATER_BLUE 0x037F // Deep Water Blue
#define COLOR_ACCENT_YLW 0xFDE0 // Bright Amber
#define COLOR_TEXT_DIM   0x9B11 // Muted Gray/White
#define COLOR_BORDER     0x3AD5 // Panel Border
#define COLOR_GREEN_NEON 0x07E0 // Bright Emerald Green
#define COLOR_RED_NEON   0xF800 // Bright Red

void updateSensorData() {
  float lastCm = res1.cm;
  float delta = (random(-12, 13) / 10.0); // Simulasi fluida
  res1.cm += delta;
  
  if(res1.cm < 0) res1.cm = 0;
  if(res1.cm > res1.maxH) res1.cm = res1.maxH;
  
  float deltaCm = res1.cm - lastCm;
  float litersPerCm = (res1.capacity * 1000.0) / res1.maxH;
  res1.flow = deltaCm * litersPerCm;

  res1.vol = (res1.cm / res1.maxH) * (res1.capacity * 1000.0);
  res1.ma = 4.0 + ((res1.cm / res1.maxH) * 16.0);
}

// 1. MENGGAMBAR FRAME STATIS (DIJALANKAN 1 KALI SAJA SAAT BOOTING)
void drawStaticUI() {
  tft.fillScreen(COLOR_DARK_BG);

  // --- HEADER BAR ---
  tft.fillRect(0, 0, 480, 36, COLOR_PANEL_BG);
  tft.drawFastHLine(0, 36, 480, COLOR_BORDER);
  tft.setTextColor(COLOR_NEON_BLUE, COLOR_PANEL_BG);
  tft.drawString("AKND Tech. SCADA - R1", 12, 8, 4);

  // --- SKALA / RULER TANGKI ---
  int tankY = 50;
  int tankH = 215;
  for (int i = 0; i <= 4; i++) {
    int lineY = tankY + (i * (tankH / 4));
    tft.drawFastHLine(10, lineY, 20, COLOR_BORDER);
    tft.setTextColor(COLOR_TEXT_DIM, COLOR_DARK_BG);
    int pctVal = 100 - (i * 25);
    tft.drawNumber(pctVal, 2, lineY - 4, 1);
  }

  // --- METRIC CARDS FRAME ---
  int cardX = 170;
  int cardW = 295;
  int cardH = 50;

  // Kartu 1: Level
  int y1 = 48;
  tft.fillRoundRect(cardX, y1, cardW, cardH, 6, COLOR_CARD_BG);
  tft.drawRoundRect(cardX, y1, cardW, cardH, 6, COLOR_BORDER);
  tft.setTextColor(COLOR_TEXT_DIM, COLOR_CARD_BG);
  tft.drawString("LEVEL TANGKI", cardX + 10, y1 + 8, 2);
  tft.setTextColor(TFT_WHITE, COLOR_CARD_BG);
  tft.drawString("cm", cardX + 250, y1 + 15, 2);

  // Kartu 2: Volume
  int y2 = 104;
  tft.fillRoundRect(cardX, y2, cardW, cardH, 6, COLOR_CARD_BG);
  tft.drawRoundRect(cardX, y2, cardW, cardH, 6, COLOR_BORDER);
  tft.setTextColor(COLOR_TEXT_DIM, COLOR_CARD_BG);
  tft.drawString("VOLUME AIR", cardX + 10, y2 + 8, 2);
  tft.setTextColor(TFT_WHITE, COLOR_CARD_BG);
  tft.drawString("m3", cardX + 250, y2 + 15, 2);

  // Kartu 3: Flow
  int y3 = 160;
  tft.fillRoundRect(cardX, y3, cardW, cardH, 6, COLOR_CARD_BG);
  tft.drawRoundRect(cardX, y3, cardW, cardH, 6, COLOR_BORDER);
  tft.setTextColor(COLOR_TEXT_DIM, COLOR_CARD_BG);
  tft.drawString("FLOW RATE", cardX + 10, y3 + 8, 2);
  tft.setTextColor(TFT_WHITE, COLOR_CARD_BG);
  tft.drawString("L/s", cardX + 250, y3 + 15, 2);

  // Kartu 4: Current 4-20mA (Menggantikan Tren)
  int y4 = 216;
  tft.fillRoundRect(cardX, y4, cardW, cardH, 6, COLOR_CARD_BG);
  tft.drawRoundRect(cardX, y4, cardW, cardH, 6, COLOR_BORDER);
  tft.setTextColor(COLOR_TEXT_DIM, COLOR_CARD_BG);
  tft.drawString("CURRENT SENSOR", cardX + 10, y4 + 8, 2);
  tft.setTextColor(TFT_WHITE, COLOR_CARD_BG);
  tft.drawString("mA", cardX + 250, y4 + 15, 2);

  // --- FOOTER BAR ---
  tft.fillRect(0, 290, 480, 30, COLOR_PANEL_BG);
  tft.drawFastHLine(0, 290, 480, COLOR_BORDER);
  tft.setTextColor(COLOR_TEXT_DIM, COLOR_PANEL_BG);
  tft.drawString("IP: " + WiFi.localIP().toString(), 30, 298, 2);

  // Status Badge WiFi Initial
  if (WiFi.status() == WL_CONNECTED) {
    tft.fillRoundRect(385, 6, 85, 22, 11, COLOR_GREEN_NEON);
    tft.setTextColor(TFT_BLACK, COLOR_GREEN_NEON);
    tft.drawString("ONLINE", 403, 10, 2);
  } else {
    tft.fillRoundRect(385, 6, 85, 22, 11, COLOR_RED_NEON);
    tft.setTextColor(TFT_WHITE, COLOR_RED_NEON);
    tft.drawString("OFFLINE", 398, 10, 2);
  }
}

// 2. PEMBARUAN DATA DINAMIS (ANTI KEDIP / FLICKER FREE)
void updateDynamicUI() {
  heartbeatDot = !heartbeatDot;

  // --- FOOTER HEARTBEAT ---
  if (heartbeatDot) {
    tft.fillCircle(15, 305, 4, COLOR_GREEN_NEON);
  } else {
    tft.fillCircle(15, 305, 4, COLOR_PANEL_BG);
  }

  // --- WAKTU DAN TANGGAL DI FOOTER ---
  struct tm timeinfo;
  if(getLocalTime(&timeinfo)) {
    char dateStr[12];
    char timeStr[10];
    strftime(dateStr, sizeof(dateStr), "%d/%m/%Y", &timeinfo);
    strftime(timeStr, sizeof(timeStr), "%H:%M:%S", &timeinfo);
    
    tft.setTextColor(COLOR_ACCENT_YLW, COLOR_PANEL_BG);
    tft.drawString(String(dateStr) + "  " + String(timeStr), 250, 298, 2);
  }

  // --- TANGKI AIR DIGITAL ---
  int tankX = 35;
  int tankY = 50;
  int tankW = 120;
  int tankH = 215;

  float percent = res1.cm / res1.maxH;
  int fillH = percent * tankH;

  // Hanya perbarui area air jika ada perubahan tinggi piksel
  if (fillH != lastFillH) {
    tft.fillRect(tankX, tankY, tankW, tankH - fillH, COLOR_CARD_BG);
    tft.fillRect(tankX, tankY + (tankH - fillH), tankW, fillH, COLOR_WATER_BLUE);
    tft.drawRoundRect(tankX, tankY, tankW, tankH, 4, COLOR_NEON_BLUE);
    
    // Redraw badge persentase
    tft.fillRoundRect(tankX + 15, tankY + (tankH / 2) - 18, 90, 32, 6, COLOR_PANEL_BG);
    tft.drawRoundRect(tankX + 15, tankY + (tankH / 2) - 18, 90, 32, 6, COLOR_BORDER);
    tft.setTextColor(COLOR_NEON_BLUE, COLOR_PANEL_BG);
    tft.setTextPadding(70);
    tft.drawFloat(percent * 100.0, 1, tankX + 22, tankY + (tankH / 2) - 12, 4);
    tft.setTextPadding(0);
    tft.drawString("%", tankX + 85, tankY + (tankH / 2) - 12, 2);

    lastFillH = fillH;
  }

  // --- METRIC CARDS ---
  int cardX = 170;
  int cardW = 295;

  // Kartu 1: Level Air
  int y1 = 48;
  tft.setTextColor(COLOR_NEON_BLUE, COLOR_CARD_BG);
  tft.setTextPadding(110);
  tft.drawFloat(res1.cm, 1, cardX + 130, y1 + 10, 4);
  tft.fillRect(cardX + 10, y1 + 38, cardW - 20, 4, COLOR_PANEL_BG);
  tft.fillRect(cardX + 10, y1 + 38, (cardW - 20) * percent, 4, COLOR_NEON_BLUE);

  // Kartu 2: Volume Air
  int y2 = 104;
  tft.setTextColor(COLOR_ACCENT_YLW, COLOR_CARD_BG);
  tft.setTextPadding(110);
  tft.drawFloat(res1.vol / 1000.0, 2, cardX + 130, y2 + 10, 4);
  tft.fillRect(cardX + 10, y2 + 38, cardW - 20, 4, COLOR_PANEL_BG);
  tft.fillRect(cardX + 10, y2 + 38, (cardW - 20) * percent, 4, COLOR_ACCENT_YLW);

  // Kartu 3: Flow Rate
  int y3 = 160;
  tft.setTextColor(COLOR_GREEN_NEON, COLOR_CARD_BG);
  tft.setTextPadding(110);
  tft.drawFloat(res1.flow, 1, cardX + 130, y3 + 10, 4);

  // Kartu 4: Current 4-20mA
  int y4 = 216;
  tft.setTextColor(COLOR_NEON_BLUE, COLOR_CARD_BG);
  tft.setTextPadding(110);
  tft.drawFloat(res1.ma, 2, cardX + 130, y4 + 10, 4);
  tft.setTextPadding(0);
}

void sendDataToClients() {
  if (ws.count() == 0) return;

  StaticJsonDocument<1024> doc;
  JsonObject sys = doc.createNestedObject("sys");
  sys["ram_total"] = ESP.getHeapSize();
  sys["ram_free"]  = ESP.getFreeHeap();
  sys["flash_total"] = ESP.getFlashChipSize();
  sys["flash_used"]  = ESP.getSketchSize();

  JsonArray tankArray = doc.createNestedArray("tanks");
  JsonObject t = tankArray.createNestedObject();
  t["name"] = res1.name;
  t["capacity"] = res1.capacity;
  t["maxH"] = res1.maxH;
  t["cm"] = res1.cm;
  t["vol"] = res1.vol;
  t["flow"] = res1.flow;
  t["ma"] = res1.ma;

  String output;
  serializeJson(doc, output);
  ws.textAll(output);
}

void setup() {
  Serial.begin(115200);

  tft.init();
  tft.setRotation(1); // Landscape Mode
  tft.fillScreen(COLOR_DARK_BG);
  
  // Tampilan awal saat koneksi WiFi
  tft.fillRect(80, 100, 320, 120, COLOR_PANEL_BG);
  tft.drawRoundRect(80, 100, 320, 120, 8, COLOR_NEON_BLUE);
  tft.setTextColor(COLOR_NEON_BLUE, COLOR_PANEL_BG);
  tft.drawString("AKND TECH. SCADA", 100, 120, 4);
  tft.setTextColor(COLOR_TEXT_DIM, COLOR_PANEL_BG);
  tft.drawString("Connecting WiFi & Sync Time...", 95, 160, 2);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }

  // Sinkronisasi Waktu Real-Time (NTP Server)
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);

  // Digambar sekali saja saat booting
  drawStaticUI();
  updateDynamicUI();

  ws.onEvent([](AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len){
    if (type == WS_EVT_CONNECT) sendDataToClients();
  });
  
  server.addHandler(&ws);
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/html", index_html);
  });

  server.begin();
}

void loop() {
  ws.cleanupClients();

  if (millis() - lastUpdate >= 1000) {
    lastUpdate = millis();
    updateSensorData();
    sendDataToClients();
    updateDynamicUI();
  }
}