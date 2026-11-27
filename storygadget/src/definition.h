#pragma once

#include <ArduinoJson.h>
#include <modules/coreclient.h>

#include <map>

#include "const.h"

class StoryDef;
class ChapterDef;

class MenuDef {
 public:
  MenuId getId();
  size_t getMenuSize();
  TrackPath getTrack();
  TrackPath getOptionTrack(uint8_t index);
  ChapterId getOptionNext(uint8_t index);

 private:
  MenuId id;
  StoryDef* storyDef;
  JsonObject jsonNode;

  friend class StoryDef;
};

class ChapterDef {
 public:
  ChapterId getId();
  ImagePath getImage();
  TrackPath getTrack();
  MenuId getMenu();
  ChapterId getNext();

 private:
  ChapterId id;
  StoryDef* storyDef;
  JsonObject jsonNode;

  friend class StoryDef;
};

class StoryDef {
 public:
  StoryId getId();

  ChapterId getStart();
  ChapterDef* getChapter(ChapterId id);
  MenuDef* getMenu(MenuId id);

  ImagePath getCoverImagePath();
  TrackPath getTeaserTrackPath();

  void assertLoaded(ActivityCoreClient& core);

 private:
  StoryId id;
  RfidId rfid;
  uint8_t folder;
  JsonDocument jsonDocument;

  std::map<MenuId, MenuDef> menuDefs;
  std::map<ChapterId, ChapterDef> chapterDefs;

  friend class MenuDef;
  friend class ChapterDef;
  friend class Definition;
};

class Definition {
 public:
  Definition(ActivityCoreClient& core);
  void setup();

  int storiesCount;
  StoryId getStoryId(int index);
  StoryId getStoryIdByRfid(RfidId rfid);
  StoryDef* getStory(int index);
  StoryDef* getStory(StoryId story);

 private:
  ActivityCoreClient& core;
  JsonDocument jsonDocument;

  StoryId stories[MAX_STORIES];
  std::map<StoryId, StoryDef> storyDefs;

  void load();
};