#pragma once

#include <base/base.h>

/* client-id of this gadget */
inline constexpr ClientId MUSICGADGET_ID = 13;

/* startup blinking rate */
inline constexpr uint16_t BLINK_RATE = 50;

/* core status irq */
inline constexpr uint8_t CORE_STATUS_IRQ = 2;

/* button pins */
inline constexpr uint8_t START_BUTTON = 4;
inline constexpr uint8_t REPEAT_BUTTON = 5;
inline constexpr uint8_t NEXT_BUTTON = 6;
inline constexpr uint8_t STOP_BUTTON = 7;

/* indicator pins */
inline constexpr uint8_t INDICATOR_PLAYING = 8;

/* category switch */
inline constexpr uint8_t CATEGORY_SWITCH_ID = GADGET_EXPANSION_ID1;
inline constexpr uint8_t CATEGORY_SWITCH_COUNT = 4;

/* image identifiers */
inline constexpr ImageId STARTUP_IMAGE = "STARTUP";

/* track identifiers */
inline constexpr TrackId STARTUP_TRACK = 1;

/* rc522 module pins */
inline constexpr uint8_t RC522_RST = 9;
inline constexpr uint8_t RC522_SS = 10;
inline constexpr uint8_t RC522_MOSI = 11;
inline constexpr uint8_t RC522_MISO = 12;
inline constexpr uint8_t RC522_SCK = 13;

inline constexpr uint8_t MAX_RFID_UID_SIZE = 10;

/* reset timeout after not playing anymore */
inline constexpr unsigned long NOTPLAYING_RESET_TIMEOUT = 5000;