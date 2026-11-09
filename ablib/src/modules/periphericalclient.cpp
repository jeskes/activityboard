#include "periphericalclient.h"

PeriphericalClient::PeriphericalClient(ClientId client)
    : client(client) {
}

bool PeriphericalClient::sendRequest(ModuleId module, BaseRequest* request, uint8_t length) {
  if (request->type != RequestType::STATUS) {
    // DUMP(request, length);
  }

  Wire.beginTransmission(module);
  Wire.write((uint8_t*)request, length);
  Wire.endTransmission(true);

  delay(50);

  return true;
}

bool PeriphericalClient::requestStatus(ModuleId module, StatusType statusType, BaseStatus* status, uint8_t length) {
	return requestStatus(module, client, statusType, status, length);
}

bool PeriphericalClient::requestStatus(ModuleId module, ClientId client, StatusType statusType, BaseStatus* status, uint8_t length) {
  StatusRequest request(client, statusType);
  sendRequest(module, &request, sizeof(StatusRequest));

  int received = Wire.requestFrom(module, length);
  if (received == length) {
    uint8_t* buffer = (uint8_t*)status;
    for (int idx = 0; idx < length; idx++) {
      buffer[idx] = Wire.read();
    }
    // DUMP(status, length);
  }
  while (Wire.available()) {
    Wire.read();
  }

  return received == length;
}
