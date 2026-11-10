#pragma once

#include "base/base.h"

/* core status irq */

inline constexpr uint8_t CORE_STATUS_IRQ = 19;

/* wire1 */

inline constexpr uint8_t WIRE1_SDA = 32;
inline constexpr uint8_t WIRE1_SCL = 33;

/* I/O expand modules on wire1 */

inline constexpr uint8_t ROTARY_EXPANSION_ID = 0x20;
inline constexpr uint8_t ROTARY_EXPANSION_INT = 36;

inline constexpr uint8_t EXTRAS_EXPANSION_ID = 0x21;
inline constexpr uint8_t EXTRAS_EXPANSION_INT = 39;
inline constexpr uint8_t EXTRAS_WIFI_BUTTON = 0;
inline constexpr uint8_t EXTRAS_WIFI_LED = 1;

/* wifi */

inline constexpr unsigned long WIFI_ACTIVATION_POLLING = 20;
inline constexpr unsigned long WIFI_ACTIVE_POLLING = 5;

/* tft display */

inline constexpr int8_t TFT_CLK = 16;
inline constexpr int8_t TFT_MISO = (-1);
inline constexpr int8_t TFT_MOSI = 13;
inline constexpr int8_t TFT_CS = 15;
inline constexpr int8_t TFT_DC = 2;
inline constexpr int8_t TFT_RST = 4;
inline constexpr int8_t TFT_BL = 17;

inline constexpr long BRIGHTNESS_STEP = 40;
inline constexpr long INITIAL_BRIGHTNESS = 180;
inline constexpr long MINIMUM_BRIGHTNESS = 20;
inline constexpr long MAXIMUM_BRIGHTNESS = 255;

/* chunk for images : 320 cols, 8 rows, 16 bit (2 byte) color rgb565be */
inline constexpr unsigned int DISPLAY_BUFFER_SIZE = 320 * 8 * 2;

/* dfplayer */

inline constexpr uint8_t PLAYER_TX = 25;
inline constexpr uint8_t PLAYER_RX = 26;
inline constexpr uint8_t PLAYER_BUSY = 27;

inline constexpr long INITIAL_VOLUME = 1;
inline constexpr long VOLUMNE_STEP = 1;
inline constexpr long MINIMUM_VOLUME = 1;
inline constexpr long MAXIMUM_VOLUME = 30;

/* sd card */

inline constexpr uint8_t SD_CLK = 18;
inline constexpr uint8_t SD_MISO = 34;
inline constexpr uint8_t SD_MOSI = 23;
inline constexpr uint8_t SD_CS = 5;

/* led ring */

inline constexpr uint8_t LEDRING_DI = 14;
inline constexpr int LEDRING_LED_COUNT = 16;
inline constexpr int LEDRING_RING_COUNT = 2;
inline constexpr uint8_t LEDRING_BRIGHTNESS = 40;

/* startup */

inline constexpr TrackId STARTUP_TRACK_ID = 100;
inline constexpr const char* STARTUP_IMAGE = "/10/images/startup.raw";
