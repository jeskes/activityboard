#include "clientcontext.h"

/* TODO : check this value */
inline constexpr int VALUE_BUFFER_SIZE = 128;

/* static buffers */

static char trackPathBuffer[PATH_BUFFER_SIZE];

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

  LOG("messages: client=%d, count=%d", client, config["messages"].is<JsonObject>() ? config["messages"].as<JsonObject>().size() : 0);
  LOG("menus: client=%d, count=%d", client, config["menus"].is<JsonObject>() ? config["menus"].as<JsonObject>().size() : 0);
  LOG("aliases: client=%d, count=%d", client, config["alias"].is<JsonObject>() ? config["alias"].as<JsonObject>().size() : 0);
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

ImageDef ClientContext::getImageDef(ImagePath path) {
  ImageDef def;
  resolvePath(path, def.path);
  return def;
}

DataDef ClientContext::getDataDef(DataPath path) {
  DataDef def;
  resolvePath(path, def.path);
  return def;
}

TrackDef ClientContext::getTrackDef(TrackPath path) {
  TrackDef def;
  resolvePath(path, def.path);
  return def;
}

void ClientContext::resolvePath(const char* path, char* physical) {
  if (!path || !*path) {
    *physical = 0;
  }
  else if (*path == '@') {
    resolvePath(config["alias"][path + 1].as<const char*>(), physical);
  }
  else if (*path == '/') {
    strcpy(physical, path);
  }
  else {
    sprintf(physical, "/%02d/%s", client, path);
  }
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