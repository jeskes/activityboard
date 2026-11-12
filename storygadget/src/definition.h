#pragma once

#include <ArduinoJson.h>
#include <modules/coreclient.h>

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
  StoryId getStoryIdByRfid(RfidId rfid);
  JsonDocument& getStoryDefinition(StoryId story);

 private:
  ActivityCoreClient& core;
  JsonDocument storiesDocument;

  StoryId stories[MAX_STORIES];
  JsonObject storyInfos[MAX_STORIES];
  std::map<StoryId, JsonDocument> storyDefinitions;

  void loadStories();
  void loadStory(JsonDocument& doc, DataId data);
};