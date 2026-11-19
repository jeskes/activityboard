#include "uartcontroller.h"

#include "../base/base.h"

StreamController::StreamController(Stream& stream)
    : stream(stream) {
}

void StreamController::setHandler(StreamCommandHandler* handler) {
  this->handler = handler;
}

void StreamController::loop() {
  if (stream.available()) {
    String message = stream.readStringUntil('\n');
    message.trim();
    if (handler) {
      int separated = message.indexOf(':');
      if (separated != -1) {
        String cmd = message.substring(0, separated);
        cmd.trim();
        String params = message.substring(separated + 1);
        params.trim();
        handler->handleStreamCommand(cmd.c_str(), params.c_str());
      }
      else {
        handler->handleStreamCommand(message.c_str(), "");
      }
    }
  }
}
UartController::UartController(HardwareSerial& serial)
    : StreamController(serial){
}

void UartController::setup(int8_t rx, int8_t tx) {
  getSerial().begin(115200, SERIAL_8N1, rx, tx);
  LOG("uart controller initialized; rx=%d, tx=%d", rx, tx);
}

void UartController::send(StreamCommand cmd, const char* params) {
  char buffer[STREAM_COMMAND_BUFFER_SIZE];
  const char* args = params ? params : "";
  snprintf(buffer, STREAM_COMMAND_BUFFER_SIZE, "%s:%s", cmd, args);
  getSerial().println(buffer);
}
