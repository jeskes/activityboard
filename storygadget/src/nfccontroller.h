#pragma once

#include <MFRC522.h>
#include <modules/coreclient.h>

class NfcEventHandler {
 public:
  virtual void onNewRfidCard(RfidId rfid) = 0;
  virtual void onRfidCardRemoved() = 0;
};

class NfcController {
 public:
  NfcController(ActivityCoreClient& core);

  void setup();
  void loop();

  void setHandler(NfcEventHandler* handler);

 private:
  ActivityCoreClient& core;
  MFRC522 mfrc522;

  byte currentUID[20] = {0};
  byte currentUIDLength = 0;

  void checkCard();
  uint8_t missingCount;
  unsigned long lastCheckTime;

  NfcEventHandler* handler;
};
