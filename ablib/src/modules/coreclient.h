#pragma once

#include "periphericalclient.h"

class CoreStatusHandler {
 public:
  virtual void handleCoreStatus(CoreStatus status) = 0;
};

class DataStream {
 public:
  DataStream(ClientId client);

  int read();
  void close();

 private:
  ClientId client;
  int bufferIndex;
  uint8_t buffer[REQUEST_BUFFER_SIZE];
};

class ActivityCoreClient : public PeriphericalClient {
 public:
  ActivityCoreClient(ClientId clientId);

  void setup(uint8_t irq);
  void loop();

  void init(ImageId startupImage);

  void putParam(ParameterId id, uint16_t value);
  void putParam(ParameterId id, const char* value);

  void playMusic(MusicId id);

  void playSound(TrackId id);
  void playSound(TrackFolder folder, TrackId id);
  void stopSound();

  void clearDisplay();
  void displayMessage(MessageId id);
  void displayImage(ImageId id);
  void displayMenu(MenuId id, uint8_t selectedIndex);

  DataStream readData(DataId id);

  void setStatusHandler(CoreStatusHandler* handler);

 private:
  void requestCoreStatus(CoreStatus& status);

  unsigned long heartbeat;
  unsigned long statusBlocked;
  void processAvailableStatus();
  void blockStatusProcessing(unsigned long timeout);
  
  CoreStatusHandler* statusHandler;
};
