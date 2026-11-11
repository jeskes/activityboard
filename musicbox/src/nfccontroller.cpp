#include "nfccontroller.h"

#include "const.h"

NfcController::NfcController(ActivityCoreClient& core)
    : core(core),
      mfrc522(RC522_SS, RC522_RST) {
}

void NfcController::setup() {
  mfrc522.PCD_Init();
}

void NfcController::loop() {
  checkCard();
}

void NfcController::checkCard() {
  // Alle 150ms prüfen, um den Bus nicht zu überlasten
  if (millis() - lastCheckTime > 15000) {
    lastCheckTime = millis();
    core.playMusic("04A2B3C4D5E6F7");
	
    /*
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
    }
    else {
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
    */
  }
}
