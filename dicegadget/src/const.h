#pragma once

/* startup blinking rate */
#define BLINK_RATE 50

/* noise pine for randim */
#define NOISE_PIN A0

/* start button pin */
#define START_BUTTON 10

/* indicator pins for 1..6 */
#define INDICATOR_LED_1 2
#define INDICATOR_LED_2 3
#define INDICATOR_LED_3 4
#define INDICATOR_LED_4 5
#define INDICATOR_LED_5 6
#define INDICATOR_LED_6 7

/* timeouts (just in case something is blocking) */
#define INDICATING_TIMEOUT 1000
#define JINGLE_PLAYING_TIMEOUT 12000
#define NUMBER_PLAYING_TIMEOUT 15000
#define AUTORESET_TIMEOUT 20000

/* timeout to reset display after a throw */
#define RESET_DISPLAY_TIMEOUT 5000

/* languages: 00x english, 01x bavarian, 02x russian, 03x german */
#define LANGUAGES_COUNT 4

/* jingle sounds: 101 ... 110 */
#define JINGLES_COUNT 10
#define INITIAL_JINGLE 101

/* client-id of this gadget */
const ClientId DICEGADGET_ID = 11;

/* display identifiers */
const MessageId MESSAGE_WELCOME = "WELCOME";
const MessageId MESSAGE_GUESS = "GUESS";
const MessageId MESSAGE_STARTED = "STARTED";
const MessageId MESSAGE_NUMBER = "NUMBER";
const MessageId MESSAGE_MATCH = "MATCH";
const MessageId MESSAGE_SORRY = "SORRY";
const BitmapId BITMAP_STARTUP = "STARTUP";
const ParameterId PARAMETER_NUMBER = "number";
const ParameterId PARAMETER_GUESS = "guess";
