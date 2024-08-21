#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ST7789.h>
#include <TinyGPSPlus.h>
#include <HardwareSerial.h>
#include "bitmaps.h"  // Vloženie bitmap knižnice

// DEFINOVANIE TFT PINOV na ESP32

#define TFT_CS     15  // Pin CS
#define TFT_RST    4   // Pin RST
#define TFT_DC     2   // Pin DC
#define TFT_MOSI   23  // Pin MOSI
#define TFT_SCK    18  // Pin SCK

// Inicializácia ST7789 displeja s rozlíšením 240x320
Adafruit_ST7789 tft = Adafruit_ST7789(TFT_CS, TFT_DC, TFT_RST);

// Nastavenie pre GPS modul
HardwareSerial gpsSerial(2);  // Použitie UART2 na ESP32 (RX2/TX2)
TinyGPSPlus gps;

bool satelliteConnected = false;  // Premenná na sledovanie stavu pripojenia
int lastMinute = -1;  // Premenná na sledovanie poslednej zobrazenej minúty

void setup() {
  Serial.begin(115200);
  gpsSerial.begin(9600, SERIAL_8N1, 16, 17);  // RX=GPIO16, TX=GPIO17

  // Inicializácia TFT displeja
  tft.init(240, 320);           // Rozlíšenie 240x320
  tft.setRotation(1);           // Otočenie displeja (1 = landscape orientácia)
  tft.fillScreen(ST77XX_BLACK); // Vyplnenie obrazovky čiernou farbou
  
  // Nastavenie textových vlastností
  tft.setTextWrap(true);
  tft.setTextColor(ST77XX_WHITE);
  tft.setTextSize(2);

  // Odpočítavanie od 3 do 1
  delay(2000);
  tft.drawBitmap(0, 0, cislotri, 240, 320, ST77XX_WHITE);
  delay(1000);
  tft.fillScreen(ST77XX_BLACK);

  tft.drawBitmap(0, 0, cislodva, 240, 320, ST77XX_WHITE);
  delay(1000);
  tft.fillScreen(ST77XX_BLACK);

  tft.drawBitmap(0, 0, cislojeden, 240, 320, ST77XX_WHITE);
  delay(1000);
  tft.fillScreen(ST77XX_BLACK);

  // Zobrazenie "firstlogobitmap"
  delay(200);
  tft.drawBitmap(0, 0, logobitmap, 240, 320, ST77XX_WHITE);
  delay(3000);
  tft.fillScreen(ST77XX_BLACK);

  // Zobrazenie textu "Hladam Satelity"
  tft.setCursor(0, 0);
  tft.println("Hladam Satelity");
  Serial.println("Hladam Satelity");
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
      tft.setCursor(0 + (i * 30), 100); // Nastaví kurzor pre každú bodku o 30 pixelov doprava
      tft.print(".");
      delay(700); // Časovač pre zobrazenie jednotlivých bodiek (0,7 sekundy)
    }
    delay(700); // Pauza pred vymazaním všetkých bodiek
    tft.fillRect(0, 100, 240, 24, ST77XX_BLACK);  // Vymaže staré bodky
  } else if (!satelliteConnected) {
    // Akonáhle je pripojený k satelitu a ešte nebol zobrazený text
    satelliteConnected = true;
    tft.fillScreen(ST77XX_BLACK);
    tft.setCursor(0, 0);
    tft.setTextSize(2);
    tft.println("Satelit pripojeny!");
    Serial.println("Satelit pripojeny!");
    delay(2000);
  }

  // Získanie rýchlosti, výšky a času
  float speedKmh = gps.speed.kmph();
  float altitude = gps.altitude.meters();
  int hour = gps.time.isValid() ? (gps.time.hour() + 2) % 24 : 0;  // Pridáme 2 hodiny pre časovú zónu Slovenska (UTC+2)
  int minute = gps.time.isValid() ? gps.time.minute() : 0;

  // Zobrazenie rýchlosti a výšky
  tft.fillScreen(ST77XX_BLACK);
  
  // Rýchlosť
  tft.drawBitmap(0, 0, speedbitmap, 38, 38, ST77XX_WHITE);
  tft.setCursor(50, 0);
  tft.setTextSize(3);
  tft.println(String(speedKmh, 2) + " km/h");

  Serial.println(String(speedKmh, 2) + " km/h");

  // Výška
  tft.drawBitmap(0, 160, vyskabitmap, 40, 40, ST77XX_WHITE);
  tft.setCursor(50, 180);
  tft.setTextSize(3);
  tft.println(String(altitude, 2) + " m");

  Serial.println(String(altitude, 2) + " m");


  // TOTO JE PRIDANE LEN PRE ZOBRAZENIE V SERIAL MONITORE
  Serial.print("Sat: ");
  Serial.println(gps.satellites.value());
  Serial.print("Lat: ");
  Serial.println(gps.location.lat(), 6);
  Serial.print("Lng: ");
  Serial.println(gps.location.lng(), 6);
  Serial.print("Alt(m): ");
  Serial.println(gps.altitude.meters(), 1);
  // TOTO JE PRIDANE LEN PRE ZOBRAZENIE V SERIAL MONITORE


  // Zobrazenie času len ak sa zmení minúta
  if (minute != lastMinute) {
    // Vymaže starý čas
    tft.fillRect(180, 120, 60, 40, ST77XX_BLACK);  
    lastMinute = minute;
  }

  // Vždy zobrazí aktuálny čas
  displayTime(hour, minute);

  delay(700); // Aktualizácia každých 0.7 sekundy
}

void displayTime(int hour, int minute) {
  tft.setCursor(0, 120);  // Nastavenie kurzora na pravú stranu v strede
  tft.setTextSize(3);
  tft.print("Cas:");
    
  // Zobrazenie s vedúcimi nulami, ak je to potrebné
  if (hour < 10) tft.print("0");
  tft.print(hour);
  tft.print(":");
  if (minute < 10) tft.print("0");
  tft.print(minute);
  Serial.println("Aktualny Cas: " + String(hour) + ":" + String(minute));
}
