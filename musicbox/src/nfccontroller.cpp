#include "nfccontroller.h"

#include <base/base.h>

#include "const.h"

NfcController::NfcController(ActivityCoreClient& core)
    : core(core),
      mfrc522(RC522_SS, RC522_RST) {
}

void NfcController::setup() {
  LOG("initialize mfrc522...");
  mfrc522.PCD_Init();
  delay(4);
  mfrc522.PCD_DumpVersionToSerial();
  bool ok = mfrc522.PCD_PerformSelfTest();
  Serial.print("Hardware-Selbsttest: ");
  Serial.println(ok ? "BESTANDEN (OK)" : "FEHLGESCHLAGEN");

  LOG("nfc reader initialized");
}

void NfcController::loop() {
  checkCard();
}

void NfcController::checkCard() {
  if (millis() - lastCheckTime > 250) {
    lastCheckTime = millis();

    // checking twice : a card might just respond to second call
    bool cardPresent = mfrc522.PICC_IsNewCardPresent() || mfrc522.PICC_IsNewCardPresent();

    if (cardPresent && mfrc522.PICC_ReadCardSerial()) {
      missingCount = 0;

      if (mfrc522.uid.size != currentUIDLength || memcmp(mfrc522.uid.uidByte, currentUID, mfrc522.uid.size) != 0) {
        currentUIDLength = mfrc522.uid.size;
        memcpy(currentUID, mfrc522.uid.uidByte, mfrc522.uid.size);

        char uid[2 * currentUIDLength + 1] = {0};
        activityLogger.tohex(currentUID, currentUIDLength, uid);
        LOG("found new card: uid=%s", uid);

        core.playMusic(uid);
        digitalWrite(INDICATOR_PLAYING, HIGH);
      }

      mfrc522.PCD_StopCrypto1();
    }
    else {
      if (currentUIDLength > 0) {
        missingCount++;
        if (missingCount >= 3) {
          LOG("card removed");
          // core.stopSound();
          currentUIDLength = 0;
          memset(currentUID, 0, sizeof(currentUID));
          missingCount = 0;
        }
      }
    }
  }
}