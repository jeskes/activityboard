#ifndef _AB_CONST
#define _AB_CONST

#include <ablib.h>

#define MAX_STORIES 3
#define MAX_TITLE_LENGTH 32

/* sd card module */

#define SD_CS   SS
#define SD_MOSI MOSI
#define SD_MISO MISO
#define SD_CLK  SCK

/* button pins */

#define BUTTON_1 27
#define BUTTON_2 14
#define BUTTON_3 12
#define BUTTON_4 13

/* indicator pins */

#define LED_1 32
#define LED_2 33
#define LED_3 25
#define LED_4 26

/* global constants */

const BitmapId BITMAP_STARTUP = "STARTUP";

#endif
