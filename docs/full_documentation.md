# HinbitConfig Library Documentation

## Overview
The `HinbitConfig` library for Arduino enables easy configuration management and interaction using Bluetooth Low Energy (BLE). It utilizes the LittleFS file system to store configurations in a JSON file, providing an interface to set, get, and list configuration values. This library is designed for ESP32 boards and is suitable for applications requiring remote configuration updates or BLE-based communication.

## Features
- **Bluetooth Low Energy (BLE) Configuration:** Allows setting up a BLE service with characteristics for communication.
- **File System Management:** Manages configuration files using LittleFS for reliable storage.
- **Configuration Handling:** Supports reading and writing key-value pairs to a JSON file.
- **Default Configuration Creation:** Automatically generates a default configuration file if none exists.
- **Device Name Customization:** BLE device name can be customized based on the `DEVICE_NAME` value from the JSON configuration file.

## Project Structure
```
/HinbitConfigLibrary
│
├── HinbitConfig.h              # Header file defining the class and methods.
├── HinbitConfig.cpp            # Implementation file with method definitions.
├── library.properties          # Metadata file for the Arduino library.
├── README.md                   # Project overview and basic instructions.
├── examples
│   └── BasicUsage
│       └── BasicUsage.ino      # Basic example demonstrating library usage.
└── docs
    └── full_documentation.md   # Full documentation of the library.
```

## Installation
1. Download the repository as a ZIP file.
2. Open the Arduino IDE.
3. Go to **Sketch** -> **Include Library** -> **Add .ZIP Library...** and select the downloaded ZIP file.

Alternatively, copy the folder into the Arduino libraries directory:
- On Windows: `Documents\Arduino\libraries`
- On macOS: `~/Documents/Arduino/libraries`
- On Linux: `~/Arduino/libraries`

## Getting Started

### Including the Library
Include the library at the top of your Arduino sketch:
```cpp
#include <HinbitConfig.h>
```

### Initializing the Library
Create an instance of the `HinbitConfig` class and call `begin()` in the `setup()` function:
```cpp
HinbitConfig config;

void setup() {
  Serial.begin(115200);
  config.begin();  // Initializes BLE, LittleFS, and loads configuration.
}

void loop() {
  config.handleBLERequests();  // Handles BLE communication.
}
```

### Configuration File
The configuration is stored in a JSON file named `config.json` by default. The configuration file contains key-value pairs like Wi-Fi credentials, device name, and other settings.

### Example Configuration (`config.json`):
```json
{
    "WIFI": {
        "SSID": "MySSID",
        "PASS": "MyPassword"
    },
    "LAST_DATE": "01-10-1971",
    "DEVICE_NAME": "HINBIT_DEVICE",
    "CODE_VERSION": "1.6"
}
```

### Methods and Functions

#### `begin()`
Initializes BLE, the file system, and starts configuration setup.
```cpp
config.begin();
```

#### `setBLEDeviceActive()`
Activates the BLE device with the specified configuration settings.

#### `handleBLERequests()`
Handles BLE terminal requests, including setting and getting configuration values.

#### `setConfigValue(String key, String value)`
Sets a configuration value for the specified key.
```cpp
config.setConfigValue("WIFI-SSID", "MyNetwork");
```

#### `getConfigValue(String key)`
Gets the value of a specified configuration key.
```cpp
String ssid = config.getConfigValue("WIFI-SSID");
Serial.println("Wi-Fi SSID: " + ssid);
```

#### `listConfigKeys()`
Lists all keys in the configuration file.

#### `startConfig()`
Starts configuration mode and creates a default configuration if none exists.

#### `createDefaultConfig()`
Creates a default `config.json` file with standard values.

#### `readConfig(String key)`
Reads the configuration value associated with the given key.

#### `writeConfig(String key, String value)`
Writes the specified value to the given key in the configuration file.

#### `printConfig(String key = "")`
Prints the configuration for the specified key. If no key is provided, it prints the entire configuration.

#### `listLittleFSFiles()`
Lists all files stored in the LittleFS file system.

#### `formatLittleFS()`
Formats the LittleFS file system, erasing all stored files.

### Example Sketch
Here’s a complete example demonstrating basic usage of the `HinbitConfig` library:

```cpp
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
```

### Troubleshooting
1. **Failed to Mount LittleFS**: If LittleFS fails to mount, the library will attempt to format it. If formatting also fails, check for hardware issues or retry after a reset.
2. **BLE Communication Issues**: Ensure the BLE device is properly initialized and the BLE service is started before attempting to communicate.

### License
This library is licensed under the MIT License. See the `LICENSE` file for more information.

---

This documentation provides an in-depth look at the library’s structure, methods, and usage examples. If you need any additional sections or modifications, feel free to let me know!
