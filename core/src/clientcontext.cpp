#include "clientcontext.h"

/* TODO : check this value */
inline constexpr int VALUE_BUFFER_SIZE = 128;

/* static buffers */

static char musicNameBuffer[ID_BUFFER_SIZE];
static char musicImageBuffer[PATH_BUFFER_SIZE];

/* contexts (manager) */

ClientContexts::ClientContexts(Storage& storage)
    : storage(storage) {
}

ClientContext& ClientContexts::get(ClientId client) {
  auto iter = contexts.find(client);
  if (iter != contexts.end()) {
    return iter->second;
  }

  ClientContext& context = contexts[client];
  context.init(storage, client);

  return context;
}

/* context */

void ClientContext::init(Storage& storage, ClientId client) {
  this->client = client;
  this->storage = &storage;

  char path[PATH_BUFFER_SIZE];
  sprintf(path, "/%02d/config.json", client);

  if (!storage.loadJson(path, config)) {
    LOG("no config file found: client=%d, path=%s.", client, path);
    return;
  }

  LOG("loaded config file: client=%d, path=%s", client, path);

  if (config["parameters"].is<JsonArray>()) {
    JsonArray names = config["parameters"].as<JsonArray>();
    for (JsonVariant v : names) {
      const char* name = v.as<const char*>();
      params[name] = "";
      params[name].reserve(VALUE_BUFFER_SIZE);
    }
    LOG("parameters: client=%d, count=%d", client, names.size());
  }
  else {
    LOG("parameters: client=%d, count=0");
  }

  initMusicCategories(storage);

  LOG("messages: client=%d, count=%d", client, config["messages"].is<JsonObject>() ? config["messages"].as<JsonObject>().size() : 0);
  LOG("menus: client=%d, count=%d", client, config["menus"].is<JsonObject>() ? config["menus"].as<JsonObject>().size() : 0);
  LOG("images: client=%d, count=%d", client, config["images"].is<JsonObject>() ? config["images"].as<JsonObject>().size() : 0);
  LOG("data: client=%d, count=%d", client, config["data"].is<JsonObject>() ? config["data"].as<JsonObject>().size() : 0);
  LOG("music categories: client=%d, count=%d", client, config["music"]["categories"].is<JsonArray>() ? config["music"]["categories"].as<JsonArray>().size() : 0);
  LOG("music aliases: client=%d, count=%d", client, config["music"]["alias"].is<JsonObject>() ? config["music"]["alias"].as<JsonObject>().size() : 0);
}

void ClientContext::initMusicCategories(Storage& storage) {
  if (config["music"]["categories"].is<JsonArray>()) {
    JsonArray categories = config["music"]["categories"].as<JsonArray>();
    for (size_t idx = 0; idx < categories.size(); idx++) {
      TrackFolder folder = categories[idx]["folder"].as<TrackFolder>();
      char path[PATH_BUFFER_SIZE];
      sprintf(path, "/%02d/music.json", folder);
      LOG("load music folder config: path=%s", path);
      storage.loadJson(path, musicCategories[idx]);
      LOG("loaded music folder config: path=%s, entries=%d", path, musicCategories[idx].size());
    }
  }
}

void ClientContext::putParam(ParameterId name, const char* value, byte append) {
  if (append) {
    params[name] += value;
  }
  else {
    params[name] = value;
  }
  // LOG("put parameter: %s => %s", name, params[name].c_str());
}

const char* ClientContext::getMessage(MessageId id) {
  if (config["messages"][id].is<const char*>()) {
    return config["messages"][id].as<const char*>();
  }
  LOG("message id %s not found.", id);
  return id;
}

String ClientContext::formatMessage(MessageId id) {
  String output = String(getMessage(id));
  output.reserve(VALUE_BUFFER_SIZE);

  // LOG("format: %s", output.c_str());

  for (const auto& pair : params) {
    String placeholder = "";
    placeholder.reserve(pair.first.length() + 4);
    placeholder += "{{";
    placeholder += pair.first;
    placeholder += "}}";

    if (output.indexOf(placeholder) != -1) {
      output.replace(placeholder, pair.second);
      // LOG("placeholder replaced: %s [%s] => %s", placeholder.c_str(), pair.second.c_str(), output.c_str());
    }
  }

  return utf8ToExtendedAscii(output);
}

MenuDef ClientContext::getMenuDef(MenuId id) {
  MenuDef def;
  if (config["menus"][id].is<JsonObject>()) {
    JsonObject object = config["menus"][id].as<JsonObject>();
    def.title = object["title"].as<const char*>();
    int idx = 0;
    for (JsonVariant option : object["options"].as<JsonArray>()) {
      if (idx < MAX_MENU_OPTIONS) {
        def.options[idx] = option.as<const char*>();
        idx++;
      }
    }
    while (idx < MAX_MENU_OPTIONS) {
      def.options[idx] = nullptr;
      idx++;
    }
  }
  else {
    LOG("menu id %s not found", id);
  }
  return def;
}

ImageDef ClientContext::getImageDef(ImageId id) {
  ImageDef def;
  if (config["images"][id].is<const char*>()) {
    sprintf(def.path, "/%02d/images/%s", client, config["images"][id].as<const char*>());
  }
  else if (config["images"][id].is<JsonObject>()) {
    JsonObject object = config["images"][id].as<JsonObject>();
    sprintf(def.path, "/%02d/images/%s", client, object["path"].as<const char*>());
    if (object["width"].is<int>()) {
      def.width = object["width"].as<int>();
    }
    if (object["height"].is<int>()) {
      def.width = object["height"].as<int>();
    }
  }
  else {
    LOG("image id %s not found.", id);
    strcpy(def.path, "/10/images/fallback.raw");
  }
  return def;
}

DataDef ClientContext::getDataDef(DataId id) {
  DataDef def;
  if (config["data"][id].is<const char*>()) {
    sprintf(def.path, "/%02d/data/%s", client, config["data"][id].as<const char*>());
  }
  else {
    LOG("data id %s not found.", id);
    strcpy(def.path, "/10/data/fallback.txt");
  }
  return def;
}

MusicDef ClientContext::getMusicDef(MusicAlias alias) {
  MusicDef def;
  JsonObject aliasDef = config["music"]["alias"][alias].as<JsonObject>();
  if (!aliasDef.isNull()) {
    MusicCategory category = aliasDef["category"].as<MusicCategory>();
    MusicName trackname = aliasDef["trackname"].as<MusicName>();
    LOG("map music alias: %s => (%d, %s)", alias, category, trackname);
    return getMusicDef(category, trackname);
  }
  else {
    LOG("music not found: alias=%s", alias);
  }
  return def;
}

MusicDef ClientContext::getMusicDef(MusicCategory category, MusicName name) {
  MusicDef def;
  JsonObject categoryDef = config["music"]["categories"].as<JsonArray>()[category];
  if (!categoryDef.isNull()) {
    def.folder = categoryDef["folder"].is<TrackFolder>() ? categoryDef["folder"].as<TrackFolder>() : client;
    JsonDocument categoryConfig = musicCategories[category];
    if (categoryConfig.size() > 0) {
      if (!categoryConfig[name].isNull()) {
        def.track = categoryConfig[name].as<TrackId>();
        strlcpy(musicNameBuffer, name, ID_BUFFER_SIZE);
        def.name = musicNameBuffer;
        fillImagePath(def);
      }
      else {
        LOG("music not found in category: category=%d, name=%s", category, name);
      }
    }
    else {
      LOG("music category is empty: category=%d", category);
    }
  }
  else {
    LOG("music not found: category=%d", category);
  }
  return def;
}

MusicDef ClientContext::getMusicDef(MusicCategory category, MusicIndex index) {
  MusicDef def;
  JsonObject categoryDef = config["music"]["categories"].as<JsonArray>()[category];
  if (!categoryDef.isNull()) {
    def.folder = categoryDef["folder"].is<TrackFolder>() ? categoryDef["folder"].as<TrackFolder>() : client;
    JsonDocument tracks = musicCategories[category];
    index = index % tracks.size();
    uint8_t idx = 0;
    for (JsonPair entry : tracks.as<JsonObject>()) {
      if (idx == index) {
        def.track = entry.value().as<TrackId>();
        strlcpy(musicNameBuffer, entry.key().c_str(), ID_BUFFER_SIZE);
        def.name = musicNameBuffer;
        fillImagePath(def);
        break;
      }
      idx++;
    }
  }
  else {
    LOG("music not found: category=%d", category);
  }
  return def;
}

void ClientContext::fillImagePath(MusicDef& def) {
  char path[PATH_BUFFER_SIZE];
  sprintf(path, "/%02d/images/%s.raw", def.folder, def.name);
  strlcpy(musicImageBuffer, storage->exists(path) ? path : "/10/images/music.raw", PATH_BUFFER_SIZE);
  def.image = musicImageBuffer;
}

String ClientContext::utf8ToExtendedAscii(const String& utf8Str) {
  String result = "";
  result.reserve(utf8Str.length());

  for (size_t i = 0; i < utf8Str.length(); i++) {
    uint8_t c = utf8Str[i];

    if (c == 0xC3 && i + 1 < utf8Str.length()) {
      uint8_t next = utf8Str[++i];

      switch (next) {
        case 0x84:
          result += (char)142;
          break;  // Ä
        case 0x96:
          result += (char)153;
          break;  // Ö
        case 0x9C:
          result += (char)154;
          break;  // Ü
        case 0xA4:
          result += (char)132;
          break;  // ä
        case 0xB6:
          result += (char)148;
          break;  // ö
        case 0xBC:
          result += (char)129;
          break;  // ü
        default:
          result += (char)next;
          break;
      }
    }
    else if (c == 0xC2 && i + 1 < utf8Str.length() && (uint8_t)utf8Str[i + 1] == 0xDF) {
      result += (char)225;  // ß
      i++;
    }
    else {
      result += (char)c;
    }
  }
  return result;
}