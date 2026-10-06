#include "clientcontext.h"

#include <ArduinoJson.h>
#include <ablib.h>

#define VALUE_BUFFER_SIZE 128
#define INITIAL_CLIENT_ID 10

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
  init(client, context);

  return context;
}

void ClientContexts::init(ClientId client, ClientContext& context) {
  context.client = client;

  char path[PATH_BUFFER_SIZE];
  sprintf(path, "/%02d/parameters.json", client);

  JsonDocument doc;
  if (!storage.loadJson(path, doc)) {
    LOG("ClientContext: no parameters file found: path=%s.", path);
  }
  else {
    JsonArray names = doc.as<JsonArray>();
    for (JsonVariant v : names) {
      const char* name = v.as<const char*>();
      context.params[name] = "";
      context.params[name].reserve(VALUE_BUFFER_SIZE);
    }
    LOG("ClientContext: initialized parameters: client=%d, count=%d.", client, names.size());
  }

  sprintf(path, "/%02d/messages.json", client);
  if (!storage.loadJson(path, context.messages)) {
    LOG("ClientContext: no messages file found: %s.", path);
  }
  else {
    LOG("ClientContext: initialized messages: client=%d, count=%d.", client, context.messages.size());
  }

  sprintf(path, "/%02d/menus.json", client);
  if (!storage.loadJson(path, context.menus)) {
    LOG("ClientContext: no menus file found: %s.", path);
  }
  else {
    LOG("ClientContext: initialized menus: client=%d, count=%d.", client, context.menus.size());
  }

  sprintf(path, "/%02d/bitmaps.json", client);
  if (!storage.loadJson(path, context.bitmaps)) {
    LOG("ClientContext: no bitmaps file found: %s.", path);
  }
  else {
    LOG("ClientContext: initialized bitmaps: client=%d, count=%d.", client, context.bitmaps.size());
  }
}

/* context */

void ClientContext::putParam(const char* name, const char* value, byte append) {
  if (append) {
    params[name] += value;
  }
  else {
    params[name] = value;
  }
  LOG("ClientContest: put parameter: %s => %s", name, params[name].c_str());
}

const char* ClientContext::getMessage(const char* id) {
  const char* message = messages[id];
  if (!message) {
    LOG("ClientContext: message id %s not found.", id);
    return id;
  }
  return message;
}

String ClientContext::formatMessage(MessageId id) {
  String output = String(getMessage(id));
  output.reserve(VALUE_BUFFER_SIZE);

  LOG("format: %s", output.c_str());

  for (const auto& pair : params) {
    String placeholder = "";
    placeholder.reserve(pair.first.length() + 4);
    placeholder += "{{";
    placeholder += pair.first;
    placeholder += "}}";

    if (output.indexOf(placeholder) != -1) {
      output.replace(placeholder, pair.second);
      LOG("placeholder replaced: %s [%s] => %s", placeholder.c_str(), pair.second.c_str(), output.c_str());
    }
    else {
      LOG("placeholder not found: %s", placeholder.c_str());
    }
  }

  return utf8ToExtendedAscii(output);
}

MenuDef ClientContext::getMenuDef(MenuId id) {
  MenuDef def;
  if (menus[id].is<JsonObject>()) {
    JsonObject object = menus[id].as<JsonObject>();
    def.title = object["title"].as<const char*>();
    int idx = 0;
    for (JsonVariant option : object["options"].as<JsonArray>()) {
      if (idx < MAX_MENU_OPTIONS) {
        def.options[idx] = option.as<const char*>();
      }
    }
  }
  return def;
}

BitmapDef ClientContext::getBitmapDef(BitmapId id) {
  BitmapDef def;
  if (bitmaps[id].is<const char*>()) {
    sprintf(def.path, "/%02d/%s", client, bitmaps[id].as<const char*>());
  }
  else if (bitmaps[id].is<JsonObject>()) {
    JsonObject object = bitmaps[id].as<JsonObject>();
    sprintf(def.path, "/%02d/%s", client, object["path"].as<const char*>());
    if (object["width"].is<int>()) {
      def.width = object["width"].as<int>();
    }
    if (object["height"].is<int>()) {
      def.width = object["height"].as<int>();
    }
  }
  else {
    LOG("ClientContext: bitmap id %d not found.", id);
    strcpy(def.path, "/10/fallback.raw");
  }
  return def;
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