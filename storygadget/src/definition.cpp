#include "definition.h"

#include "const.h"

/* (stories) definition */

Definition::Definition(ActivityCoreClient& core)
    : core(core) {
}

void Definition::setup() {
  load();
}

void Definition::load() {
  DataStream stream = core.readData(STORIES_DATA);
  DeserializationError error = deserializeJson(jsonDocument, stream);
  if (error) {
    LOG("cannot load stories: error=%s", error.c_str());
    return;
  }

  storiesCount = 0;
  JsonObject entries = jsonDocument.as<JsonObject>();
  for (JsonPair entry : entries) {
    if (storiesCount < MAX_STORIES) {
      StoryId storyId = entry.key().c_str();
      JsonObject storyNode = entry.value().as<JsonObject>();
      stories[storiesCount] = storyId;
      StoryDef& storyDef = storyDefs[storyId];
      storyDef.id = storyId;
      storyDef.folder = storyNode["folder"].as<uint8_t>();
      storyDef.rfid = storyNode["rfid"].as<RfidId>();
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

StoryId Definition::getStoryIdByRfid(RfidId rfid) {
  for (const auto& entry : storyDefs) {
    if (!strcmp(entry.second.rfid, rfid)) {
      return entry.second.id;
    }
  }
  return nullptr;
}

StoryDef* Definition::getStory(int index) {
  return getStory(getStoryId(index));
}

StoryDef* Definition::getStory(StoryId story) {
  StoryDef* result = nullptr;
  const auto& found = storyDefs.find(story);
  if (found != storyDefs.end()) {
    result = &found->second;
  }
  return result;
}

/* story definition */

void StoryDef::assertLoaded(ActivityCoreClient& core) {
  if (!jsonDocument.isNull()) {
    return;
  }

  char path[ID_BUFFER_SIZE];
  snprintf(path, ID_BUFFER_SIZE, "/%02d/definition.json", folder);
  LOG("loading story definition: path=%s", path);
  core.displayMessage(LOADING_STORY);
  DataStream stream = core.readData(path);
  DeserializationError error = deserializeJson(jsonDocument, stream);
  if (error) {
    LOG("cannot load story %s: error=%s", id, error.c_str());
    return;
  }

  LOG("story definition loaded");
  Serial.println();
  serializeJsonPretty(jsonDocument, Serial);
  Serial.println();

  JsonObject chapters = jsonDocument["chapters"].as<JsonObject>();
  for (JsonPair entry : chapters) {
    ChapterId chapterId = entry.key().c_str();
    JsonObject chapterNode = entry.value().as<JsonObject>();
    ChapterDef& chapterDef = chapterDefs[chapterId];
    chapterDef.id = chapterId;
    chapterDef.storyDef = this;
    chapterDef.jsonNode = chapterNode;
  }

  JsonObject menus = jsonDocument["menus"].as<JsonObject>();
  for (JsonPair entry : menus) {
    MenuId menuId = entry.key().c_str();
    JsonObject menuNode = entry.value().as<JsonObject>();
    MenuDef& menuDef = menuDefs[menuId];
    menuDef.id = menuId;
    menuDef.storyDef = this;
    menuDef.jsonNode = menuNode;
  }
}

StoryId StoryDef::getId() {
  return id;
}

ChapterId StoryDef::getStart() {
  return jsonDocument["start"].as<ChapterId>();
}

ChapterDef* StoryDef::getChapter(ChapterId id) {
  auto it = chapterDefs.find(id);
  if (it != chapterDefs.end()) {
    return &it->second;
  }
  return nullptr;
}

MenuDef* StoryDef::getMenu(MenuId id) {
  auto it = menuDefs.find(id);
  if (it != menuDefs.end()) {
    return &it->second;
  }
  return nullptr;
}

ImagePath StoryDef::getCoverImagePath() {
  static char path[ID_BUFFER_SIZE];
  snprintf(path, ID_BUFFER_SIZE, "/%02d/cover.jpg", folder);
  return path;
}

TrackPath StoryDef::getTeaserTrackPath() {
  static char path[ID_BUFFER_SIZE];
  snprintf(path, ID_BUFFER_SIZE, "/%02d/teaser.mp3", folder);
  return path;
}

/* chapter definition */

ChapterId ChapterDef::getId() {
  return id;
}
ImagePath ChapterDef::getImage() {
  static char path[ID_BUFFER_SIZE];
  const char* relative = jsonNode["image"].as<const char*>();
  snprintf(path, ID_BUFFER_SIZE, "/%02d/%s", storyDef->folder, relative);
  return path;
}

TrackPath ChapterDef::getTrack() {
  static char path[ID_BUFFER_SIZE];
  const char* relative = jsonNode["track"].as<const char*>();
  snprintf(path, ID_BUFFER_SIZE, "/%02d/%s", storyDef->folder, relative);
  return path;
}

MenuId ChapterDef::getMenu() {
  return jsonNode["menu"].as<MenuId>();
}

ChapterId ChapterDef::getNext() {
  return jsonNode["next"].as<ChapterId>();
}

/* menu definition */

MenuId MenuDef::getId() {
  return id;
}

size_t MenuDef::getMenuSize() {
  return jsonNode["options"].as<JsonArray>().size();
}

TrackPath MenuDef::getTrack() {
  static char path[ID_BUFFER_SIZE];
  const char* relative = jsonNode["track"].as<const char*>();
  snprintf(path, ID_BUFFER_SIZE, "/%02d/%s", storyDef->folder, relative);
  return path;
}

TrackPath MenuDef::getOptionTrack(uint8_t index) {
  static char path[ID_BUFFER_SIZE];
  const char* relative = jsonNode["options"].as<JsonArray>()[index]["track"].as<const char*>();
  snprintf(path, ID_BUFFER_SIZE, "/%02d/%s", storyDef->folder, relative);
  return path;
}

ChapterId MenuDef::getOptionNext(uint8_t index) {
  return jsonNode["options"].as<JsonArray>()[index]["next"].as<ChapterId>();
}
