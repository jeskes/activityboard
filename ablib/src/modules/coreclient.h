#pragma once

#include "periphericalclient.h"

class CoreStatusHandler {
 public:
  virtual bool handleCoreStatus(CoreStatus status) = 0;
};

class DataStream {
 public:
  DataStream();

  int read();
  void close();

 private:
  int bufferIndex;
  uint8_t buffer[REQUEST_BUFFER_SIZE];
};

class ActivityCoreClient : public PeriphericalClient {
 public:
  ActivityCoreClient(ClientId clientId, Scheduler& scheduler);

  void setup();
  void init(BitmapId startupBitmap);

  void putParam(const char* name, uint16_t value);
  void putParam(const char* name, const char* value);

  void playSound(TrackId track);
  void stopSound();

  void clearDisplay();
  void displayMessage(MessageId messageId);
  void displayBitmap(BitmapId bitmapId);
  void displayMenu(MenuId menuId, uint8_t selectedIndex);

  DataStream readData(DataId id);

  void snoozeStatus(unsigned long delay);
  void setStatusHandler(CoreStatusHandler* handler);

  bool sendRequest(ModuleId module, BaseRequest* request, uint8_t length, unsigned long snoozeStatusDelay);
};
