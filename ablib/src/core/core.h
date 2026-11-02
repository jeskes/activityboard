#pragma once

#include "../modules/modules.h"

/* ========================================================================= */
/*                                                        core general types */
/* ========================================================================= */

const ClientId SYSTEM_CLIENT_ID = 10;

using ParameterId = const char*;

using MessageId = const char*;
using BitmapId = const char*;
using MenuId = const char*;
using TrackId = const char*;
using DataId = const char*;

/* ========================================================================= */
/*                                                        core request types */
/* ========================================================================= */

struct __attribute__((packed)) InitCoreRequest : public BaseRequest {
  InitCoreRequest(ClientId client, BitmapId id)
      : BaseRequest(client, RequestType::INIT) {
    strlcpy(startupBitmap, id, ID_BUFFER_SIZE);
  }
  char startupBitmap[ID_BUFFER_SIZE];
};

struct __attribute__((packed)) ClearDisplayRequest : public BaseRequest {
  ClearDisplayRequest(ClientId client)
      : BaseRequest(client, RequestType::CLEAR_DISPLAY) {
  }
};

/* 32 - 1 (client) - 2 (request-type) - 10 (id-length) - 1 (append-flag) - 2 terminating 0 */

#define DISPLAY_PARAM_VALUE_BUFFER_SIZE 16

struct __attribute__((packed)) ParameterRequest : public BaseRequest {
  ParameterRequest(ClientId client, ParameterId id, const char* value, bool append)
      : BaseRequest(client, RequestType::PARAMETER) {
    strlcpy(this->id, id, PARAM_BUFFER_SIZE);
    strlcpy(this->value, value, DISPLAY_PARAM_VALUE_BUFFER_SIZE);
    this->append = append;
  }
  char id[PARAM_BUFFER_SIZE];
  uint8_t append;
  char value[DISPLAY_PARAM_VALUE_BUFFER_SIZE];
};

struct __attribute__((packed)) DisplayMessageRequest : public BaseRequest {
  DisplayMessageRequest(ClientId client, MessageId id)
      : BaseRequest(client, RequestType::DISPLAY_MESSAGE) {
    strlcpy(messageId, id, ID_BUFFER_SIZE);
  }
  char messageId[ID_BUFFER_SIZE];
};

struct __attribute__((packed)) DisplayBitmapRequest : public BaseRequest {
  DisplayBitmapRequest(ClientId client, BitmapId id)
      : BaseRequest(client, RequestType::DISPLAY_BITMAP) {
    strlcpy(bitmapId, id, ID_BUFFER_SIZE);
  }
  char bitmapId[ID_BUFFER_SIZE];
};

struct __attribute__((packed)) DisplayMenuRequest : public BaseRequest {
  DisplayMenuRequest(ClientId client, MenuId id, uint8_t selectedIndex)
      : BaseRequest(client, RequestType::DISPLAY_MENU),
        selectedIndex(selectedIndex) {
    strlcpy(menuId, id, ID_BUFFER_SIZE);
  }
  char menuId[ID_BUFFER_SIZE];
  uint8_t selectedIndex;
};

struct __attribute__((packed)) PlaySoundRequest : public BaseRequest {
  PlaySoundRequest(ClientId client, TrackId id)
      : BaseRequest(client, RequestType::PLAY_SOUND) {
    strlcpy(trackId, id, ID_BUFFER_SIZE);
  }
  char trackId[ID_BUFFER_SIZE];
};

struct __attribute__((packed)) StopSoundRequest : public BaseRequest {
  StopSoundRequest(ClientId client)
      : BaseRequest(client, RequestType::STOP_SOUND) {
  }
};

struct __attribute__((packed)) ReadDataRequest : public BaseRequest {
  ReadDataRequest(ClientId client, DataId id)
      : BaseRequest(client, RequestType::READ_DATA) {
    strlcpy(dataId, id, ID_BUFFER_SIZE);
  }
  char dataId[ID_BUFFER_SIZE];
};

/* ========================================================================= */
/*                                                               core status */
/* ========================================================================= */

enum class PlayerState : uint16_t {
  UNKNOWN = 0,
  IDLE = 1,
  PLAYING = 2
};

struct __attribute__((packed)) CoreStatus : public BaseStatus {
  CoreStatus()
      : BaseStatus(StatusType::CORE_STATUS) {
  }
  PlayerState playerState;
};

/* ========================================================================= */
/*                                                           core client api */
/* ========================================================================= */

#define STATUS_POLLING_DELAY 200
#define STATUS_POLLING_TIMEOUT 50

class CoreStatusHandler {
 public:
  virtual bool handleCoreStatus(CoreStatus status) = 0;
};

class DataStream : public Stream {
 public:
  DataStream(ClientId client);
  bool eof = false;
  int read() override;
  int available() override;
  int peek() override;
  size_t write(uint8_t) override;
  void flush() override;

  int bufferIndex;
  uint8_t buffer[REQUEST_BUFFER_SIZE];
  ClientId client;
};

class ActivityCoreClient : public PeriphericalClient {
 public:
  ActivityCoreClient(ClientId clientId, Scheduler& scheduler);

  void setup();
  void init(BitmapId startupBitmap);

  void putParameter(ParameterId id, uint16_t value);
  void putParameter(ParameterId id, const char* value);

  void playSound(TrackId id);
  void stopSound();

  void clearDisplay();
  void displayMessage(MessageId id);
  void displayBitmap(BitmapId id);
  void displayMenu(MenuId id, uint8_t selectedIndex);

  DataStream readData(DataId id);

  void pauseStatus();
  bool isStatusPaused();
  void resumeStatus(unsigned long pollingDelay = STATUS_POLLING_DELAY);
  void setStatusHandler(CoreStatusHandler* handler, unsigned long pollingDelay = STATUS_POLLING_DELAY, unsigned long pollingTimeout = STATUS_POLLING_TIMEOUT);
};
