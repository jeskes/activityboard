#pragma once

#include <base/base.h>
#include <modules/requests.h>
#include <uart/uartcontroller.h>

#include <functional>

#include "ledring.h"
#include "rotaryexpansion.h"

using SoundStateChangeHandler = std::function<void()>;
using SoundPlayingHandler = std::function<void(TrackPath path)>;

class SoundController : public StreamCommandHandler {
 public:
  SoundController(LedRing& ledRing, RotaryExpansion& rotary);

  void setup();
  void loop();

  void play(TrackPath path);
  void stop();

  void startMaintenance();
  void stopMaintenance();

  PlayerState playerState;
  void onPlaying(SoundPlayingHandler handler);
  void onStateChange(SoundStateChangeHandler handler);

  void handleStreamCommand(StreamCommand cmd, const char* params) override;

 private:
  bool mute;
  int volume;

  LedRing& ledRing;
  RotaryAdapter volumeAdapter;
  UartController uartController;

  bool checkPlaying();
  void awaitBusy(bool playing);
  void setVolume(int volume);
  void toggleMute();

  bool debouncePlaying;
  unsigned long debounceTimestamp;
  SoundPlayingHandler playingHandler;
  SoundStateChangeHandler stateChangeHandler;
};
