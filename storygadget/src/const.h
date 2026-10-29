#pragma once

#include <core/core.h>

#define MAX_STORIES 3

/* local pins */

#define SELECT_BUTTON 27
#define START_BUTTON  26
#define REPEAT_BUTTON  25
#define SKIP_BUTTON  33
#define CANCEL_BUTTON  32
#define SELECTING_LED 13
#define READING_LED   14

/* global constants */

const BitmapId BITMAP_STARTUP = "STARTUP";
const long SELECTION_TIMEOUT = 10000;

/* global types */

using StoryId = const char*;
using ChapterId = const char*;
