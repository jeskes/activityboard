#pragma once

#include <MFRC522.h>
#include <modules/coreclient.h>

#include "const.h"

class NfcController {
 public:
  NfcController(ActivityCoreClient& core);

  void setup();
  RfidId loop();

 private:
  ActivityCoreClient& core;
  MFRC522 mfrc522;

  byte currentUID[MAX_RFID_UID_SIZE];
  byte currentUIDLength;

  uint8_t missingCount;
  unsigned long lastCheckTime;

  char uid[2 * MAX_RFID_UID_SIZE + 1];
};
