#pragma once

#include <Arduino.h>

#include "../base/base.h"

/* ========================================================================= */
/*                                                     general request types */
/* ========================================================================= */

enum class RequestType : uint8_t {
  UNKNOWN = 0x00,
  INIT = 0x01,
  PARAMETER = 0x02,
  PLAY_SOUND = 0x10,
  STOP_SOUND = 0x11,
  CLEAR_DISPLAY = 0x20,
  DISPLAY_MESSAGE = 0x21,
  DISPLAY_BITMAP = 0x22,
  DISPLAY_MENU = 0x23,
  STATUS = 0xA0
};

enum class StatusType : uint8_t {
  UNKNOWN = 0,
  CORE_STATUS = 0x01,
};

struct __attribute__((packed)) BaseRequest {
  BaseRequest(ClientId client, RequestType t)
      : client(client),
        type(t) {
  }
  ClientId client;
  RequestType type;
};

struct __attribute__((packed)) StatusRequest : public BaseRequest {
  StatusRequest(ClientId client, StatusType statusType)
      : BaseRequest(client, RequestType::STATUS),
        statusType(statusType) {
  }
  StatusType statusType;
};

struct __attribute__((packed)) BaseStatus {
  uint8_t serial;
  StatusType type;
  BaseStatus(uint8_t serial, StatusType t)
      : serial(serial),
        type(t) {
  }
};

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

struct __attribute__((packed)) ParameterRequest : public BaseRequest {
  ParameterRequest(ClientId client, ParameterId id, const char* value, bool append)
      : BaseRequest(client, RequestType::PARAMETER) {
    strlcpy(this->id, id, PARAM_NAME_BUFFER_SIZE);
    strlcpy(this->value, value, PARAM_VALUE_BUFFER_SIZE);
    this->append = append;
  }
  char id[PARAM_NAME_BUFFER_SIZE];
  byte append;
  char value[PARAM_VALUE_BUFFER_SIZE];
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
  PlaySoundRequest(ClientId client, TrackFolder folder, TrackId track)
      : BaseRequest(client, RequestType::PLAY_SOUND),
        folder(folder),
        track(track) {
  }
  TrackFolder folder;
  TrackId track;
};

struct __attribute__((packed)) StopSoundRequest : public BaseRequest {
  StopSoundRequest(ClientId client)
      : BaseRequest(client, RequestType::STOP_SOUND) {
  }
};

/* ========================================================================= */
/*                                                               core status */
/* ========================================================================= */

enum class PlayerState : uint8_t {
  UNKNOWN = 0x00,
  IDLE = 0x01,
  PLAYING = 0x02
};

struct __attribute__((packed)) CoreStatus : public BaseStatus {
  CoreStatus(uint8_t serial=0)
      : BaseStatus(serial, StatusType::CORE_STATUS) {
  }
  PlayerState playerState;
};
