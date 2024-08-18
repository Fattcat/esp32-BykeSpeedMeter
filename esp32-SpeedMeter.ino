#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <TinyGPSPlus.h>
#include <HardwareSerial.h>

// Nastavení pro OLED displej 128x64
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

// Nastavení pro I2C
#define OLED_RESET -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Nastavení pro GPS modul
HardwareSerial gpsSerial(2);  // Použití UART2 na ESP32 (RX2/TX2)
TinyGPSPlus gps;

void setup() {
  Serial.begin(115200);
  gpsSerial.begin(9600, SERIAL_8N1, 16, 17);  // RX=GPIO16, TX=GPIO17

  // Inicializace OLED displeje
  if (!display.begin(SSD1306_PAGEADDR, 0x3C)) {
    Serial.println(F("OLED init failed"));
    for (;;)
      ;
  }

  // Vyčištění obrazovky a nastavení výchozího textu
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);
  display.setTextSize(1);

  // Úvodní obrazovka
  display.setCursor(0, 0);
  display.println("    GPS Tachometer");
  display.display();
  delay(2000);
  display.clearDisplay();  // Vyčištění obrazovky před zobrazením dat
}

void loop() {
  // Zpracování GPS dat
  while (gpsSerial.available() > 0) {
    gps.encode(gpsSerial.read());
  }

  if (gps.location.isUpdated()) {
    // Vymazání obrazovky před dalším vykreslením
    display.clearDisplay();

    // Získání rychlosti
    float speedKmh = gps.speed.kmph();

    // Získání nadmořské výšky
    float altitude = gps.altitude.meters();

    // Získání zeměpisné šířky a délky
    double latitude = gps.location.lat();
    double longitude = gps.location.lng();

    // Zobrazení na displeji
    display.setCursor(0, 0);
    display.setTextSize(1);
    display.print("Speed: ");
    display.setTextSize(2);
    display.println(String(speedKmh, 2) + " km/h");

    Serial.println("Speed: ");
    Serial.println(String(speedKmh, 2) + " km/h");




    display.setTextSize(1);
    display.print("Altitude: ");
    display.setTextSize(2);
    display.println(String(altitude, 2) + " m");

    Serial.println("Altitude: ");
    Serial.println(String(altitude, 2) + " m");



    display.setTextSize(1);
    display.print("Latitude: ");
    display.println(String(latitude, 6));
    Serial.println("Latitude: ");
    Serial.println(String(latitude, 6));



    display.setTextSize(1);
    display.print("Longitude: ");
    display.println(String(longitude, 6));
    
    Serial.println("Longitude: ");
    Serial.println(String(longitude, 6));


    // Aktualizace displeje
    display.display();
  }

  delay(1000);  // Aktualizace každou sekundu
}
