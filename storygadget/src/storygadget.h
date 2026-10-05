#ifndef _AB_STORY_GADGET_H
#define _AB_STORY_GADGET_H

#include <ablib.h>

#include "const.h"
#include "storage.h"
#include "storycontroller.h"

const ClientId STORYGADGET_ID = 12;

class StoryGadget : public GadgetBase {
 public:
  StoryGadget();

  void setup();
  void loop();

  Storage storage;
  ActivitySoundClient sound;
  ActivityNumpadClient numpad;
  ActivityDisplayClient display;

 private:
  void loadStories();
  JsonDocument storiesDocument;

  const char* stories[MAX_STORIES];
  const char* titles[MAX_STORIES];

  StoryController controller;
};

#endif