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
  PLAY_TRACK = 0x10,
  STOP_SOUND = 0x11,
  PAUSE_TRACK = 0x12,
  RESUME_TRACK = 0x13,
  CLEAR_DISPLAY = 0x20,
  DISPLAY_MESSAGE = 0x21,
  DISPLAY_IMAGE = 0x22,
  DISPLAY_MENU = 0x23,
  READ_DATA = 0x24,
  STATUS = 0xA0
};

enum class StatusType : uint8_t {
  UNKNOWN = 0,
  CORE_STATUS = 0x01,
  CORE_DATA = 0x02
};

struct __attribute__((packed)) BaseRequest {
  BaseRequest(ClientId client, RequestType type, size_t requestSize) {
    memset(this, 0, requestSize);
    this->client = client;
    this->type = type;
  }
  ClientId client;
  RequestType type;
};

struct __attribute__((packed)) StatusRequest : public BaseRequest {
  StatusRequest(ClientId client, StatusType statusType)
      : BaseRequest(client, RequestType::STATUS, sizeof(StatusRequest)),
        statusType(statusType) {
  }
  StatusType statusType;
  bool interrupted;
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
  InitCoreRequest(ClientId client, ImagePath path)
      : BaseRequest(client, RequestType::INIT, sizeof(InitCoreRequest)) {
    strlcpy(startupImage, path, ID_BUFFER_SIZE);
  }
  char startupImage[ID_BUFFER_SIZE];
};

struct __attribute__((packed)) ClearDisplayRequest : public BaseRequest {
  ClearDisplayRequest(ClientId client)
      : BaseRequest(client, RequestType::CLEAR_DISPLAY, sizeof(ClearDisplayRequest)) {
  }
};

struct __attribute__((packed)) ParameterRequest : public BaseRequest {
  ParameterRequest(ClientId client, ParameterId id, const char* value, bool append)
      : BaseRequest(client, RequestType::PARAMETER,sizeof(ParameterRequest)) {
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
      : BaseRequest(client, RequestType::DISPLAY_MESSAGE, sizeof(DisplayMessageRequest)) {
    strlcpy(messageId, id, ID_BUFFER_SIZE);
  }
  char messageId[ID_BUFFER_SIZE];
};

struct __attribute__((packed)) DisplayImageRequest : public BaseRequest {
  DisplayImageRequest(ClientId client, ImagePath path)
      : BaseRequest(client, RequestType::DISPLAY_IMAGE,sizeof(DisplayImageRequest)) {
    strlcpy(this->path, path, ID_BUFFER_SIZE);
  }
  char path[ID_BUFFER_SIZE];
};

struct __attribute__((packed)) DisplayMenuRequest : public BaseRequest {
  DisplayMenuRequest(ClientId client, MenuId id, uint8_t selectedIndex)
      : BaseRequest(client, RequestType::DISPLAY_MENU, sizeof(DisplayMenuRequest)),
        selectedIndex(selectedIndex) {
    strlcpy(menuId, id, ID_BUFFER_SIZE);
  }
  char menuId[ID_BUFFER_SIZE];
  uint8_t selectedIndex;
};

struct __attribute__((packed)) SoundControl : CoreFlags<SoundControl> {
  static inline constexpr uint8_t DISPLAY_INFO = (1 << 0);
  static inline constexpr uint8_t RESERVED_1 = (1 << 1);
  static inline constexpr uint8_t RESERVED_2 = (1 << 2);
  static inline constexpr uint8_t RESERVED_3 = (1 << 3);
  static inline constexpr uint8_t RESERVED_4 = (1 << 4);
  static inline constexpr uint8_t RESERVED_5 = (1 << 5);
  static inline constexpr uint8_t RESERVED_6 = (1 << 6);
  static inline constexpr uint8_t RESERVED_7 = (1 << 7);
  SoundControl()
      : CoreFlags() {
  }
  SoundControl(uint8_t flags)
      : CoreFlags(flags) {
  }
};

struct __attribute__((packed)) PlayTrackRequest : public BaseRequest {
  PlayTrackRequest(ClientId client, TrackPath path, SoundControl control)
      : BaseRequest(client, RequestType::PLAY_TRACK, sizeof(PlayTrackRequest)),
        control(control) {
    strlcpy(this->path, path, ID_BUFFER_SIZE);
  }

  SoundControl control;
  char path[ID_BUFFER_SIZE];
};

struct __attribute__((packed)) StopSoundRequest : public BaseRequest {
  StopSoundRequest(ClientId client)
      : BaseRequest(client, RequestType::STOP_SOUND, sizeof(StopSoundRequest)) {
  }
};

struct __attribute__((packed)) PauseTrackRequest : public BaseRequest {
  PauseTrackRequest(ClientId client)
      : BaseRequest(client, RequestType::PAUSE_TRACK, sizeof(PauseTrackRequest)){
  }
};

struct __attribute__((packed)) ResumeTrackRequest : public BaseRequest {
  ResumeTrackRequest(ClientId client)
      : BaseRequest(client, RequestType::RESUME_TRACK, sizeof(ResumeTrackRequest)){
  }
};

struct __attribute__((packed)) ReadDataRequest : public BaseRequest {
  ReadDataRequest(ClientId client, DataPath path)
      : BaseRequest(client, RequestType::READ_DATA, sizeof(ReadDataRequest)) {
    strlcpy(this->path, path, ID_BUFFER_SIZE);
  }
  char path[ID_BUFFER_SIZE];
};

/* ========================================================================= */
/*                                                               core status */
/* ========================================================================= */

enum class PlayerState : uint8_t {
  UNKNOWN = 0x00,
  IDLE = 0x01,
  PLAYING = 0x02
};

struct __attribute__((packed)) WebState {
  bool active;
  uint16_t age;
  Input input;
};

struct __attribute__((packed)) CoreControl : CoreFlags<CoreControl> {
  static inline constexpr uint8_t NONE = 0;
  static inline constexpr uint8_t STOP_ALL = (1 << 0);    // Bit 0: 0x01 (1)
  static inline constexpr uint8_t RESERVED_1 = (1 << 1);  // Bit 1: 0x02 (2)
  static inline constexpr uint8_t RESERVED_2 = (1 << 2);  // Bit 2: 0x04 (4)
  static inline constexpr uint8_t RESERVED_3 = (1 << 3);  // Bit 3: 0x08 (8)
  static inline constexpr uint8_t RESERVED_4 = (1 << 4);  // Bit 4: 0x10 (16)
  static inline constexpr uint8_t RESERVED_5 = (1 << 5);  // Bit 5: 0x20 (32)
  static inline constexpr uint8_t RESERVED_6 = (1 << 6);  // Bit 6: 0x40 (64)
  static inline constexpr uint8_t RESERVED_7 = (1 << 7);  // Bit 7: 0x80 (128)
};

struct __attribute__((packed)) CoreStatus : public BaseStatus {
  CoreStatus(uint8_t serial = 0)
      : BaseStatus(serial, StatusType::CORE_STATUS),
        playerState(PlayerState::UNKNOWN),
        webState{false} {
  }
  CoreControl control;
  PlayerState playerState;
  WebState webState;
};
