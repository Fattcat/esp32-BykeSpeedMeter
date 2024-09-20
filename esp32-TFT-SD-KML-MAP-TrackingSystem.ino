#include <TFT_eSPI.h>  // Knižnica pre ovládanie TFT displeja
#include <SPI.h>       // Knižnica pre SPI komunikáciu (TFT a SD karta)
#include <SD.h>        // Knižnica pre SD kartu
#include <TinyGPS++.h> // Knižnica pre GPS modul

// CONNECTION
// TFT Display to esp32

// VCC -> 3.3V
// GND -> GND
// SCL (or SCK) -> GPIO18
// SDA -> GPIO23 
// RES -> GPIO4
// DC -> GPIO2
// CS -> GPIO15

// GPS neo 6m Module to esp32 

// VCC -> 3.3V
// GND -> GND
// RX -> GPIO17
// TX -> GPIO16

// SD Card to ESP32

// VCC -> 3.3V
// GND -> GND
// CS -> GPIO5
// SCK -> GPIO18
// MOSI -> GPIO23
// MISO -> GPIO19 

TFT_eSPI tft = TFT_eSPI();  // Inštancia TFT displeja
TinyGPSPlus gps;            // Inštancia GPS knižnice
HardwareSerial SerialGPS(1); // Definícia UART1 pre komunikáciu s GPS

void setup() {
  Serial.begin(115200); // Seriová komunikácia pre ladenie

  // GPS inicializácia: NEO-6M je pripojený k UART1 (Rx na GPIO16, Tx na GPIO17)
  SerialGPS.begin(9600, SERIAL_8N1, 16, 17);  // Rx = GPIO16, Tx = GPIO17

  // Inicializácia TFT displeja
  tft.init();              // Spustenie displeja
  tft.setRotation(1);      // Nastavenie orientácie displeja (1 = Landscape)
  tft.fillScreen(TFT_BLACK);  // Vyčistenie displeja na čiernu

  // Inicializácia SD karty na pin CS = GPIO5
  if (!SD.begin(5)) {  // CS pin SD karty je pripojený na GPIO5
    Serial.println("SD Card Mount Failed");  // Chyba, ak sa nepodarí načítať SD kartu
    return;
  }

  // Otvorenie súboru mapy na SD karte
  File mapFile = SD.open("/map.kml");
  if (mapFile) {
    // Proces načítania a vykreslenia mapy
    // Na základe obsahu KML súboru treba vykresliť body a trasy
    mapFile.close();
  } else {
    Serial.println("Map file not found");  // Ak súbor mapy nie je nájdený
  }
}

void loop() {
  // Spracovanie GPS dát
  while (SerialGPS.available() > 0) {
    gps.encode(SerialGPS.read());  // Dekódovanie GPS dát
  }

  if (gps.location.isUpdated()) {
    // Získanie aktuálnej polohy z GPS
    double latitude = gps.location.lat();   // Zemepisná šírka
    double longitude = gps.location.lng();  // Zemepisná dĺžka
    
    // Vypočítajte pozíciu na mape na základe GPS súradníc
    int x = map(longitude, MIN_LNG, MAX_LNG, 0, tft.width());  // Mapovanie dĺžky na šírku displeja
    int y = map(latitude, MIN_LAT, MAX_LAT, tft.height(), 0);  // Mapovanie šírky na výšku displeja

    // Znova vykreslite mapu a pozíciu
    tft.fillScreen(TFT_BLACK);  // Vymaže displej pred kreslením novej polohy
    tft.drawCircle(x, y, 5, TFT_RED);  // Bod pre aktuálnu pozíciu
  }

  delay(10000);  // Refresh každých 10 sekúnd
}
