#pragma once

#include <core/core.h>

/* client-id of this gadget */
const ClientId DICEGADGET_ID = 11;

/* startup blinking rate */
#define BLINK_RATE 50

/* noise pine for randim */
#define NOISE_PIN A0

/* button pins */
#define START_BUTTON 2
#define GUESS_BUTTON 3
#define JINGLE_BUTTON 4

/* indicator pins for 1..6 */
#define INDICATOR_LED_1 7
#define INDICATOR_LED_2 8
#define INDICATOR_LED_3 9
#define INDICATOR_LED_4 10
#define INDICATOR_LED_5 11
#define INDICATOR_LED_6 12

/* timeouts */
#define INDICATING_TIMEOUT 5000
#define JINGLE_PLAYING_TIMEOUT 12000
#define NUMBER_PLAYING_TIMEOUT 15000
#define AUTORESET_TIMEOUT 20000
#define RESET_DISPLAY_TIMEOUT 10000

/* languages: 00x english, 01x bavarian, 02x russian, 03x german */
#define LANGUAGES_COUNT 4

/* jingle sounds: 101..103 */
#define JINGLES_COUNT 3
#define JINGLE_START 101
#define INITIAL_JINGLE 0

/* message identifiers */
const MessageId WELCOME_MESSAGE = "WELCOME";
const MessageId GUESS_MESSAGE = "GUESS";
const MessageId JINGLE_MESSAGE = "JINGLE";
const MessageId STARTED_MESSAGE = "STARTED";
const MessageId NUMBER_MESSAGE = "NUMBER";
const MessageId MATCH_MESSAGE = "MATCH";
const MessageId SORRY_MESSAGE = "SORRY";

/* bitmap identifiers */
const BitmapId STARTUP_BITMAP = "STARTUP";

/* parameter identifiers */
const ParameterId NUMBER_PARAMETER = "number";
const ParameterId GUESS_PARAMETER = "guess";
const ParameterId JINGLE_PARAMETER = "jingle";

/* track identifiers */
const TrackId STARTUP_TRACK = 101;
