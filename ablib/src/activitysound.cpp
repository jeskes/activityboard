#include <Arduino.h>
#include <Wire.h>

#include "ablib.h"

#define POLLING_TIMEOUT 500

static void pollSoundStatus();
static SoundStatusHandler* statusHandler = nullptr;
static PlayerState lastPlayerState = PlayerState::UNKNOWN;
static Task statusPollingTask(POLLING_TIMEOUT, TASK_FOREVER, &pollSoundStatus);

ActivitySoundClient::ActivitySoundClient(ClientId client, Scheduler& scheduler)
    : PeriphericalClient(client) {
  scheduler.addTask(statusPollingTask);
}

void ActivitySoundClient::play(TrackId track, SoundStatusHandler* handler) {
  LOG("send play sound request : track=%d.", track);
  setStatusHandler(handler);

  PlaySoundRequest request(client, track);
  sendRequest(SOUND_MODULE_ID, &request, sizeof(PlaySoundRequest));
}

void ActivitySoundClient::stop() {
  LOG("send play stop request.");
  StopSoundRequest request(client);
  sendRequest(SOUND_MODULE_ID, &request, sizeof(StopSoundRequest));
}

void ActivitySoundClient::setStatusHandler(SoundStatusHandler* handler) {
  statusHandler = handler;
  lastPlayerState = PlayerState::UNKNOWN;
  if (handler) {
    statusPollingTask.enable();
  }
  else {
    statusPollingTask.disable();
  }
}

static void pollSoundStatus() {
  if (statusHandler) {
    SoundStatus status;
    PeriphericalClient::requestStatus(SOUND_MODULE_ID, &status, sizeof(SoundStatus));

    // LOG("ActivitySoundClient: received status from sound-service: player-state=%d", status.playerState);

    if (lastPlayerState != status.playerState) {
      lastPlayerState = status.playerState;
      bool cancelHandler = statusHandler->playerStateChanged(lastPlayerState);
      if (cancelHandler) {
        statusHandler = nullptr;
        statusPollingTask.disable();
      }
    }
  }
  else {
    statusPollingTask.disable();
  }
}