#pragma once

#include <ablib.h>

#define MAX_STORIES 3

/* sd card module */

#define SD_CS   SS
#define SD_MOSI MOSI
#define SD_MISO MISO
#define SD_CLK  SCK

/* pins */

#define START_BUTTON  32
#define SELECT_BUTTON 33
#define SELECTING_LED 25
#define READING_LED   26

/* global constants */

const BitmapId BITMAP_STARTUP = "STARTUP";
const long SELECTION_TIMEOUT = 10000;

/* global types */

using StoryId = const char*;
using ChapterId = const char*;
