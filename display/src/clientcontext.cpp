#include "clientcontext.h"

#include <ArduinoJson.h>
#include <ablib.h>

#define PATH_BUFFER_SIZE 64
#define VALUE_BUFFER_SIZE 128
#define INITIAL_CLIENT_ID 10

ClientContext::ClientContext(Storage& storage)
    : clientId(INITIAL_CLIENT_ID),
      storage(storage) {
}

void ClientContext::setup() {
  /* nothing yet */
}

void ClientContext::init(uint16_t clientId) {
  LOG("ClientContext: initialize client: id=%d", clientId);
  this->clientId = clientId;
  initParams(clientId);
  initMessages(clientId);
  initBitmaps(clientId);
}

void ClientContext::initParams(uint16_t clientId) {
  char path[PATH_BUFFER_SIZE];
  sprintf(path, "/%02d/parameters.json", clientId);

  JsonDocument doc;
  if (!storage.loadJson(path, doc)) {
    LOG("ClientContext: cannot load parameter file: path=%s.", path);
    return;
  }

  JsonArray names = doc.as<JsonArray>();
  params.clear();

  for (JsonVariant v : names) {
    const char* name = v.as<const char*>();
    params[name] = "";
    params[name].reserve(VALUE_BUFFER_SIZE);
  }

  LOG("ClientContext: initialized parameters: count=%d.", names.size());
}

void ClientContext::initMessages(uint16_t clientId) {
  char path[PATH_BUFFER_SIZE];
  sprintf(path, "/%02d/messages.json", clientId);
  if (!storage.loadJson(path, messages)) {
    LOG("ClientContext: cannot load messages file: %s.", path);
    return;
  }

  LOG("ClientContext: initialized messages: count=%d.", messages.size());
}

void ClientContext::initBitmaps(uint16_t clientId) {
  char path[PATH_BUFFER_SIZE];
  sprintf(path, "/%02d/bitmaps.json", clientId);
  if (!storage.loadJson(path, bitmaps)) {
    LOG("ClientContext: cannot load bitmaps file: %s.", path);
    return;
  }

  LOG("ClientContext: initialized bitmaps: count=%d.", bitmaps.size());
}

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

String ClientContext::formatMessage(const char* id) {
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

String ClientContext::getBitmapPath(const char* id) {
  if (!bitmaps[id].is<const char*>()) {
    LOG("ClientContext: bitmap id %d not found.", id);
    return "/10/fallback.bmp";
  }
  char path[PATH_BUFFER_SIZE];
  sprintf(path, "/%02d/%s", clientId, bitmaps[id]);
  return String(path);
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