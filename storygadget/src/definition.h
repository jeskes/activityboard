#pragma once

#include <ArduinoJson.h>
#include <core/core.h>

#include <map>

#include "const.h"

class Definition {
 public:
  Definition(ActivityCoreClient& core);
  void setup();

  int storiesCount;
  StoryId getStoryId(int index);
  JsonObject getStoryInfo(int index);
  JsonObject getStoryInfo(StoryId story);
  JsonDocument& getStoryDefinition(StoryId story);

 private:
  ActivityCoreClient& core;
  JsonDocument storiesDocument;

  StoryId stories[MAX_STORIES];
  std::map<StoryId, JsonDocument> storyDefinitions;

  void loadStories();
  void loadStory(JsonDocument& doc, DataId data);
};
