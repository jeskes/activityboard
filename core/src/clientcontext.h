#pragma once

#include <map>

#include "base/base.h"
#include "storage/storage.h"

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

struct DataDef {
  char path[PATH_BUFFER_SIZE];
};

struct MusicDef {
  TrackFolder folder;
  TrackId track;
};

class ClientContext {
 public:
  void clearParams();
  void clearParam(ParameterId id);
  void putParam(ParameterId id, const char* value, byte append);

  const char* getMessage(MessageId id);
  String formatMessage(MessageId id);
  MenuDef getMenuDef(MenuId id);
  BitmapDef getBitmapDef(BitmapId id);
  DataDef getDataDef(DataId id);
  MusicDef getMusicDef(MusicId id);

  String utf8ToExtendedAscii(const String& utf8);

 private:
  void init(Storage& storage, ClientId client);

  ClientId client;
  JsonDocument config;
  std::map<String, String> params;

  friend class ClientContexts;
};

class ClientContexts {
 public:
  ClientContexts(Storage& storage);
  ClientContext& get(ClientId client);

 private:
  Storage& storage;
  std::map<ClientId, ClientContext> contexts;
};