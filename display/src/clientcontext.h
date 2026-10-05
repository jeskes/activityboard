#ifndef _AB_TEXT
#define _AB_TEXT

#include <ArduinoJson.h>
#include <ablib.h>

#include <map>

#include "storage.h"

class ClientContext {
 public:
  void clearParams();
  void clearParam(const char* name);
  void putParam(const char* name, const char* value, byte append);

  const char* getMessage(MessageId id);
  String formatMessage(MessageId id);
  String getBitmapPath(BitmapId id);
  JsonObject getMenuDef(MenuId id);
  
  String utf8ToExtendedAscii(const String& utf8);

 private:
  ClientId client;
  std::map<String, String> params;
  JsonDocument messages;
  JsonDocument bitmaps;
  JsonDocument menus;

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
