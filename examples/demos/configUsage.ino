#include <HinbitConfig.h>

HinbitConfig hinbitConfig;

void setup() {
  Serial.begin(115200);
  hinbitConfig.begin();

  Serial.println("BLE Config Test Initialized");
  Serial.println("Use a BLE terminal to send commands like:");
  Serial.println("SET,ConfigKey,ConfigValue");
  Serial.println("GET,ConfigKey");
  Serial.println("LIST");

  // LittleFS test
  hinbitConfig.listLittleFSFiles(); // List all files
}

void loop() {
  // Handle BLE requests (SET, GET, LIST)
  hinbitConfig.handleBLERequests();

  delay(1000);
}
