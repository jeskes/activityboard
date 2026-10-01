#include <Arduino.h>
#include <Wire.h>

#include "ablib.h"

#define POLLING_TIMEOUT 500

static void pollSoundStatus();
static SoundStatusHandler* statusHandler = nullptr;
static PlayerState lastPlayerState = PlayerState::UNKNOWN;
static Task statusPollingTask(POLLING_TIMEOUT, TASK_FOREVER, &pollSoundStatus);

ActivitySoundClient::ActivitySoundClient(Scheduler* scheduler) {
  scheduler->addTask(statusPollingTask);
}

void ActivitySoundClient::setup(uint16_t folderId) {
  LOG("setuip sound module : folder=%d.", folderId);
  SetupSoundRequest request(folderId);
  sendRequest(SOUND_MODULE_ID, &request, sizeof(SetupSoundRequest));
}

void ActivitySoundClient::play(uint16_t trackId, SoundStatusHandler* handler) {
  LOG("send play sound request : track=%d.", trackId);
  setStatusHandler(handler);

  PlaySoundRequest request(trackId);
  sendRequest(SOUND_MODULE_ID, &request, sizeof(PlaySoundRequest));
}

void ActivitySoundClient::stop() {
  LOG("send play stop request.");
  StopSoundRequest request;
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