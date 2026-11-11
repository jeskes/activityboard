#pragma once

#include <MFRC522.h>

#include "modules/coreclient.h"

class NfcController {
 public:
  NfcController(ActivityCoreClient& core);

  void setup();
  void loop();

 private:
  MFRC522 mfrc522;
  ActivityCoreClient& core;

  byte lastUID[20] = {0};
  byte lastLength = 0;
  unsigned long lastCheckTime;

  void checkCard();
};
