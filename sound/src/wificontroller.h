#pragma once

#include <SimpleFTPServer.h>

class WifiController {
 public:
  WifiController();

  void setup();
  void loop();

  bool start();
  bool stop();

  bool started;

 private:
  FtpServer ftpServer;
};
