#include "nfccontroller.h"

#include <base/base.h>

#include "const.h"

NfcController::NfcController(ActivityCoreClient& core)
    : core(core),
      mfrc522(RC522_SS, RC522_RST),
      currentUID{0},
      currentUIDLength(0),
      uid{0} {
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

RfidId NfcController::loop() {
  RfidId rfidId = nullptr;

  if (millis() - lastCheckTime > 250) {
    lastCheckTime = millis();

    // checking twice : a card might just respond to second call
    bool cardPresent = mfrc522.PICC_IsNewCardPresent() || mfrc522.PICC_IsNewCardPresent();

    if (cardPresent && mfrc522.PICC_ReadCardSerial()) {
      missingCount = 0;

      if (mfrc522.uid.size != currentUIDLength || memcmp(mfrc522.uid.uidByte, currentUID, mfrc522.uid.size) != 0) {
        currentUIDLength = mfrc522.uid.size;
        memcpy(currentUID, mfrc522.uid.uidByte, mfrc522.uid.size);
        activityLogger.tohex(currentUID, currentUIDLength, uid);
        LOG("found new card: uid=%s", uid);
        rfidId = uid;
      }

      mfrc522.PCD_StopCrypto1();
    }
    else {
      if (currentUIDLength > 0) {
        missingCount++;
        if (missingCount >= 3) {
          LOG("card removed");
          currentUIDLength = 0;
          memset(currentUID, 0, sizeof(currentUID));
          missingCount = 0;
        }
      }
    }
  }

  return rfidId;
}
