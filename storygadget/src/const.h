#pragma once

#include <base/base.h>

/* client-id of this gadget */
const ClientId STORYGADGET_ID = 12;

/* core status irq */
inline constexpr uint8_t CORE_STATUS_IRQ = 4;

/* pins */

inline constexpr uint8_t START_BUTTON = 26;
inline constexpr uint8_t REPEAT_BUTTON = 25;
inline constexpr uint8_t NEXT_BUTTON = 33;
inline constexpr uint8_t STOP_BUTTON = 32;

inline constexpr uint8_t SELECTING_LED = 13;
inline constexpr uint8_t READING_LED = 14;
inline constexpr uint8_t MENU_LED = 27;

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
