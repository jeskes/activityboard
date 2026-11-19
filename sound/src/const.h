#pragma once

#include <Arduino.h>

/* MAX98357A (I2S Audio) */

inline constexpr uint8_t AUDIO_LRC = 3;
inline constexpr uint8_t AUDIO_BCLK = 1;
inline constexpr uint8_t AUDIO_DIN = 0;

// SD-Card Modul (SPI)

inline constexpr uint8_t SD_SCK = 4;
inline constexpr uint8_t SD_MISO = 5;
inline constexpr uint8_t SD_MOSI = 6;
inline constexpr uint8_t SD_CS = 7;

// communication with core

inline constexpr uint8_t UART_RX = 20;      // connected to Core GPIO 25 (TX)
inline constexpr uint8_t UART_TX = 21;      // connected to Core GPIO 26 (RX)
inline constexpr uint8_t AUDIO_BUSY = 10;  // LOW = playing, HIGH = idle
