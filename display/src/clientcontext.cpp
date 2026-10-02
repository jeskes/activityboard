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
    LOG("ClientContext: cannot load parameter file: %s.", path);
    return;
  }

  JsonArray names = doc.as<JsonArray>();
  params.reserve(names.size());

  for (JsonVariant v : names) {
    const char* name = v.as<const char*>();
    params[name] = "";
    params[name].reserve(VALUE_BUFFER_SIZE);
  }
}

void ClientContext::initMessages(uint16_t clientId) {
  char path[PATH_BUFFER_SIZE];
  sprintf(path, "/%02d/messages.json", clientId);
  if (!storage.loadJson(path, messages)) {
    LOG("ClientContext: cannot load messages file: %s.", path);
    return;
  }
}

void ClientContext::initBitmaps(uint16_t clientId) {
  char path[PATH_BUFFER_SIZE];
  sprintf(path, "/%02d/bitmaps.json", clientId);
  if (!storage.loadJson(path, messages)) {
    LOG("ClientContext: cannot load bitmaps file: %s.", path);
    return;
  }
}

void ClientContext::putParam(const char* name, const char* value, byte append) {
  if (params.count(name)) {
    if (append) {
      params[name] += value;
    }
    else {
      params[name] = value;
    }
  }
}

const char* ClientContext::getMessage(const char* id) {
  if (!messages[id].is<const char*>()) {
    LOG("ClientContext: message id %d not found.", id);
    return id;
  }
  return messages[id];
}

String ClientContext::formatMessage(const char* id) {
  String output = String(getMessage(id));
  output.reserve(VALUE_BUFFER_SIZE);

  for (const auto& pair : params) {
    String placeholder = "";
    placeholder.reserve(strlen(pair.first) + 4);
    placeholder += "{{";
    placeholder += pair.first;
    placeholder += "}}";

    if (output.indexOf(placeholder) != -1) {
      output.replace(placeholder, pair.second);
    }
  }

  return output;
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