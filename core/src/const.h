#pragma once

/* tft display */

#define TFT_SCK 16
#define TFT_MISO (-1)
#define TFT_MOSI 13
#define TFT_CS 15
#define TFT_DC 2
#define TFT_RST 4
#define TFT_BL 17

#define BRIGHTNESS_CLK 34
#define BRIGHTNESS_DT 35
#define BRIGHTNESS_SW 33

#define INITIAL_BRIGHTNESS 128
#define MINIMUM_BRIGHTNESS 50
#define MAXIMUM_BRIGHTNESS 255

/* dfplayer (RX must be protected by 1k resistence) */

#define PLAYER_TX 25
#define PLAYER_RX 26
#define PLAYER_BUSY 27

#define VOLUME_CLK 36
#define VOLUME_DT 39
#define VOLUME_SW 32

#define INITIAL_VOLUME 1
#define MINIMUM_VOLUME 0
#define MAXIMUM_VOLUME 25

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
#define STARTUP_BITMAP "/10/startup.raw"
