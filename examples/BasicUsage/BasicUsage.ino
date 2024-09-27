#include <HinbitConfig.h>

HinbitConfig config;

void setup() {
  Serial.begin(115200);
  config.begin();
  
  // Activate BLE
  config.setBLEDeviceActive();
  
  // Print the current configuration
  config.printConfig();
  
  // Set a new configuration value
  config.writeConfig("wifi-ssid", "MyWiFiNetwork");
  config.writeConfig("wifi-password", "MyWiFiPassword");
  
  // List files in LittleFS
  config.listLittleFSFiles();
}

void loop() {
  config.handleBLERequests();
}
