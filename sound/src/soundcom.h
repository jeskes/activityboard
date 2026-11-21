#pragma once

#include <uart/uartcontroller.h>

namespace SoundCom {
inline constexpr StreamCommand INFO = "INFO";
inline constexpr StreamCommand ERROR = "ERROR";
inline constexpr StreamCommand PLAY = "PLAY";
inline constexpr StreamCommand PLAYING = "PLAYING";
inline constexpr StreamCommand STOP = "STOP";
inline constexpr StreamCommand VOLUME = "VOLUME";
inline constexpr StreamCommand START_MAINTENANCE = "START_MAINTENANCE";
inline constexpr StreamCommand STOP_MAINTENANCE = "STOP_MAINTENANCE";
};  // namespace SoundCom
