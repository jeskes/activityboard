#pragma once

#include <MFRC522.h>

#include "modules/coreclient.h"

class NfcController {
 public:
  NfcController(ActivityCoreClient& core);

  void setup();
  void loop();

 private:
  ActivityCoreClient& core;
  MFRC522 mfrc522;

  byte currentUID[20] = {0};
  byte currentUIDLength = 0;

  void checkCard();
  uint8_t missingCount;
  unsigned long lastCheckTime;
};
