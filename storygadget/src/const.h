#pragma once

#include <base/base.h>

/* client-id of this gadget */
const ClientId STORYGADGET_ID = 12;

/* core status irq */
inline constexpr uint8_t CORE_STATUS_IRQ = 36;

/* pins */

inline constexpr uint8_t START_BUTTON = 26;
inline constexpr uint8_t REPEAT_BUTTON = 25;
inline constexpr uint8_t NEXT_BUTTON = 33;
inline constexpr uint8_t STOP_BUTTON = 32;

inline constexpr uint8_t SELECTING_LED = 13;
inline constexpr uint8_t READING_LED = 14;
inline constexpr uint8_t MENU_LED = 27;

/* rc522 module pins */

inline constexpr uint8_t RC522_RST = 4;
inline constexpr uint8_t RC522_SS = 5;
inline constexpr uint8_t RC522_MOSI = 23;
inline constexpr uint8_t RC522_MISO = 19;
inline constexpr uint8_t RC522_SCK = 18;

/* global constants */

#define MAX_STORIES 10

inline constexpr MessageId LOADING_STORY = "LOADING_STORY";
inline constexpr ImageId STARTUP_IMAGE = "STARTUP";
inline constexpr TrackId STARTUP_TRACK = 1;

inline constexpr DataId STORIES_DATA = "stories";

inline constexpr unsigned long SELECTION_TIMEOUT = 10000;

/* global types */

using StoryId = const char*;
using ChapterId = const char*;