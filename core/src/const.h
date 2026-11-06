#pragma once

/* wire1 */

#define WIRE1_SDA 32
#define WIRE1_SCL 33

/* I/O expand modules on wire1 */

#define ROTARY_EXPANSION_ID  0x20
#define ROTARY_EXPANSION_INT 36

#define EXTRAS_EXPANSION_ID  0x21
#define EXTRAS_EXPANSION_INT 39
#define EXTRAS_WIFI_BUTTON   0
#define EXTRAS_WIFI_LED      1

/* wifi */

#define WIFI_ACTIVATION_TIMEOUT 10000
#define WIFI_ACTIVATION_POLLING 500

/* tft display */

#define TFT_SCK 16
#define TFT_MISO (-1)
#define TFT_MOSI 13
#define TFT_CS 15
#define TFT_DC 2
#define TFT_RST 4
#define TFT_BL 17

#define BRIGHTNESS_STEP 40L
#define INITIAL_BRIGHTNESS 180L
#define MINIMUM_BRIGHTNESS 20L
#define MAXIMUM_BRIGHTNESS 255L

/* chunk for bitmaps : 320 cols, 8 rows, 16 bit (2 byte) color rgb565be */
const unsigned int DISPLAY_BUFFER_SIZE = 320 * 8 * 2;

/* dfplayer */

#define PLAYER_TX 25
#define PLAYER_RX 26
#define PLAYER_BUSY 27

#define INITIAL_VOLUME 1L
#define VOLUMNE_STEP   1L
#define MINIMUM_VOLUME 1L
#define MAXIMUM_VOLUME 30L

/* sd card */

#define SD_SCK 18
#define SD_MISO 19
#define SD_MOSI 23
#define SD_CS 5

/* led ring */

#define LEDRING_DI 14
#define LEDRING_LED_COUNT 16
#define LEDRING_RING_COUNT 2
#define LEDRING_BRIGHTNESS 40

/* startup */

#define STARTUP_TRACK_ID 100
#define STARTUP_BITMAP "/10/bitmaps/startup.raw"
