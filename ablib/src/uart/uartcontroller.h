#pragma once
#if defined(AB_UARTCONTROLLER)

#include <HardwareSerial.h>

using StreamCommand = const char*;

inline constexpr uint8_t STREAM_COMMAND_BUFFER_SIZE = 64;

class StreamCommandHandler {
 public:
  virtual void handleStreamCommand(StreamCommand cmd, const char* params) = 0;
};

class StreamController {
 public:
  StreamController(Stream& stream);

  void loop();
  void setHandler(StreamCommandHandler* handler);

 protected:
  Stream& stream;
  StreamCommandHandler* handler;
};

class UartController : public StreamController {
 public:
  UartController(HardwareSerial& serial);

  void setup(int8_t rx, int8_t tx);

  void send(StreamCommand cmd, const char* params = nullptr);

 private:
  HardwareSerial& getSerial() {
    return *((HardwareSerial*)&stream);
  }
};
#endif