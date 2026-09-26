#ifndef _AB_SOUND_SERVICE
#define _AB_SOUND_SERVICE

#include <DFRobotDFPlayerMini.h>
#include <Encoder.h>
#include <SoftwareSerial.h>
#include <ablib.h>

class SoundService : public PeriphericalService {
 public:
  SoundService();

  bool mute;
  int volume;
  PlayerState playerState;

  virtual void setup();
  virtual void loop();

  virtual void processRequest(BaseRequest* request);
  virtual void publishStatus();

  void play(int folder, int track);
  void stop();

 private:
  DFRobotDFPlayerMini player;
  Encoder volumeEncoder;
  SoftwareSerial playerSerial;

  void setupPlayer();
  bool checkPlayer();
  void updatePlayerVolume();
  void awaitPlayerBusy(bool busy);
};

#endif