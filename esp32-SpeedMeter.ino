#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <TinyGPSPlus.h>
#include <HardwareSerial.h>
#include "bitmaps.h"  // Vloženie bitmap knižnice

// Nastavenie pre OLED displej 128x64
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

// Nastavenie pre I2C
#define OLED_RESET -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Nastavenie pre GPS modul
HardwareSerial gpsSerial(2);  // Použitie UART2 na ESP32 (RX2/TX2)
TinyGPSPlus gps;

bool satelliteConnected = false;  // Premenná na sledovanie stavu pripojenia

void setup() {
  Serial.begin(115200);
  gpsSerial.begin(9600, SERIAL_8N1, 16, 17);  // RX=GPIO16, TX=GPIO17

  // Inicializácia OLED displeja
  if (!display.begin(SSD1306_PAGEADDR, 0x3C)) {
    Serial.println(F("OLED init failed"));
    for (;;);
  }

  // Vyčistenie obrazovky a nastavenie východzieho textu
  display.clearDisplay();
  display.setTextWrap(1);
  display.setTextColor(SSD1306_WHITE);
  display.setTextSize(1);

  // Zobrazenie textu "Hladam Satelity"
  display.setCursor(0, 0);
  display.println("Hladam Satelity");
  display.display();
}

void loop() {
  // Spracovanie GPS dát
  while (gpsSerial.available() > 0) {
    gps.encode(gpsSerial.read());
  }

  if (!gps.location.isValid()) {
    // Animácia "Hladam Satelity..."
    satelliteConnected = false;  // Resetovať stav pripojenia, ak nie je signál
    for (uint8_t i = 0; i < 3; i++) {
      display.setCursor(0 + (i * 6), 10); // Nastaví kurzor pre každú bodku o 6 pixelov doprava
      display.println(".");
      display.display();
      delay(700); // Časovač pre zobrazenie jednotlivých bodiek (0,7 sekundy)
    }
    delay(700); // Pauza pred vymazaním všetkých bodiek
    display.fillRect(0, 10, SCREEN_WIDTH, 8, SSD1306_BLACK);  // Vymaže staré bodky
    display.display();
  } else if (!satelliteConnected) {
    // Akonáhle je pripojený k satelitu a ešte nebol zobrazený text
    satelliteConnected = true;
    display.clearDisplay();
    display.setCursor(0, 0);
    display.setTextSize(1);
    display.println("Satelit pripojeny!");
    Serial.println("Satelit pripojeny!");
    display.display();
    delay(2000);

    // Získanie rýchlosti a výšky
    float speedKmh = gps.speed.kmph();
    float altitude = gps.altitude.meters();

    // Zobrazenie rýchlosti a výšky
    display.clearDisplay();
    display.drawBitmap(0, 10, speedbitmap, 19, 19, SSD1306_WHITE);  // Zobrazenie ikony rýchlosti
    display.setCursor(20, 10);
    display.setTextSize(2);
    display.println(String(speedKmh, 2) + "km/h");
    Serial.println(String(speedKmh, 2) + "km/h");

    display.drawBitmap(0, 40, vyskabitmap, 20, 20, SSD1306_WHITE);  // Zobrazenie ikony výšky
    display.setCursor(20, 40);
    display.setTextSize(2);
    display.println(String(altitude, 2) + "m");
    Serial.println(String(altitude, 2) + "m");
    display.display();
  } else {
    // Získanie rýchlosti a výšky a pravidelné zobrazovanie
    float speedKmh = gps.speed.kmph();
    float altitude = gps.altitude.meters();

    // Zobrazenie rýchlosti a výšky
    display.clearDisplay();
    display.drawBitmap(0, 10, speedbitmap, 19, 19, SSD1306_WHITE);  // Zobrazenie ikony rýchlosti
    display.setCursor(20, 10);
    display.setTextSize(2);
    display.println(String(speedKmh, 2) + "km/h");
    Serial.println(String(speedKmh, 2) + "km/h");

    display.drawBitmap(0, 40, vyskabitmap, 20, 20, SSD1306_WHITE);  // Zobrazenie ikony výšky
    display.setCursor(20, 40);
    display.setTextSize(2);
    display.println(String(altitude, 2) + " m");
    Serial.println(String(altitude, 2) + " m");
    display.display();
  }

  delay(700); // Aktualizácia každých 0.7 sekundy
}
