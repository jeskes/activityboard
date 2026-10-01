#ifndef _AB_SOUND_SERVICE
#define _AB_SOUND_SERVICE

#include <DFRobotDFPlayerMini.h>
#include <Encoder.h>
#include <SoftwareSerial.h>
#include <ablib.h>

class SoundService : public PeriphericalService {
 public:
  SoundService();

  bool currentMute = false;
  bool pendingMute = false;

  int currentVolume;
  int pendingVolume;

  PlayerState playerState;

  void setup(uint8_t module) override;
  void loop() override;

  void processRequest(BaseRequest* request) override;
  void publishStatus() override;

  void clientSetup(uint16_t folderId);
  void play(uint16_t trackId);
  void stop();

 private:
  uint16_t folderId;
  Encoder volumeEncoder;
  DFRobotDFPlayerMini player;
  SoftwareSerial playerSerial;

  void setupPlayer();
  bool checkPlayer();
  void readPlayerVolume();
  void updatePlayerVolume();
  void awaitPlayerBusy(bool busy);
};

#endif