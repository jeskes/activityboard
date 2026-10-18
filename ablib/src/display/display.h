#pragma once

#include "../modules/modules.h"
#include "../rotary/rotary.h"

/* ========================================================================= */
/*                                                               display api */
/* ========================================================================= */

using ParameterId = const char*;
using MessageId = const char*;
using BitmapId = const char*;
using MenuId = const char*;

struct __attribute__((packed)) InitDisplayRequest : public BaseRequest {
  InitDisplayRequest(ClientId client, BitmapId id)
      : BaseRequest(client, RequestType::DISPLAY_INIT) {
    strlcpy(startupBitmap, id, ID_BUFFER_SIZE);
  }
  char startupBitmap[ID_BUFFER_SIZE];
};

struct __attribute__((packed)) ClearDisplayRequest : public BaseRequest {
  ClearDisplayRequest(ClientId client)
      : BaseRequest(client, RequestType::DISPLAY_CLEAR) {
  }
};

/* 32 - 1 (client) - 2 (request-type) - 10 (id-length) - 1 (append-flag) - 2 terminating 0 */

#define DISPLAY_PARAM_VALUE_BUFFER_SIZE 16

struct __attribute__((packed)) DisplayParameterRequest : public BaseRequest {
  DisplayParameterRequest(ClientId client, ParameterId id, const char* value, bool append)
      : BaseRequest(client, RequestType::DISPLAY_PARAMETER) {
    strlcpy(this->id, id, PARAM_BUFFER_SIZE);
    strlcpy(this->value, value, DISPLAY_PARAM_VALUE_BUFFER_SIZE);
    this->append = append;
  }
  char id[PARAM_BUFFER_SIZE];
  byte append;
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
  DisplayMenuRequest(ClientId client, MenuId id)
      : BaseRequest(client, RequestType::DISPLAY_MENU) {
    strlcpy(menuId, id, ID_BUFFER_SIZE);
  }
  char menuId[ID_BUFFER_SIZE];
};

struct __attribute__((packed)) DisplayStatus : public BaseStatus {
  DisplayStatus()
      : BaseStatus(StatusType::DISPLAY_STATUS) {
  }
  RotaryStatus rotaryStatus;
};

class ActivityDisplayClient : public PeriphericalClient {
 public:
  ActivityDisplayClient(ClientId clientId, Scheduler& scheduler);
  void init(BitmapId startupBitmap);
  void clearDisplay();
  void putParam(const char* name, uint16_t value);
  void putParam(const char* name, const char* value);
  void displayMessage(MessageId messageId);
  void displayBitmap(BitmapId bitmapId);
  void displayMenu(MenuId menuId);
};
