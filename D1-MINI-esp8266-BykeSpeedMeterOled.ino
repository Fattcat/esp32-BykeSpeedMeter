// Code for D1 MINI ESP8266

// ------- CONNECTION -------

// Oled --> D1 Mini
// VCC - 3.3V
// GND - GND
// SCL - D1
// SDA - D2

// GPS NEO 6M --> D1 Mini
// VCC - 3.3V
// GND - GND
// TX - RX 
// RX - TX

// ------- CONNECTION -------

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <TinyGPSPlus.h>
#include "bitmaps.h"  // Vloženie bitmap knižnice

// Nastavenie pre OLED displej 128x64
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

// Nastavenie pre I2C
#define OLED_RESET -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Nastavenie pre GPS modul
TinyGPSPlus gps;

// Definovanie pinov pre GPS modul
#define GPS_RX_PIN RX  // GPIO3
#define GPS_TX_PIN TX  // GPIO1

void setup() {
  Serial.begin(115200);  // Hlavný sériový port na komunikáciu s PC
  Serial.swap(); // Swap TX a RX na pinoch D1 Mini

  // Inicializácia OLED displeja
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("OLED init failed"));
    for (;;);
  }

  // Vyčistenie obrazovky a nastavenie východzieho textu
  display.clearDisplay();
  display.setTextWrap(1);
  display.setTextColor(SSD1306_WHITE);
  display.setTextSize(1);

  // ODPOČÍTAVANIE OD ČÍSLA 3 PO ČÍSLO 1
  delay(2000);
  display.drawBitmap(0, 0, cislotri, 128, 64, SSD1306_WHITE);
  display.display();
  delay(1000);
  display.clearDisplay();

  display.drawBitmap(0, 0, cislodva, 128, 64, SSD1306_WHITE);
  display.display();
  delay(1000);
  display.clearDisplay();

  display.drawBitmap(0, 0, cislojeden, 128, 64, SSD1306_WHITE);
  display.display();
  delay(1000);
  display.clearDisplay();

  // Zobrazenie "firstlogobitmap"
  delay(200);
  display.drawBitmap(0, 0, logobitmap, 128, 64, SSD1306_WHITE);
  display.display();
  delay(3000);

  // Vyčistenie po zobrazení "firstlogobitmap"
  display.clearDisplay();

  // Zobrazenie textu "Hľadám Satelity"
  display.setCursor(0, 0);
  display.println("Hladam Satelity");
  Serial.println("Hladam Satelity");
  display.display();
}

void loop() {
  // Spracovanie GPS dát
  while (Serial.available() > 0) {
    gps.encode(Serial.read());
  }

  if (!gps.location.isValid()) {
    // Animácia "Hľadám Satelity..."
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
  }

  // Získanie rýchlosti, výšky a času
  float speedKmh = gps.speed.kmph();
  float altitude = gps.altitude.meters();
  int hour = gps.time.isValid() ? (gps.time.hour() + 2) % 24 : 0;  // Pridáme 2 hodiny pre časovú zónu Slovenska (UTC+2)
  int minute = gps.time.isValid() ? gps.time.minute() : 0;

  // Zobrazenie rýchlosti a výšky
  display.clearDisplay();

  // Rýchlosť
  display.drawBitmap(0, 0, speedbitmap, 19, 19, SSD1306_WHITE);
  display.setCursor(20, 0);
  display.setTextSize(2);
  display.println(String(speedKmh, 2) + "km/h");

  Serial.println(String(speedKmh, 2) + "km/h");

  // Výška
  display.drawBitmap(0, 43, vyskabitmap, 20, 20, SSD1306_WHITE);
  display.setCursor(25, 50);
  display.setTextSize(2);
  display.println(String(altitude, 2) + "m");

  Serial.println(String(altitude, 2) + "m");

  // Zobrazenie času len ak sa zmení minúta
  if (minute != lastMinute) {
    // Vymaže starý čas
    display.fillRect(80, 32, 48, 16, SSD1306_BLACK);  
    lastMinute = minute;
  }

  // Vždy zobrazí aktuálny čas
  displayTime(hour, minute);

  display.display();

  delay(700); // Aktualizácia každých 0.7 sekundy
}

void displayTime(int hour, int minute) {
  display.setCursor(0, 24);  // Nastavenie kurzora na pravú stranu v strede
  display.setTextSize(2);
  display.print("Cas:");

  // Zobrazenie s vedúcimi nulami, ak je to potrebné
  if (hour < 10) display.print("0");
  display.print(hour);
  display.print(":");
  if (minute < 10) display.print("0");
  display.print(minute);
  Serial.println("Aktuálny Čas: " + String(hour) + ":" + String(minute));
}