#pragma once

#include "periphericalclient.h"

class CoreStatusHandler {
 public:
  virtual bool handleCoreStatus(CoreStatus status) = 0;
};

class ActivityCoreClient : public PeriphericalClient {
 public:
  ActivityCoreClient(ClientId clientId, Scheduler& scheduler);

  void setup();
  void init(BitmapId startupBitmap);

  void putParam(const char* name, uint16_t value);
  void putParam(const char* name, const char* value);

  void playSound(TrackId track);
  void playSound(TrackFolder folder, TrackId track);
  void stopSound();

  void clearDisplay();
  void displayMessage(MessageId messageId);
  void displayBitmap(BitmapId bitmapId);
  void displayMenu(MenuId menuId, uint8_t selectedIndex);

  void snoozeStatus(unsigned long delay);
  void setStatusHandler(CoreStatusHandler* handler);

  bool sendRequest(ModuleId module, BaseRequest* request, uint8_t length, unsigned long snoozeStatusDelay);
};
