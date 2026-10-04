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

  static PlayerState playerState;
  static void publishStatus();

  void setup(uint8_t module) override;
  void loop() override;

  void processRequest(BaseRequest* request) override;

  void play(ClientId client, TrackId track);
  void stop();

 private:
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