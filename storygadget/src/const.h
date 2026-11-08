#pragma once

#include <base/base.h>

/* client-id of this gadget */
const ClientId STORYGADGET_ID = 12;

/* pins */

#define START_BUTTON   26
#define REPEAT_BUTTON  25
#define NEXT_BUTTON    33
#define STOP_BUTTON    32

#define SELECTING_LED 13
#define READING_LED   14
#define MENU_LED 27

/* global constants */

#define MAX_STORIES 10

const MessageId LOADING_STORY = "LOADING_STORY";
const BitmapId STARTUP_BITMAP = "STARTUP";
const TrackId STARTUP_TRACK = 1;

const DataId STORIES_DATA = "stories";

const unsigned long SELECTION_TIMEOUT = 10000;

/* global types */

using StoryId = const char*;
using ChapterId = const char*;

