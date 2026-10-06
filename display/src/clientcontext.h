#ifndef _AB_TEXT
#define _AB_TEXT

#include <ArduinoJson.h>
#include <ablib.h>

#include <map>

#include "storage.h"

#define MAX_MENU_OPTIONS 4

struct MenuDef {
  const char* title = nullptr;
  const char* options[MAX_MENU_OPTIONS];
};

struct BitmapDef {
  char path[PATH_BUFFER_SIZE];
  uint16_t width = DEFAULT_BITMAP_WIDTH;
  uint16_t height = DEFAULT_BITMAP_HEIGHT;
};

class ClientContext {
 public:
  void clearParams();
  void clearParam(const char* name);
  void putParam(const char* name, const char* value, byte append);

  const char* getMessage(MessageId id);
  String formatMessage(MessageId id);
  MenuDef getMenuDef(MenuId id);
  BitmapDef getBitmapDef(BitmapId id);

  String utf8ToExtendedAscii(const String& utf8);

 private:
  ClientId client;
  std::map<String, String> params;
  JsonDocument messages;
  JsonDocument menus;
  JsonDocument bitmaps;

  friend class ClientContexts;
};

class ClientContexts {
 public:
  ClientContexts(Storage& storage);
  ClientContext& get(ClientId client);

 private:
  Storage& storage;
  std::map<ClientId, ClientContext> contexts;
  void init(ClientId client, ClientContext& context);
};

#endif
