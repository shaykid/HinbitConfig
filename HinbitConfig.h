#ifndef HINBITCONFIG_H
#define HINBITCONFIG_H

#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEServer.h>
#include <LittleFS.h>
#include <ArduinoJson.h>

// HinbitConfig is a class that provides BLE device activation, file handling
// using LittleFS, and config management through JSON storage.
class HinbitConfig {
public:
    // Initialize BLE and LittleFS
    void begin();

    // Activate BLE device
    void setBLEDeviceActive();

    // Handle BLE requests
    void handleBLERequests();

    // Set a configuration value by key
    void setConfigValue(String key, String value);

    // Get a configuration value by key
    String getConfigValue(String key);

    // List all configuration keys
    String listConfigKeys();

    // Start configuration mode
    bool startConfig();

    // Create a default configuration
    void createDefaultConfig();

    // Read a configuration value
    String readConfig(String key);

    // Write a configuration value
    void writeConfig(String key, String value);

    // Print a configuration value or the entire configuration
    void printConfig(String key = "");

    // List all files in the LittleFS filesystem
    void listLittleFSFiles();

    // Format the LittleFS filesystem
    void formatLittleFS();

    // Initialize the filesystem
    void initFileSystem();

private:
    // Internal BLE server and characteristic
    BLEServer *pServer;
    BLECharacteristic *pCharacteristic;

    // Configuration map to store key-value pairs
    std::map<String, String> config;

    // Initialize BLE
    void initializeBLE();
};

#endif
