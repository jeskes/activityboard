#pragma once

#include <core/core.h>

/* client-id of this gadget */
const ClientId STORYGADGET_ID = 12;

/* pins */

#define START_BUTTON   26
#define REPEAT_BUTTON  25
#define NEXT_BUTTON    33
#define CANCEL_BUTTON  32

#define SELECTING_LED 13
#define READING_LED   14
#define MENU_LED 27

/* global constants */

#define MAX_STORIES 10

const BitmapId STARTUP_BITMAP = "STARTUP";
const TrackId STARTUP_TRACK = 1;

const long SELECTION_TIMEOUT = 10000;

/* global types */

using StoryId = const char*;
using ChapterId = const char*;
