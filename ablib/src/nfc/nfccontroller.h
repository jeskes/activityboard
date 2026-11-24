#pragma once
#ifdef AB_NFCCONTROLLER

#include <MFRC522.h>
#include <modules/coreclient.h>

class NfcController {
 public:
  NfcController(uint8_t ss, uint8_t rst);

  bool setup();
  RfidId loop();

 private:
  MFRC522 mfrc522;

  byte currentUID[MAX_RFID_UID_SIZE];
  byte currentUIDLength;

  uint8_t missingCount;
  unsigned long lastCheckTime;

  char uid[2 * MAX_RFID_UID_SIZE + 1];
};
#endif