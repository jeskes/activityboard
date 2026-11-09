#include <SPI.h>
#include <MFRC522.h>
#include <Wire.h>

#define RST_PIN   9   // Reset-Pin für das RC522 Modul
#define SS_PIN    10  // Slave Select (CS) Pin für das RC522 Modul
#define ESP32_I2C_ADDR 0x55 // I2C-Adresse deines ESP32-Slaves

MFRC522 mfrc522(SS_PIN, RST_PIN);

byte lastUID[10] = {0};
byte lastLength = 0;
unsigned long lastCheckTime = 0;
bool cardPresent = false;

void setup() {
  Serial.begin(115200);
  Wire.begin();        // Nano startet als I2C-Master
  SPI.begin();         // Startet den SPI-Bus zum RC522
  mfrc522.PCD_Init();  // Initialisiert das RC522 Modul
  Serial.println("Nano NFC-Bruecke bereit...");
}

void sendIDToESP32(byte* uidBytes, byte length) {
  Wire.beginTransmission(ESP32_I2C_ADDR);
  Wire.write(uidBytes, length);
  Wire.endTransmission();
}

void loop() {
  // Alle 150ms prüfen, um den Bus nicht zu überlasten
  if (millis() - lastCheckTime > 150) {
    lastCheckTime = millis();
    
    // Versuchen, eine Karte zu lesen
    bool success = mfrc522.PICC_IsNewCardPresent() && mfrc522.PICC_ReadCardSerial();
    
    if (success) {
      // Überprüfen, ob es eine neue Karte ist (verhindert Dauerschleife beim Senden)
      if (mfrc522.uid.size != lastLength || memcmp(mfrc522.uid.uidByte, lastUID, mfrc522.uid.size) != 0) {
        
        memcpy(lastUID, mfrc522.uid.uidByte, mfrc522.uid.size);
        lastLength = mfrc522.uid.size;
        cardPresent = true;

        Serial.print("Neue Karte erkannt! Sende an ESP32... ID: ");
        sendIDToESP32(mfrc522.uid.uidByte, mfrc522.uid.size);
      }
      
      // Karte nach dem Lesen in den Haltemodus versetzen
      mfrc522.PICC_HaltA();
    } else {
      // Wenn vorher eine Karte da war und jetzt weg ist -> Stopp-Signal senden (0xFF)
      if (cardPresent) {
        Serial.println("Karte entfernt! Sende STOPP an ESP32.");
        byte stopSignal[] = {0xFF};
        sendIDToESP32(stopSignal, 1);
        
        cardPresent = false;
        lastLength = 0;
        memset(lastUID, 0, sizeof(lastUID));
      }
    }
  }
}
