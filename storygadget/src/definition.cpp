#include "definition.h"

#include <ArduinoJson.h>

#include "const.h"

Definition::Definition(ActivityCoreClient& core)
    : core(core) {
}

void Definition::setup() {
  loadStories();
}

void Definition::loadStories() {
  DataStream stream = core.readData(STORIES_DATA);
  DeserializationError error = deserializeJson(storiesDocument, stream);
  if (error) {
    LOG("cannot load stories: error=%s", error.c_str());
    return;
  }

  storiesCount = 0;
  JsonObject entries = storiesDocument.as<JsonObject>();
  for (JsonPair entry : entries) {
    if (storiesCount < MAX_STORIES) {
      stories[storiesCount] = entry.key().c_str();
      storiesCount++;
    }
    else {
      break;
    }
  }

  LOG("stories loaded: count=%d", storiesCount);
}

StoryId Definition::getStoryId(int index) {
  return stories[index];
}

JsonObject Definition::getStoryInfo(int index) {
  return getStoryInfo(stories[index]);
}

JsonObject Definition::getStoryInfo(StoryId story) {
  return storiesDocument[story].as<JsonObject>();
}

JsonDocument& Definition::getStoryDefinition(StoryId story) {
  auto iter = storyDefinitions.find(story);
  if (iter != storyDefinitions.end()) {
    return iter->second;
  }

  DataId data = getStoryInfo(story)["definition"].as<DataId>();
  JsonDocument& definition = storyDefinitions[story];
  loadStory(definition, data);

  return definition;
}

void Definition::loadStory(JsonDocument& doc, DataId data) {
  LOG("loading story definition: %s", data);
  core.displayMessage(LOADING_STORY);
  DataStream stream = core.readData(data);
  DeserializationError error = deserializeJson(doc, stream);
  if (error) {
    LOG("cannot load story definition: error=%s", error.c_str());
  }
  else {
    LOG("story definition loaded");
    Serial.println();
    serializeJsonPretty(doc, Serial);
    Serial.println();
  }
}