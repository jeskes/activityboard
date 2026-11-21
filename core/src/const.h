#pragma once

#include <base/base.h>

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
inline constexpr uint8_t EXTRAS_STOP_ALL_BUTTON = 2;
inline constexpr uint8_t EXTRAS_MAINENANCE_BUTTON = 3;
inline constexpr uint8_t EXTRAS_MAINENANCE_LED = 4;

inline constexpr uint8_t SWITCH_EXPANSION_ID = 0x22;

/* tft display */

inline constexpr int8_t TFT_CLK = 16;
inline constexpr int8_t TFT_MISO = (-1);
inline constexpr int8_t TFT_MOSI = 13;
inline constexpr int8_t TFT_CS = 15;
inline constexpr int8_t TFT_DC = 2;
inline constexpr int8_t TFT_RST = 4;
inline constexpr int8_t TFT_BL = 17;
inline constexpr int8_t TOUCH_SDA = WIRE1_SDA;
inline constexpr int8_t TOUCH_SCL = WIRE1_SCL;
inline constexpr int8_t TOUCH_INT = 34;
inline constexpr int8_t TOUCH_RST = -1;
inline constexpr int8_t TOUCH_I2C_ADDRESS = 0x38;

inline constexpr long BRIGHTNESS_STEP = 40;
inline constexpr long INITIAL_BRIGHTNESS = 180;
inline constexpr long MINIMUM_BRIGHTNESS = 20;
inline constexpr long MAXIMUM_BRIGHTNESS = 255;

/* chunk for images : 320 cols, 12 rows, 16 bit (2 byte) color rgb565be */
inline constexpr unsigned int DISPLAY_BUFFER_SIZE = 320 * 12 * 2;

/* sound system */

inline constexpr uint8_t SOUND_SYSTEM_TX = 25;    /* connectedto ESP32-C3 GPIO 20 (RX) */
inline constexpr uint8_t SOUND_SYSTEM_RX = 26;    /* connected to ESP32-C3 GPIO 21 (TX) */
inline constexpr uint8_t SOUND_SYSTEM_BUSY = 27; /* connected to ESP32-C3 GPIO 10 (BUSY) */

inline constexpr long VOLUMNE_STEP = 5;
inline constexpr long INITIAL_VOLUME = 5;
inline constexpr long MINIMUM_VOLUME = 5;
inline constexpr long MAXIMUM_VOLUME = 100;

inline constexpr int PLAYER_WAIT_FOR_BUSY_COUNT = 20;
inline constexpr unsigned long PLAYER_WAIT_FOR_BUSY_TIMEOUT = 100;
inline constexpr unsigned long PLAYER_DEBOUNCE_DELAY = 100;

/* sd card */

inline constexpr uint8_t SD_CLK = 18;
inline constexpr uint8_t SD_MISO = 35;
inline constexpr uint8_t SD_MOSI = 23;
inline constexpr uint8_t SD_CS = 5;

/* led ring */

inline constexpr uint8_t LEDRING_DI = 14;
inline constexpr int LEDRING_LED_COUNT = 16;
inline constexpr int LEDRING_RING_COUNT = 2;
inline constexpr uint8_t LEDRING_BRIGHTNESS = 40;

/* startup */

inline constexpr const char* STARTUP_IMAGE = "/10/images/startup.jpg";
