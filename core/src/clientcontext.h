#pragma once

#include <map>

#include "base/base.h"
#include "storage/storage.h"

#define MAX_MENU_OPTIONS 4

struct MenuDef {
  const char* title = nullptr;
  const char* options[MAX_MENU_OPTIONS] = {nullptr};
};

struct ImageDef {
  char path[PATH_BUFFER_SIZE] = {0};
  uint16_t width = DEFAULT_IMAGE_WIDTH;
  uint16_t height = DEFAULT_IMAGE_HEIGHT;
};

struct DataDef {
  char path[PATH_BUFFER_SIZE] = {0};
};

struct MusicDef {
  TrackFolder folder = 0;
  TrackId track = 0;
  MusicName name = nullptr;
  const char* image = nullptr;
};

class ClientContext {
 public:
  void putParam(ParameterId id, const char* value, byte append);

  const char* getMessage(MessageId id);
  String formatMessage(MessageId id);
  MenuDef getMenuDef(MenuId id);
  ImageDef getImageDef(ImageId id);
  DataDef getDataDef(DataId id);
  MusicDef getMusicDef(MusicAlias alias);
  MusicDef getMusicDef(MusicCategory category, MusicName name);
  MusicDef getMusicDef(MusicCategory category, MusicIndex index);

  String utf8ToExtendedAscii(const String& utf8);

 private:
  void init(Storage& storage, ClientId client);
  void initMusicCategories(Storage& storage);

  ClientId client;
  Storage* storage;

  JsonDocument config;
  std::map<String, String> params;

  void fillImagePath(MusicDef& def);
  std::map<uint8_t, JsonDocument> musicCategories;

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