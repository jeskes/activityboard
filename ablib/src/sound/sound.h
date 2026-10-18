#pragma once

#include "../base/base.h"
#include "../modules/modules.h"

/* ========================================================================= */
/*                                                                 sound api */
/* ========================================================================= */

using TrackId = uint16_t;

enum class PlayerState : uint16_t {
  UNKNOWN = 0,
  IDLE = 1,
  PLAYING = 2
};

struct __attribute__((packed)) PlaySoundRequest : public BaseRequest {
  PlaySoundRequest(ClientId client, TrackId track)
      : BaseRequest(client, RequestType::SOUND_PLAY),
        track(track) {
  }
  TrackId track;
};

struct __attribute__((packed)) StopSoundRequest : public BaseRequest {
  StopSoundRequest(ClientId client)
      : BaseRequest(client, RequestType::SOUND_STOP) {
  }
};

class SoundStatusHandler {
 public:
  virtual bool playerStateChanged(PlayerState state) = 0;
};

class ActivitySoundClient : public PeriphericalClient {
 public:
  ActivitySoundClient(ClientId client, Scheduler& scheduler);
  void play(TrackId track, SoundStatusHandler* statusHandler = nullptr);
  void stop();
  void setStatusHandler(SoundStatusHandler* handler);
};

struct __attribute__((packed)) SoundStatus : public BaseStatus {
  SoundStatus(PlayerState playerState = PlayerState::UNKNOWN)
      : BaseStatus(StatusType::SOUND_STATUS),
        playerState(playerState) {
  }
  PlayerState playerState;
};

