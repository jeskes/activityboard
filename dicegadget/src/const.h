#pragma once

#include <base/base.h>

/* client-id of this gadget */
inline constexpr ClientId DICEGADGET_ID = 11;

/* startup blinking rate */
inline constexpr uint16_t BLINK_RATE = 50;

/* core status irq */
inline constexpr uint8_t CORE_STATUS_IRQ = 2;

/* button pins */
inline constexpr uint8_t START_BUTTON = 3;
inline constexpr uint8_t GUESS_BUTTON = 4;
inline constexpr uint8_t JINGLE_BUTTON = 5;

/* indicator pins for 1..6 */
inline constexpr uint8_t INDICATOR_LED_1 = 7;
inline constexpr uint8_t INDICATOR_LED_2 = 8;
inline constexpr uint8_t INDICATOR_LED_3 = 9;
inline constexpr uint8_t INDICATOR_LED_4 = 10;
inline constexpr uint8_t INDICATOR_LED_5 = 11;
inline constexpr uint8_t INDICATOR_LED_6 = 12;

/* noise pin for random */
inline constexpr uint8_t NOISE_PIN = A0;

/* timeouts */
inline constexpr unsigned long INDICATING_TIMEOUT = 5000;
inline constexpr unsigned long JINGLE_PLAYING_TIMEOUT = 12000;
inline constexpr unsigned long NUMBER_PLAYING_TIMEOUT = 15000;
inline constexpr unsigned long AUTORESET_TIMEOUT = 20000;
inline constexpr unsigned long RESET_DISPLAY_TIMEOUT = 10000;

/* languages: 00x english, 01x bavarian, 02x russian, 03x german */
inline constexpr long LANGUAGES_COUNT = 4;

/* jingle sounds: 101..103 */
inline constexpr uint8_t JINGLES_COUNT = 3;
inline constexpr uint8_t JINGLE_START = 101;
inline constexpr uint8_t INITIAL_JINGLE = 0;

/* message identifiers */
inline constexpr MessageId WELCOME_MESSAGE = "WELCOME";
inline constexpr MessageId GUESS_MESSAGE = "GUESS";
inline constexpr MessageId JINGLE_MESSAGE = "JINGLE";
inline constexpr MessageId STARTED_MESSAGE = "STARTED";
inline constexpr MessageId NUMBER_MESSAGE = "NUMBER";
inline constexpr MessageId MATCH_MESSAGE = "MATCH";
inline constexpr MessageId SORRY_MESSAGE = "SORRY";

/* image identifiers */
inline constexpr ImageId STARTUP_IMAGE = "STARTUP";

/* parameter identifiers */
inline constexpr ParameterId NUMBER_PARAMETER = "number";
inline constexpr ParameterId GUESS_PARAMETER = "guess";
inline constexpr ParameterId JINGLE_PARAMETER = "jingle";

/* track identifiers */
inline constexpr TrackId STARTUP_TRACK = 101;

namespace DiceInput {
	inline constexpr Input JINGLE = StdInput::CUSTOM + 0;
	inline constexpr Input NEXT_JINGLE = StdInput::CUSTOM + 1;
	inline constexpr Input NEXT_GUESS = StdInput::CUSTOM + 2;
}

inline constexpr unsigned long WEBINPUT_INTERVAL = 1000;