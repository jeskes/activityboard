#pragma once

#include <ablib.h>

#define MAX_STORIES 3

/* sd card module */

#define SD_CS   SS
#define SD_MOSI MOSI
#define SD_MISO MISO
#define SD_CLK  SCK

/* local pins */

#define START_BUTTON  32
#define SELECT_BUTTON 33
#define SELECTING_LED 25
#define READING_LED   26

/* master select rotary encoder behind PCF8574 pins (attach INT to pin 25) */

#define MASTERSELECT_MODULE_ID 0x21
#define MASTERSELECT_INTERRUPT 25

#define MASTERSELECT_ENCODER_CLK P0
#define MASTERSELECT_ENCODER_DT  P1
#define MASTERSELECT_ENCODER_SW  P2

/* global constants */

const BitmapId BITMAP_STARTUP = "STARTUP";
const long SELECTION_TIMEOUT = 10000;

/* global types */

using StoryId = const char*;
using ChapterId = const char*;
